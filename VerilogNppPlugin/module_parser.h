#ifndef MODULE_PARSER_H
#define MODULE_PARSER_H

#include <QString>
#include <QVector>

namespace Verilog {

struct Variable {
    QString type;
    QString sign;
    QString range_left, range_right;
    QString name;
    QString tail_comment;
    QVector<QString> head_comment;
};

struct Parameter {
    Variable var;
    QString value;
};

struct Port {
    QString dir;
    Variable var;
};

struct Module {
    QString name;
    QVector<Parameter> params;
    QVector<Port> ports;
};

class ModuleParser {
  public:
    enum GrammarError {
        ERROR_NONE,
        ERROR_MODULE_NAME,
        ERROR_PORT,
        ERROR_VAR_NAME,
        ERROR_PORT_END,
        ERROR_PARAM_BRACKET,
        ERROR_PARAM_EQUAL
    };
    ModuleParser();
    ModuleParser(const char*);
    /// \note Each token's length (including line comment) cannot exceed MAX_TOKEN_LEN
    /// \warning Block comment not supported!
    bool ParseModule(const char*);
    GrammarError GetLastError();

  private:
    enum ParserState {
        PARSER_END,
        PARSER_LAST,
        PARSER_EMPTY,
        PARSER_APPEND,
        PARSER_TOKEN,
    };

    enum LexerState {
        LEXER_MODULE,
        LEXER_MODULE_NAME,
        LEXER_PORT,
        LEXER_PORT_DIR,
        LEXER_VAR_TYPE,
        LEXER_VAR_SIGN,
        LEXER_VAR_BRACKET_L,
        LEXER_VAR_RANGE_L,
        LEXER_VAR_RANGE_R,
        LEXER_VAR_NAME,
        LEXER_PORT_END,
        LEXER_PARAM_BRACKET,
        LEXER_PARAM_EQUAL,
        LEXER_PARAM_VALUE,
        LEXER_END
    };

    static const int MAX_TOKEN_LEN = 256;

    static constexpr char
        KEYWORD_MODULE[] = "module"     ,
        KEYWORD_INPUT[]  = "input"      ,
        KEYWORD_OUTPUT[] = "output"     ,
        KEYWORD_INOUT[]  = "inout"      ,
        KEYWORD_SIGNED[] = "signed"     ,
        KEYWORD_UNSGN[]  = "unsigned"   ,
        KEYWORD_WIRE[]   = "wire"       ,
        KEYWORD_REG[]    = "reg"        ,
        KEYWORD_PARAM[]  = "parameter"  ;

    static const char
        OPERATOR_LBK = '(',
        OPERATOR_RBK = ')',
        OPERATOR_LSB = '[',
        OPERATOR_RSB = ']',
        OPERATOR_CMM = ',',
        OPERATOR_CLN = ':',
        OPERATOR_SCL = ';',
        OPERATOR_NUM = '#',
        OPERATOR_SLH = '/',
        OPERATOR_EQL = '=';

    static constexpr char operators_[] = {
        OPERATOR_LBK,
        OPERATOR_RBK,
        OPERATOR_LSB,
        OPERATOR_RSB,
        OPERATOR_CMM,
        OPERATOR_CLN,
        OPERATOR_SCL,
        OPERATOR_NUM,
        OPERATOR_SLH,
        OPERATOR_EQL,
        '\0'
    };

    bool IsOpt(char);
    inline bool IsSpace(char);
    bool ModuleLexer(const QString&, bool is_opt, bool head_of_line);

    LexerState lexer_state_;
    Module module_structure_;
    Variable var_;
    Port port_;
    Parameter param_;
};

}
#endif // MODULE_PARSER_H
