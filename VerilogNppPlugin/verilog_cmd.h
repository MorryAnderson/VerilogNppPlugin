#ifndef VERILOG_CMD_H
#define VERILOG_CMD_H

#include <tchar.h>

#define KEYWORD_STR_SIZE 2048

class VerilogCmd {
  public:
    VerilogCmd();
    VerilogCmd(const TCHAR*);
    inline bool get_enabled(){return enabled_;}
    inline void set_enabled(bool state){enabled_ = state;}
    inline int get_autocomplete_len(){return autocomplete_len_;}
    inline const char* get_keywords(){return keywords_;}
    void LoadIniFile(const TCHAR* dir);
  private:
    static constexpr TCHAR inifilename[] = L"\\verilog.ini";
    char keywords_[KEYWORD_STR_SIZE];
    bool enabled_;
    int autocomplete_len_;
};

#endif // VERILOG_CMD_H
