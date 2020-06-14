#ifndef VERILOG_CMD_H
#define VERILOG_CMD_H

#include <tchar.h>
#include "module_parser.h"

class VerilogCmd {
  public:
    VerilogCmd();
    VerilogCmd(const TCHAR*);
    void LoadIniFile(const TCHAR*);
    inline bool get_enabled(){return enabled_;}
    inline void set_enabled(bool state){enabled_ = state;}
    inline int get_autocomplete_len(){return autocomplete_len_;}
    inline const char* get_keywords(){return keywords_;}
    //
    bool ParseModule(const char*);

  private:
    static const int KEYWORD_STR_SIZE = 2048;
    static constexpr TCHAR inifilename[] = L"\\verilog.ini";
    char keywords_[KEYWORD_STR_SIZE];
    bool enabled_;
    int autocomplete_len_;
    Verilog::ModuleParser module_parser_;
};

#endif // VERILOG_CMD_H
