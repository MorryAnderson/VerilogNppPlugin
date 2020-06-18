#include "module_parser.h"
using namespace Verilog;

ModuleParser::ModuleParser() :
        lexer_state_(LEXER_MODULE),
        last_error_pos_(-1),
        max_dir_len_(0), max_type_len_(0),
        max_sign_len_(0), max_range_left_len_(0), max_range_right_len_(0),
        max_name_len_(0), max_value_len_(0),
        last_comma_pos_(0)
{
    module_structure_.ports.reserve(32);
    formatted_code_.reserve(2048);
    port_.dir = KEYWORD_INPUT;
}

ModuleParser::ModuleParser(const char * code) : ModuleParser() {
    ParseModule(code);
}

bool ModuleParser::ParseModule(const char * code){
    const char* code_it = code;
    const char* previous_code_it = code;
    char token[MAX_TOKEN_LEN] = {'\0'};
    char* token_it = token;

    int state(PARSER_EMPTY);
    bool head_of_line(true);
    bool is_opt(false);
    bool grammar_ok(true);

    lexer_state_ = LEXER_MODULE;

    module_structure_.params.clear();
    module_structure_.ports.clear();

    while (state != PARSER_END) {
        switch (state) {
        case PARSER_EMPTY: {
            if (*code_it == '\0') {
                state = PARSER_END;
            } else if (*code_it == '\r') {
                head_of_line = true;
                ++code_it;
                ++code_it;  // \r\n
            } else if (*code_it == '\n') {
                head_of_line = true;
                ++code_it;
            } else if (IsSpace(*code_it)) {
                do {++code_it;} while(IsSpace(*code_it));
            } else if (IsOpt(*code_it)) {
                *(token_it++) = *(code_it++);
                state = PARSER_TOKEN;
                is_opt = true;
            } else {
                state = PARSER_APPEND;
                *(token_it++) = *(code_it++);
                is_opt = false;
            }
            break;
        }
        case PARSER_APPEND: {
            if (*code_it == '\0') {
                state = PARSER_LAST;
            } else if (*code_it == '\r') {
                head_of_line = true;
                ++code_it;
                ++code_it;  // \r\n
            } else if (*code_it == '\n') {
                head_of_line = true;
                ++code_it;
                state = PARSER_TOKEN;
            } else if (IsSpace(*code_it)) {
                do {++code_it;} while(IsSpace(*code_it));
                state = PARSER_TOKEN;
            } else if (IsOpt(*code_it)) {
                state = PARSER_TOKEN;
            } else {
                *(token_it++) = *(code_it++);
            }
            break;
        }
        case PARSER_LAST:
        case PARSER_TOKEN: {
            state = (state == PARSER_LAST) ? PARSER_END : PARSER_EMPTY;
            // special process for line comments
            if (token[0] == '/') while (*code_it != '\n') *(token_it++) = *(code_it++);
            *token_it = '\0';
            token_it = token;
            // lexer, check grammar
            grammar_ok = ModuleLexer(QString::fromUtf8(token), is_opt, head_of_line);
            if (!grammar_ok) {
                last_error_pos_ = previous_code_it - code;
                return false;
            }
            //
            head_of_line = false;
            previous_code_it = code_it;
            break;
        }
        default: state = PARSER_END;
        }
    }
    Q_ASSERT(lexer_state_ == LEXER_END);
    last_error_pos_ = -1;
    return lexer_state_ == LEXER_END;
}

int ModuleParser::GetLastError(GrammarError* error)const{
    if (error != nullptr) {
        switch (lexer_state_) {
        case LEXER_END: *error = ERROR_NONE; break;
        case LEXER_MODULE_NAME: *error = ERROR_MODULE_NAME; break;
        case LEXER_PORT: *error = ERROR_PORT; break;
        case LEXER_PORT_DIR:
        case LEXER_VAR_TYPE:
        case LEXER_VAR_SIGN:
        case LEXER_VAR_BRACKET_L:
        case LEXER_VAR_NAME: *error = ERROR_VAR_NAME; break;
        case LEXER_PORT_END: *error = ERROR_PORT_END; break;
        case LEXER_PARAM_BRACKET: *error = ERROR_PARAM_BRACKET; break;
        case LEXER_PARAM_EQUAL: *error = ERROR_PARAM_EQUAL; break;
        default: *error = ERROR_NONE; break;
        }
    }
    return last_error_pos_;
}

