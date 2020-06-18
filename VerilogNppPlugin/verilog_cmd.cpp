#include "verilog_cmd.h"
#include <Windows.h>
#include <QTextStream>

VerilogCmd::VerilogCmd() : enabled_(false), autocomplete_len_(2){
    error_message_[0] = '\0';
    ERROR_NO_MODULE[0] = '\0';
    POS_OF_ERROR[0] = '\0';
    ERROR_NONE[0] = '\0';
    ERROR_MODULE_NAME[0] = '\0';
    ERROR_PORT[0] = '\0';
    ERROR_VAR_NAME[0] = '\0';
    ERROR_PORT_END[0] = '\0';
    ERROR_PARAM_BRACKET[0] = '\0';
    ERROR_PARAM_EQUAL[0] = '\0';
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

    if (0x02 == ::GetLastError()) ::MessageBox(nullptr, _T("ini file not found"), _T("WARNING"), MB_OK);

    wcstombs_s(nullptr, keywords_, KEYWORD_STR_SIZE, keywords_w, KEYWORD_STR_SIZE);

    // error messages
    ::GetPrivateProfileString(_T("error message"), _T("error_no_module"    ), L"", ERROR_NO_MODULE    , ERROR_MESSAGE_SIZE, inifilepath);
    ::GetPrivateProfileString(_T("error message"), _T("pos_of_error"       ), L"", POS_OF_ERROR       , ERROR_MESSAGE_SIZE, inifilepath);
    ::GetPrivateProfileString(_T("error message"), _T("error_none"         ), L"", ERROR_NONE         , ERROR_MESSAGE_SIZE, inifilepath);
    ::GetPrivateProfileString(_T("error message"), _T("error_module_name"  ), L"", ERROR_MODULE_NAME  , ERROR_MESSAGE_SIZE, inifilepath);
    ::GetPrivateProfileString(_T("error message"), _T("error_port"         ), L"", ERROR_PORT         , ERROR_MESSAGE_SIZE, inifilepath);
    ::GetPrivateProfileString(_T("error message"), _T("error_var_name"     ), L"", ERROR_VAR_NAME     , ERROR_MESSAGE_SIZE, inifilepath);
    ::GetPrivateProfileString(_T("error message"), _T("error_port_end"     ), L"", ERROR_PORT_END     , ERROR_MESSAGE_SIZE, inifilepath);
    ::GetPrivateProfileString(_T("error message"), _T("error_param_bracket"), L"", ERROR_PARAM_BRACKET, ERROR_MESSAGE_SIZE, inifilepath);
    ::GetPrivateProfileString(_T("error message"), _T("error_param_equal"  ), L"", ERROR_PARAM_EQUAL  , ERROR_MESSAGE_SIZE, inifilepath);
}
bool VerilogCmd::ParseModule(const char *code){
    return module_parser_.ParseModule(code);
}

int VerilogCmd::GetLastError(Verilog::ModuleParser::GrammarError* error)const{
    return module_parser_.GetLastError(error);
}

int VerilogCmd::GetFormattedCode(const char **buffer){
    return module_parser_.GetFormattedCode(buffer);
}

const TCHAR *VerilogCmd::GetErrorMessage(Verilog::ModuleParser::GrammarError error, int line, int column){
    const TCHAR* error_info;

    switch (error) {
        case Verilog::ModuleParser::ERROR_NONE:
        error_info = ERROR_NONE; break;
        case Verilog::ModuleParser::ERROR_MODULE_NAME:
        error_info = ERROR_MODULE_NAME; break;
        case Verilog::ModuleParser::ERROR_PORT:
        error_info = ERROR_PORT; break;
        case Verilog::ModuleParser::ERROR_VAR_NAME:
        error_info = ERROR_VAR_NAME; break;
        case Verilog::ModuleParser::ERROR_PORT_END:
        error_info = ERROR_PORT_END; break;
        case Verilog::ModuleParser::ERROR_PARAM_BRACKET:
        error_info = ERROR_PARAM_BRACKET; break;
        case Verilog::ModuleParser::ERROR_PARAM_EQUAL:
        error_info = ERROR_PARAM_EQUAL; break;
    }

    TCHAR error_pos[ERROR_MESSAGE_SIZE];
    swprintf_s(error_pos, ERROR_MESSAGE_SIZE, POS_OF_ERROR, line, column);

    lstrcpyW(error_message_, error_info);
    lstrcatW(error_message_, _T(" \n"));
    lstrcatW(error_message_, error_pos);

    return error_message_;
}
