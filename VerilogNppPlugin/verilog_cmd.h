#ifndef VERILOG_CMD_H
#define VERILOG_CMD_H

#include <tchar.h>
#include "module_parser.h"

class VerilogCmd {
  public:  // autocomplete
    VerilogCmd();
    VerilogCmd(const TCHAR*);
    void LoadIniFile(const TCHAR*);
    inline bool get_enabled()const{return enabled_;}
    inline void set_enabled(bool state){enabled_ = state;}
    inline int get_autocomplete_len()const{return autocomplete_len_;}
    inline const char* get_keywords()const{return keywords_;}

  public:  // module
    bool ParseModule(const char*);
    int GetLastError(Verilog::ModuleParser::GrammarError* error = nullptr)const;
    int GetFormattedCode(const char ** buffer = nullptr);
    int GetInstantiationTemplate(const char ** buffer = nullptr);
    int GetTestbenchTemplate(const char ** buffer = nullptr);
    const TCHAR* GetErrorMessage(Verilog::ModuleParser::GrammarError error, int line, int column);
    inline const TCHAR* GetMessageNoModule()const{return ERROR_NO_MODULE;}

  public:  // align
    unsigned int AlignPortList(const char* code, char** aligned_code);

  private:  // autocomplete
    static const int KEYWORD_STR_SIZE = 2048;
    static constexpr TCHAR inifilename[] = L"\\verilog.ini";
    char keywords_[KEYWORD_STR_SIZE];
    static const int ERROR_MESSAGE_SIZE = 128;
    bool enabled_;
    int autocomplete_len_;

  private:  // module
    Verilog::ModuleParser module_parser_;
    TCHAR error_message_[ERROR_MESSAGE_SIZE];
    TCHAR ERROR_NO_MODULE[ERROR_MESSAGE_SIZE];
    TCHAR POS_OF_ERROR[ERROR_MESSAGE_SIZE];
    TCHAR ERROR_NONE[ERROR_MESSAGE_SIZE];
    TCHAR ERROR_MODULE_NAME[ERROR_MESSAGE_SIZE];
    TCHAR ERROR_PORT[ERROR_MESSAGE_SIZE];
    TCHAR ERROR_VAR_NAME[ERROR_MESSAGE_SIZE];
    TCHAR ERROR_PORT_END[ERROR_MESSAGE_SIZE];
    TCHAR ERROR_PARAM_BRACKET[ERROR_MESSAGE_SIZE];
    TCHAR ERROR_PARAM_EQUAL[ERROR_MESSAGE_SIZE];
};

#endif // VERILOG_CMD_H