int ModuleParser::GetFormattedCode(const char **buffer){
    formatted_code_.clear();
    if(Q_UNLIKELY(GetLastError() != -1)) return 0;
    Parameter param;
    Port port;
    max_dir_len_ = max_type_len_ = max_sign_len_ = 0;
    max_range_left_len_ = max_range_right_len_ = max_name_len_ = 0;
    max_value_len_ = 0;
    last_comma_pos_ = 0;
    //
    formatted_code_.append(KEYWORD_MODULE);
    formatted_code_.append(" ");
    formatted_code_.append(module_structure_.name);
    formatted_code_.append(" ");

    if (!module_structure_.params.isEmpty()) {
        foreach (param, module_structure_.params) {
            max_sign_len_        = qMax(max_sign_len_,        param.var.sign.length());
            max_range_left_len_  = qMax(max_range_left_len_,  param.var.range_left.length());
            max_range_right_len_ = qMax(max_range_right_len_, param.var.range_right.length());
            max_name_len_        = qMax(max_name_len_,        param.var.name.length());
            max_value_len_       = qMax(max_value_len_,       param.value.length());
        }
        formatted_code_.append("#( ");
        formatted_code_.append(KEYWORD_PARAM);

        //! [align params]
        foreach (param, module_structure_.params) {
            formatted_code_.append("\n");
            // head comment
            InsertHeadComments(param);
            // sign
            formatted_code_.append(TAB);
            if (max_sign_len_ > 0) formatted_code_.append(param.var.sign.leftJustified(max_sign_len_+1));  // +1 for tail space
            // range
            InsertRange(param);
            // name
            formatted_code_.append(param.var.name.leftJustified(max_name_len_+1));
            // value
            if (!param.value.isEmpty()) {
                formatted_code_.append(QString("= %1").arg(param.value.leftJustified(max_value_len_+1)));
            }
            formatted_code_.append(",");
            // tail comment
            InsertTailComments(param);
        }
        RemoveLastComma();
        formatted_code_.append(")");
        //! [align params]
    }

    formatted_code_.append("(");

    if (!module_structure_.ports.isEmpty()) {
        max_sign_len_ = max_range_left_len_ = max_range_right_len_ = max_name_len_ = 0;
        foreach (port, module_structure_.ports) {
            max_sign_len_        = qMax(max_sign_len_,        port.var.sign.length());
            max_range_left_len_  = qMax(max_range_left_len_,  port.var.range_left.length());
            max_range_right_len_ = qMax(max_range_right_len_, port.var.range_right.length());
            max_name_len_        = qMax(max_name_len_,        port.var.name.length());
            max_dir_len_         = qMax(max_dir_len_,         port.dir.length());
            max_type_len_        = qMax(max_type_len_,        port.var.type.length());
        }
        //! [align ports]
        foreach (port, module_structure_.ports) {
            formatted_code_.append("\n");
            // head comment
            InsertHeadComments(port);
            // dir & type & sign
            formatted_code_.append(TAB);
            if (max_dir_len_ > 0) formatted_code_.append(port.dir.leftJustified(max_dir_len_+1));
            if (max_type_len_ > 0) formatted_code_.append(port.var.type.leftJustified(max_type_len_+1));
            if (max_sign_len_ > 0) formatted_code_.append(port.var.sign.leftJustified(max_sign_len_+1));
            // range
            InsertRange(port);
            // name
            formatted_code_.append(port.var.name.leftJustified(max_name_len_+1));
            formatted_code_.append(",");
            // tail comment
            InsertTailComments(port);
        }
        RemoveLastComma();
        //! [align ports]
    }
    formatted_code_.append(");");
    formatted_uft8_code_ = formatted_code_.toUtf8();
    if (buffer) *buffer = formatted_uft8_code_.data();
    return formatted_uft8_code_.length();
}

