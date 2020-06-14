#include "module_parser.h"
using namespace Verilog;

ModuleParser::ModuleParser() : lexer_state_(LEXER_MODULE){
    module_structure_.ports.reserve(32);
    var_.type = KEYWORD_WIRE;
    var_.sign = KEYWORD_UNSGN;
    port_.dir = KEYWORD_INPUT;
    port_.var = var_;
    param_.var = var_;
}

ModuleParser::ModuleParser(const char * code) : ModuleParser() {
    ParseModule(code);
}

bool ModuleParser::ParseModule(const char * code){
    const char* code_it = code;
    char token[MAX_TOKEN_LEN] = {'\0'};
    char* token_it = token;

    int state(PARSER_EMPTY);
    bool head_of_line(true);
    bool is_opt(false);
    bool grammar_ok(true);

    module_structure_.params.clear();
    module_structure_.ports.clear();

    while (state != PARSER_END) {
        switch (state) {
        case PARSER_EMPTY: {
            if (*code_it == '\0') {
                state = PARSER_END;
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
            if (*code_it == '/') while (*code_it != '\n') *(token_it++) = *(code_it++);
            *token_it = '\0';
            token_it = token;
            //
            grammar_ok = ModuleLexer(token, is_opt, head_of_line);
            if (!grammar_ok) return false;
            //
            head_of_line = false;
            break;
        }
        default: state = PARSER_END;
        }
    }
    return lexer_state_ == LEXER_END;
}

ModuleParser::GrammarError ModuleParser::GetLastError(){
    switch (lexer_state_) {
    case LEXER_END: return ERROR_NONE;
    case LEXER_MODULE_NAME: return ERROR_MODULE_NAME;
    case LEXER_PORT: return ERROR_PORT;
    case LEXER_PORT_DIR:
    case LEXER_VAR_TYPE:
    case LEXER_VAR_SIGN:
    case LEXER_VAR_BRACKET_L:
    case LEXER_VAR_NAME: return ERROR_VAR_NAME;
    case LEXER_PORT_END: return ERROR_PORT_END;
    case LEXER_PARAM_BRACKET: return ERROR_PARAM_BRACKET;
    case LEXER_PARAM_EQUAL: return ERROR_PARAM_EQUAL;
    default: return ERROR_NONE;
    }
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
            var_.type = token;
            break;
        } else {
            // determine default var type (wire)
            if (port_or_param == PORT_VAR) {
                var_.type = KEYWORD_WIRE;
            } else if (port_or_param == PARAM_VAR) {
               var_.type = KEYWORD_PARAM;
            }
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
            var_.sign = token;
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
        // reset variable properties
        var_ = Variable();
        var_.type = KEYWORD_WIRE;
        var_.sign = KEYWORD_UNSGN;
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
