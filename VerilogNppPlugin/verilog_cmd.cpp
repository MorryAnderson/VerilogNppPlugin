#include "verilog_cmd.h"
#include <Windows.h>
#include <QTextStream>

VerilogCmd::VerilogCmd() : enabled_(false), autocomplete_len_(2){

}

VerilogCmd::VerilogCmd(const TCHAR* dir) : VerilogCmd(){
    LoadIniFile(dir);
}

void VerilogCmd::LoadIniFile(const TCHAR* dir){
    TCHAR inifilepath[MAX_PATH];
    TCHAR keywords_w[KEYWORD_STR_SIZE*2];

    lstrcpyW(inifilepath, dir);
    lstrcatW(inifilepath, inifilename);

    autocomplete_len_ = static_cast<int>(::GetPrivateProfileIntW(_T("autocomplete"), _T("length"), 2, inifilepath));
    ::GetPrivateProfileString(_T("autocomplete"), _T("keywords"), L"", keywords_w, KEYWORD_STR_SIZE*2, inifilepath);

    if (0x02 == GetLastError()) ::MessageBox(nullptr, _T("ini file not found"), _T("WARNING"), MB_OK);

    wcstombs_s(nullptr, keywords_, KEYWORD_STR_SIZE, keywords_w, KEYWORD_STR_SIZE);
}

bool VerilogCmd::ParseModule(const char *code){
    return module_parser_.ParseModule(code);
}