bool ModuleParser::IsOpt(char c){
    const char* p(operators_);
    while (*p != '\0') if (c == *(p++)) return true;
    return false;
}

bool ModuleParser::IsSpace(char c){
    return c == ' ' || c == '\t';
}

bool ModuleParser::ModuleLexer(const QString& token, bool is_opt, bool head_of_line){
    enum {
        PORT_VAR,
        PARAM_VAR
    };
    static int port_or_param(PORT_VAR);

    switch (lexer_state_) {
    case LEXER_MODULE: {
        if (token == KEYWORD_MODULE) lexer_state_ = LEXER_MODULE_NAME;
        break;
    }
    case LEXER_MODULE_NAME:{
        if (is_opt) return false;  // grammar error
        lexer_state_ = LEXER_PORT;
        module_structure_.name = token;
        break;
    }
    case LEXER_PORT:{
        if (token == OPERATOR_LBK) {
            lexer_state_ = LEXER_PORT_DIR;
            port_or_param = PORT_VAR;
        } else if (token == OPERATOR_NUM) {
            lexer_state_ = LEXER_PARAM_BRACKET;
            port_or_param = PARAM_VAR;
        } else {
            return false;  // grammar error
        }
        break;
    }
    case LEXER_PORT_DIR: {
        if (token == KEYWORD_INPUT || token == KEYWORD_OUTPUT || token == KEYWORD_INOUT) {
            lexer_state_ = LEXER_VAR_TYPE;
            port_.dir = token;
            break;
        } else if (token == OPERATOR_RBK) {
            lexer_state_ = LEXER_END;
            break;
        }
    }
    [[clang::fallthrough]];
    case LEXER_VAR_TYPE: {
        if (token == KEYWORD_REG || token == KEYWORD_WIRE || token == KEYWORD_PARAM) {
            lexer_state_ = LEXER_VAR_SIGN;
            // omit "wire"
            if (token != KEYWORD_WIRE) var_.type = token;
            break;
        } else {
            // store line comments
            if (token.startsWith(OPERATOR_SLH)) {
               if (head_of_line) {
                   var_.head_comment.append(token);
               } else {
                   if (port_or_param == PORT_VAR) {
                       module_structure_.ports.last().var.tail_comment = token;
                   } else if (port_or_param == PARAM_VAR) {
                       module_structure_.params.last().var.tail_comment = token;
                   }
               }
               break;
            }
        }
    }
    [[clang::fallthrough]];
    case LEXER_VAR_SIGN: {
        if (token == KEYWORD_SIGNED || token == KEYWORD_UNSGN) {
            lexer_state_ = LEXER_VAR_BRACKET_L;
            // omit "unsigned"
            if (token != KEYWORD_UNSGN) var_.sign = token;
            break;
        }
    }
    [[clang::fallthrough]];
    case LEXER_VAR_BRACKET_L: {
        if (token == OPERATOR_LSB) {
            lexer_state_ = LEXER_VAR_RANGE_L;
            var_.range_left.clear();
            var_.range_right.clear();
            break;
        }
    }
    [[clang::fallthrough]];
    case LEXER_VAR_NAME: {
        if (is_opt) return false;  // grammar error
        if (token.startsWith("I_")) {
            port_.dir = KEYWORD_INPUT;
        } else if (token.startsWith("O_")) {
            port_.dir = KEYWORD_OUTPUT;
        } else if (token.startsWith("B_")) {
            port_.dir = KEYWORD_INOUT;
        }
        var_.name = token;
        if (port_or_param == PORT_VAR) {
            lexer_state_ = LEXER_PORT_END;
            port_.var = var_;
            module_structure_.ports.append(port_);
        } else if (port_or_param == PARAM_VAR) {
            lexer_state_ = LEXER_PARAM_EQUAL;
            param_.var = var_;
            param_.value.clear();
        }
        // reset variable properties. Note: port.dir is inherted
        var_ = Variable();
        break;
    }
    case LEXER_VAR_RANGE_L: {
        if (token == OPERATOR_CLN) {
            lexer_state_ = LEXER_VAR_RANGE_R;
        } else if (token == OPERATOR_RSB) {
            lexer_state_ = LEXER_VAR_NAME;
            if (var_.range_left.isEmpty()) break;
            var_.range_right = "0";
            bool is_num(false);
            int range_left_num = var_.range_left.toInt(&is_num);
            if (is_num) {
                var_.range_left.setNum(range_left_num-1);
            } else {
                var_.range_left.append("-1");
            }
        } else {
            var_.range_left.append(token);
        }
        break;
    }
    case LEXER_VAR_RANGE_R: {
        if (token == OPERATOR_RSB) {
            lexer_state_ = LEXER_VAR_NAME;
            if (var_.range_right.isEmpty()) var_.range_right = "0";
        } else {
            var_.range_right.append(token);
        }
        break;
    }
    case LEXER_PORT_END: {
        if (token == OPERATOR_CMM) {
            lexer_state_ = LEXER_PORT_DIR;
        } else if (token == OPERATOR_RBK) {
            lexer_state_ = LEXER_END;
        } else if (token.startsWith(OPERATOR_SLH)) {
            lexer_state_ = LEXER_PORT_END;
            module_structure_.ports.last().var.tail_comment = token;
        } else {
            return false;  // grammar error
        }
        break;
    }
    // param
    case LEXER_PARAM_BRACKET: {
        if (token == OPERATOR_LBK) lexer_state_ = LEXER_VAR_TYPE;
        else return false;  // grammar error
        break;
    }
    case LEXER_PARAM_EQUAL: {
        if (token == OPERATOR_EQL) {
            lexer_state_ = LEXER_PARAM_VALUE;
        } else {
            if (token == OPERATOR_CMM) {
                lexer_state_ = LEXER_VAR_TYPE;
            } else if (token == OPERATOR_RBK) {
                lexer_state_ = LEXER_PORT;
            } else if (token.startsWith(OPERATOR_SLH)) {
                lexer_state_ = LEXER_PORT;
                param_.var.tail_comment = token;
            }  else {
                return false;  // grammar error
            }
            module_structure_.params.append(param_);
        }
        break;
    }
    case LEXER_PARAM_VALUE: {
        if (token == OPERATOR_CMM) {
            lexer_state_ = LEXER_VAR_TYPE;
            module_structure_.params.append(param_);
        } else if (token == OPERATOR_RBK) {
            lexer_state_ = LEXER_PORT;
            module_structure_.params.append(param_);
        } else {
            param_.value.append(token);
        }
        break;
    }
    case LEXER_END: {
        break;
    }
    //default: break;
    }
    return true;
}

