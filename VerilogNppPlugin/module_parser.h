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

    /// \return return true if no grammar error, otherwise false.
    ///         use GetLastError() to get error info.
    /// \note Each token's length (including line comment) cannot exceed MAX_TOKEN_LEN
    /// \warning Block comments not supported!
    bool ParseModule(const char*);

    /// \return The postion(offset) where the last error occured.
    /// \retval -1 No error occured
    /// \param error Type of last error.
    int GetLastError(GrammarError* error = nullptr)const;

    ///
    inline void SetOmitWire(bool omit = true){omit_wire_ = omit;}

    // the following functions use the result of ParseModule().
    // Make sure ParseModule() is successfullyy executed before calling those function.

    /// \details this function reassign the param "pointer" to an internal string buffer.
    /// \return length of code
    /// \param pointer a pointer of char* type
    int GetFormattedCode(const char** pointer);

    int GetInstantiationTemplate(const char** pointer);

    /// \note this function call GetInstantiationTemplate() internally.
    int GetTestbenchTemplate(const char** pointer);

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

    static constexpr char TAB[] = "    ";  // 4 spaces (1 tab)

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
    bool IsSpace(char);
    bool IsEOL(char);
    bool ModuleLexer(const QString&, bool is_opt, bool head_of_line);
    void SetMaxLen();
    template <typename P> void InsertHeadComments(const P& p);
    template <typename P> void InsertRange(const P&, QString&);
    template <typename P> void InsertTailComments(const P& p);
    void RemoveLastComma();

    LexerState lexer_state_;
    Module module_structure_;
    Variable var_;
    Port port_;
    Parameter param_;

    bool omit_wire_;
    QString formatted_code_;
    QByteArray formatted_uft8_code_;

    int last_error_pos_;

    int max_port_dir_len_, max_port_type_len_;
    int max_port_sign_len_, max_port_name_len_;
    int max_port_range_left_len_, max_port_range_right_len_;

    int max_param_value_len_, max_param_type_len_;
    int max_param_sign_len_, max_param_name_len_;
    int max_param_range_left_len_, max_param_range_right_len_;

    int last_comma_pos_;

    QString instantiation_template_;
    QByteArray instantiation_template_utf8_;

    QString testbench_template_;
    QByteArray testbench_template_utf8_;
};

}
#endif // MODULE_PARSER_H
