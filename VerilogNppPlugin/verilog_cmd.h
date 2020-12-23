#ifndef VERILOG_CMD_H
#define VERILOG_CMD_H

#include <tchar.h>
#include "module_parser.h"
#include "align.h"
#include <QMap>

struct VerilogTemplate {
    QString content;
    int cur_line;
    int cur_col;
};

class VerilogCmd {
  public:  // autocomplete
    VerilogCmd();
    VerilogCmd(const TCHAR*);
    void LoadIniFile(const TCHAR*);
    void LoadTemplates(const TCHAR*);
    bool GetTemplate(const char*, char**, int*, int*);
    void LoadSnippets(const TCHAR*);
    bool GetSnippet(const char*, char**);
    inline bool get_enabled()const{return enabled_;}
    inline void set_enabled(bool state){enabled_ = state;}
    inline int get_autocomplete_len()const{return autocomplete_len_;}
    inline const char* get_keywords()const{return keywords_;}
    inline const char* get_functions()const{return functions_;}
    inline const char* get_directives()const{return directives_;}
    inline const char* get_snippets()const{return snippets_;}

  public:  // module
    bool ParseModule(const char*);
    int GetLastError(Verilog::ModuleParser::GrammarError* error = nullptr)const;
    int GetFormattedCode(const char ** buffer = nullptr);
    int GetInstantiationTemplate(const char ** buffer = nullptr);
    int GetTestbenchTemplate(const char ** buffer = nullptr);
    const TCHAR* GetErrorMessage(Verilog::ModuleParser::GrammarError error, int line, int column);
    inline const TCHAR* GetMessageNoModule()const{return STR_ERROR_NO_MODULE;}

  public:  // align
    int AlignPortList(const char* code, char** aligned_code, int indent = 0);
    int AlignAssignment(const char* code, char** aligned_code, int indent = 0);
    int AlignUnblockingAssignment(const char* code, char** aligned_code, int indent = 0);
    int AlignVariableDecl(const char* code, char** aligned_code, int indent = 0);

  private:  // autocomplete
    bool enabled_;
    static const int KEYWORD_STR_SIZE = 2048;
    char keywords_[KEYWORD_STR_SIZE];
    char functions_[KEYWORD_STR_SIZE];
    char directives_[KEYWORD_STR_SIZE];
    char snippets_[KEYWORD_STR_SIZE];
    int autocomplete_len_;
    QMap<QString, VerilogTemplate> templates_map_;
    VerilogTemplate template_;
    QMap<QString, QString> snippets_map_;
    QString snippet_;

  private:  // module
    Verilog::ModuleParser module_parser_;
    static constexpr TCHAR inifilename[] = L"\\verilog.ini";
    static const int ERROR_MESSAGE_SIZE = 128;
    TCHAR STR_error_message_[ERROR_MESSAGE_SIZE];
    TCHAR STR_ERROR_NO_MODULE[ERROR_MESSAGE_SIZE];
    TCHAR STR_POS_OF_ERROR[ERROR_MESSAGE_SIZE];
    TCHAR STR_ERROR_NONE[ERROR_MESSAGE_SIZE];
    TCHAR STR_ERROR_MODULE_NAME[ERROR_MESSAGE_SIZE];
    TCHAR STR_ERROR_PORT[ERROR_MESSAGE_SIZE];
    TCHAR STR_ERROR_VAR_NAME[ERROR_MESSAGE_SIZE];
    TCHAR STR_ERROR_PORT_END[ERROR_MESSAGE_SIZE];
    TCHAR STR_ERROR_PARAM_BRACKET[ERROR_MESSAGE_SIZE];
    TCHAR STR_ERROR_PARAM_EQUAL[ERROR_MESSAGE_SIZE];
    TCHAR STR_ERROR_PARAM_VALUE[ERROR_MESSAGE_SIZE];

  private:  // align
    Verilog::Align instantiation_align_;
    Verilog::Align assignment_align_;
    Verilog::Align unblocking_assign_align_;
    Verilog::Align variable_align_;
};

#endif // VERILOG_CMD_H