template<typename P>
void ModuleParser::InsertHeadComments(const P& p){
    QString comment;
    foreach (comment, p.var.head_comment) {
        formatted_code_.append(TAB);
        formatted_code_.append(comment);
        formatted_code_.append("\n");
    }
}

template<typename P>
void ModuleParser::InsertRange(const P& p){
    if (max_range_left_len_ + max_range_right_len_ > 0) {
        if (p.var.range_left.length() + p.var.range_right.length() > 0 ) {
            formatted_code_.append(QString("[%1:%2] ")
                                   .arg(p.var.range_left, max_range_left_len_)
                                   .arg(p.var.range_right, max_range_right_len_)
            );
        } else {
            formatted_code_.append(QString(max_range_left_len_ + max_range_right_len_ + 3 + 1, ' '));  // +3 for [:], +1 for tail space
        }
    }
}

template<typename P>
void ModuleParser::InsertTailComments(const P& p){
    formatted_code_.append("  ");  // 2 spaces
    formatted_code_.append(p.var.tail_comment);
    last_comma_pos_ = p.var.tail_comment.length() + 2;
}

void ModuleParser::RemoveLastComma(){
    last_comma_pos_ = formatted_code_.length() - last_comma_pos_ - 1;
    formatted_code_[last_comma_pos_] = ' ';
    formatted_code_.append("\n");
}
