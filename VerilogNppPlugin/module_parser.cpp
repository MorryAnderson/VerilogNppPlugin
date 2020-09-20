#include "module_parser.h"
using namespace Verilog;

const char ENDL[3] = "\r\n";

ModuleParser::ModuleParser() : lexer_state_(LEXER_MODULE), last_error_pos_(0), last_comma_pos_(0){
    max_port_dir_len_ = max_port_type_len_ = 0;
    max_port_sign_len_ = max_port_name_len_ = 0;
    max_port_range_left_len_ = max_port_range_right_len_ = 0;

    max_param_value_len_ = max_param_type_len_ = 0;
    max_param_sign_len_ = max_param_name_len_ = 0;
    max_param_range_left_len_ = max_param_range_right_len_ = 0;

    last_comma_pos_ = 0;

    module_structure_.ports.reserve(32);
    formatted_code_.reserve(1024);
    formatted_uft8_code_.reserve(1024);
    instantiation_template_.reserve(512);
    instantiation_template_utf8_.reserve(512);
    testbench_template_.reserve(2048);
    testbench_template_utf8_.reserve(2048);
    port_.dir = KEYWORD_INPUT;
    omit_wire_ = false;
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
            if (token[0] == '/' && *code_it == '/') while (!IsEOL(*code_it)) *(token_it++) = *(code_it++);
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
    // Q_ASSERT(lexer_state_ == LEXER_END);
    SetMaxLen();
    if (lexer_state_ != LEXER_END) {
        last_error_pos_ = previous_code_it - code;;
        return false;
    } else {
        last_error_pos_ = -1;
        return true;
    }
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

int ModuleParser::GetFormattedCode(const char **pointer){
    formatted_code_.clear();
    if(Q_UNLIKELY(GetLastError() != -1)) return 0;
    Parameter param;
    Port port;
    last_comma_pos_ = 0;
    //
    formatted_code_.append(KEYWORD_MODULE);
    formatted_code_.append(" ");
    formatted_code_.append(module_structure_.name);
    formatted_code_.append(" ");

    if (!module_structure_.params.isEmpty()) {
        formatted_code_.append("#( ");
        //! [align params]
        foreach (param, module_structure_.params) {
            formatted_code_.append(ENDL);
            // head comment
            InsertHeadComments(param);
            // keyword
            formatted_code_.append(TAB);
            formatted_code_.append(KEYWORD_PARAM);
            formatted_code_.append(' ');
            // sign
            if (max_param_sign_len_ > 0) formatted_code_.append(param.var.sign.leftJustified(max_param_sign_len_+1));  // +1 for tail space
            // range
            InsertRange(param, formatted_code_);
            // name
            formatted_code_.append(param.var.name.leftJustified(max_param_name_len_+1));
            // value
            if (!param.value.isEmpty()) {
                formatted_code_.append(QString("= %1").arg(param.value.leftJustified(max_param_value_len_+1)));
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
        //! [align ports]
        foreach (port, module_structure_.ports) {
            formatted_code_.append(ENDL);
            // head comment
            InsertHeadComments(port);
            // dir & type & sign
            formatted_code_.append(TAB);
            if (max_port_dir_len_ > 0) formatted_code_.append(port.dir.leftJustified(max_port_dir_len_+1));
            if (max_port_type_len_ > 0) formatted_code_.append(port.var.type.leftJustified(max_port_type_len_+1));
            if (max_port_sign_len_ > 0) formatted_code_.append(port.var.sign.leftJustified(max_port_sign_len_+1));
            // range
            InsertRange(port, formatted_code_);
            // name
            formatted_code_.append(port.var.name.leftJustified(max_port_name_len_+1));
            formatted_code_.append(",");
            // tail comment
            InsertTailComments(port);
        }
        RemoveLastComma();
        //! [align ports]
    }
    formatted_code_.append(");");
    formatted_uft8_code_ = formatted_code_.toUtf8();
    if (pointer) *pointer = formatted_uft8_code_.data();
    return formatted_uft8_code_.length();
}

int ModuleParser::GetInstantiationTemplate(const char **pointer){
    instantiation_template_.clear();
    if(Q_UNLIKELY(GetLastError() != -1)) return 0;
    Parameter param;
    Port port;

    instantiation_template_.append(module_structure_.name);
    // params
    if (!module_structure_.params.isEmpty()) {
        instantiation_template_.append(" #(");
        foreach (param, module_structure_.params) {
            instantiation_template_.append(ENDL);
            instantiation_template_.append(QString("%1.%2 ( %3 ),").arg(TAB)
                                           .arg(param.var.name.leftJustified(max_param_name_len_))
                                           .arg(param.value.leftJustified(max_param_value_len_))
            );
        }
        instantiation_template_.chop(1);
        instantiation_template_.append(ENDL);
        instantiation_template_.append(")");
    }
    instantiation_template_.append(" INST_");
    instantiation_template_.append(module_structure_.name);
    // ports
    instantiation_template_.append(" (");
    if (!module_structure_.ports.isEmpty()) {
        foreach (port, module_structure_.ports) {
            instantiation_template_.append(ENDL);
            instantiation_template_.append(QString("%1.%2 ( %2 ),").arg(TAB)
                                           .arg(port.var.name.leftJustified(max_port_name_len_))
            );
        }
        instantiation_template_.chop(1);
        instantiation_template_.append(ENDL);
    }
    instantiation_template_.append(");");
    //
    instantiation_template_utf8_ = instantiation_template_.toUtf8();
    if (pointer) *pointer = instantiation_template_utf8_.data();
    return instantiation_template_utf8_.length();
}

int ModuleParser::GetTestbenchTemplate(const char **pointer){
    testbench_template_.clear();
    if(Q_UNLIKELY(GetLastError() != -1)) return 0;

    QString clock_CLK, reset_RST_N;

    //! [testbench module]
    testbench_template_.append("`timescale 1ns/1ns");
    testbench_template_.append(ENDL);
    testbench_template_.append(ENDL);
    testbench_template_.append("module TB_");
    testbench_template_.append(module_structure_.name);
    testbench_template_.append("();");
    testbench_template_.append(ENDL);
    //! [testbench module]

    //! [params]
    if (!module_structure_.params.empty()) {
        Parameter param;
        foreach (param, module_structure_.params) {
            testbench_template_.append(ENDL);
            testbench_template_.append(QString("localparam %1 = %2 ;")
                                       .arg(param.var.name.leftJustified(max_param_name_len_))
                                       .arg(param.value.leftJustified(max_param_value_len_))
            );
        }
        testbench_template_.append(ENDL);
    }
    //! [params]

    //! [variables]
    if (!module_structure_.ports.isEmpty()) {
        int max_type_len = 4;  // wire reg
        Port port;
        foreach (port, module_structure_.ports) {
            testbench_template_.append(ENDL);
            if (port.var.name.endsWith("_CLK")) clock_CLK = port.var.name;
            else if (port.var.name.endsWith("_RST_N")) reset_RST_N = port.var.name;
            // type
            if (port.dir == KEYWORD_INPUT) {
                testbench_template_.append(QString(KEYWORD_REG).leftJustified(max_type_len+1));
            } else {
                testbench_template_.append(QString(KEYWORD_WIRE).leftJustified(max_type_len+1));
            }

            // sign
            if (max_port_sign_len_ > 0) testbench_template_.append(port.var.sign.leftJustified(max_port_sign_len_+1));
            // range
            InsertRange(port, testbench_template_);
            // name
            testbench_template_.append(port.var.name.leftJustified(max_port_name_len_+1));
            if (port.dir == KEYWORD_INPUT) testbench_template_.append("= 0");
            testbench_template_.append(";");
        }
        testbench_template_.append(ENDL);
    }
    //! [variables]

    //! [instantiation]
    GetInstantiationTemplate(nullptr);
    testbench_template_.append(ENDL);
    testbench_template_.append(instantiation_template_);
    testbench_template_.append(ENDL);
    //! [instantiation]

    //! [initial]
    testbench_template_.append(ENDL);
    if (!clock_CLK.isEmpty()) {
        testbench_template_.append(QString("initial forever #(10/2) %1 = ~%1;").arg(clock_CLK));
        testbench_template_.append(ENDL);
        testbench_template_.append(ENDL);
    }
    if (!reset_RST_N.isEmpty()) {
        testbench_template_.append(QString("initial #10 %1 = 1'b1;").arg(reset_RST_N));
        testbench_template_.append(ENDL);
        testbench_template_.append(ENDL);
    }
    //! [initial]

    testbench_template_.append("endmodule");
    testbench_template_.append(ENDL);

    testbench_template_utf8_ = testbench_template_.toUtf8();
    if (pointer) *pointer = testbench_template_utf8_.data();
    return testbench_template_utf8_.length();
}

bool ModuleParser::IsOpt(char c){
    const char* p(operators_);
    while (*p != '\0') if (c == *(p++)) return true;
    return false;
}

bool ModuleParser::IsSpace(char c){
    return c == ' ' || c == '\t';
}

bool ModuleParser::IsEOL(char c){
    return c == '\r' || c == '\n' || c == '\0';
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
            var_.type = token;
            if (omit_wire_ && token == KEYWORD_WIRE) {
                var_.type = "";
            }
            break;
        } else {
            if (!omit_wire_) var_.type = KEYWORD_WIRE;
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
            } else if (token == OPERATOR_RBK) {
                if (port_or_param == PORT_VAR) {
                    lexer_state_ = LEXER_END;
                } else if (port_or_param == PARAM_VAR) {
                    lexer_state_ = LEXER_PORT;
                }
                break;
            }
        }
    }
    [[clang::fallthrough]];
    case LEXER_VAR_SIGN: {
        // integer type
        if (token == KEYWORD_INTEGER) {
            lexer_state_ = LEXER_VAR_NAME;
            var_.sign = token;
            break;
        }
        //
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
        } else if (token.startsWith(OPERATOR_SLH)) {
            lexer_state_ = LEXER_VAR_TYPE;
            param_.var.tail_comment = token;
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

void ModuleParser::SetMaxLen(){
    max_port_dir_len_ = max_port_type_len_ = 0;
    max_port_sign_len_ = max_port_name_len_ = 0;
    max_port_range_left_len_ = max_port_range_right_len_ = 0;

    max_param_value_len_ = max_param_type_len_ = 0;
    max_param_sign_len_ = max_param_name_len_ = 0;
    max_param_range_left_len_ = max_param_range_right_len_ = 0;

    Parameter param;
    Port port;

    foreach (param, module_structure_.params) {
        max_param_sign_len_        = qMax(max_param_sign_len_,        param.var.sign.length());
        max_param_range_left_len_  = qMax(max_param_range_left_len_,  param.var.range_left.length());
        max_param_range_right_len_ = qMax(max_param_range_right_len_, param.var.range_right.length());
        max_param_name_len_        = qMax(max_param_name_len_,        param.var.name.length());
        max_param_value_len_       = qMax(max_param_value_len_,       param.value.length());
    }

    foreach (port, module_structure_.ports) {
        max_port_sign_len_        = qMax(max_port_sign_len_,        port.var.sign.length());
        max_port_range_left_len_  = qMax(max_port_range_left_len_,  port.var.range_left.length());
        max_port_range_right_len_ = qMax(max_port_range_right_len_, port.var.range_right.length());
        max_port_name_len_        = qMax(max_port_name_len_,        port.var.name.length());
        max_port_dir_len_         = qMax(max_port_dir_len_,         port.dir.length());
        max_port_type_len_        = qMax(max_port_type_len_,        port.var.type.length());
    }
}

template<typename P>
void ModuleParser::InsertHeadComments(const P& p){
    QString comment;
    foreach (comment, p.var.head_comment) {
        formatted_code_.append(TAB);
        formatted_code_.append(comment);
        formatted_code_.append(ENDL);
    }
}

template<typename P>
void ModuleParser::InsertRange(const P& p, QString& code){
    int range_total_len(0);
    int max_range_left_len(0), max_range_right_len(0);
    if (typeid(P) == typeid(Port)) {
        range_total_len = max_port_range_left_len_ + max_port_range_right_len_;
        max_range_left_len = max_port_range_left_len_;
        max_range_right_len = max_port_range_right_len_;
    } else if (typeid(P) == typeid(Parameter)) {
        range_total_len = max_param_range_left_len_ + max_param_range_right_len_;
        max_range_left_len = max_param_range_left_len_;
        max_range_right_len = max_param_range_right_len_;
    }
    if (range_total_len > 0) {
        if (p.var.range_left.length() + p.var.range_right.length() > 0 ) {
            code.append(QString("[%1:%2] ")
                                   .arg(p.var.range_left, max_range_left_len)
                                   .arg(p.var.range_right, max_range_right_len)
            );
        } else {
            code.append(QString(range_total_len + 3 + 1, ' '));  // +3 for [:], +1 for tail space
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
    formatted_code_.append(ENDL);
}
