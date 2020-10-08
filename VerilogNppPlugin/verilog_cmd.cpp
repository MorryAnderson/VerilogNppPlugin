#include "verilog_cmd.h"
#include <Windows.h>

VerilogCmd::VerilogCmd() : enabled_(false)/*, autocomplete_len_(2)*/{
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

    instantiation_align_.AddDelimeter(".", 0, 0);
    instantiation_align_.AddDelimeter("(", 1, 1);
    instantiation_align_.AddDelimeter(")", 1, 0);
    instantiation_align_.AddDelimeter(",", 0, 0);
    instantiation_align_.AddDelimeter("//", 2, 1);

    assignment_align_.AddDelimeter("=", 1, 1);
    assignment_align_.AddDelimeter(";", 0, 0);
    assignment_align_.AddDelimeter(",", 0, 0);
    assignment_align_.AddDelimeter("//", 2, 1);

    unblocking_assign_align_.AddDelimeter("<=", 1, 1);
    unblocking_assign_align_.AddDelimeter(";", 0, 0);
    unblocking_assign_align_.AddDelimeter(",", 0, 0);
    unblocking_assign_align_.AddDelimeter("//", 2, 1);

    variable_align_.AddDelimeter("[", 1, 0);
    variable_align_.AddDelimeter(":", 0, 0);
    variable_align_.AddDelimeter("]", 0, 1);
    variable_align_.AddDelimeter("=", 1, 1);
    variable_align_.AddDelimeter(";", 1, 0);
    variable_align_.AddDelimeter("///<", 2, 1);
    variable_align_.AddDelimeter("//", 2, 1);
}

VerilogCmd::VerilogCmd(const TCHAR* dir) : VerilogCmd(){
    LoadIniFile(dir);
}

void VerilogCmd::LoadIniFile(const TCHAR* dir){
    TCHAR inifilepath[MAX_PATH];
    TCHAR key_w[KEYWORD_STR_SIZE*2];

    lstrcpyW(inifilepath, dir);
    lstrcatW(inifilepath, inifilename);

    autocomplete_len_ = static_cast<int>(::GetPrivateProfileIntW(_T("autocomplete"), _T("length"), 2, inifilepath));

    if (0x02 == ::GetLastError()) {
        ::MessageBox(nullptr, _T("verilog.ini not found"), _T("WARNING"), MB_OK);
        return;
    }
    bool omit_wire = static_cast<bool>(::GetPrivateProfileIntW(_T("code"), _T("omit_wire"), 0, inifilepath));
    module_parser_.SetOmitWire(omit_wire);

    ::GetPrivateProfileString(_T("autocomplete"), _T("keywords"), L"", key_w, KEYWORD_STR_SIZE*2, inifilepath);
    wcstombs_s(nullptr, keywords_, KEYWORD_STR_SIZE, key_w, KEYWORD_STR_SIZE);

    ::GetPrivateProfileString(_T("autocomplete"), _T("functions"), L"", key_w, KEYWORD_STR_SIZE*2, inifilepath);
    wcstombs_s(nullptr, functions_, KEYWORD_STR_SIZE, key_w, KEYWORD_STR_SIZE);

    ::GetPrivateProfileString(_T("autocomplete"), _T("directives"), L"", key_w, KEYWORD_STR_SIZE*2, inifilepath);
    wcstombs_s(nullptr, directives_, KEYWORD_STR_SIZE, key_w, KEYWORD_STR_SIZE);

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

    // load templates
    LoadTemplates(inifilepath);
}

void VerilogCmd::LoadTemplates(const TCHAR* inifilepath){
    TCHAR content_w[KEYWORD_STR_SIZE*2];

    ::GetPrivateProfileStringW(_T("autocomplete"), _T("templates"), L"", content_w, KEYWORD_STR_SIZE*2, inifilepath);
    QStringList template_names = QString::fromWCharArray(content_w).split(" ");

    VerilogTemplate verilog_template;
    wchar_t section_name[128];
    QString key_name;
    QString template_content;
    int length(0);

    for (int i = 0; i < template_names.size(); ++i) {
        key_name = template_names.at(i);
        length = key_name.toWCharArray(section_name);
        section_name[length] = '\0';
        ::lstrcatW(section_name, L" T");

        ::GetPrivateProfileStringW(section_name, _T("content"), L"", content_w, 256, inifilepath);
        verilog_template.content = QString::fromWCharArray(content_w).replace("\\n", "\r\n");
        verilog_template.cur_line = static_cast<int>(::GetPrivateProfileIntW(section_name, _T("cur_line"), 0, inifilepath));
        verilog_template.cur_col = static_cast<int>(::GetPrivateProfileIntW(section_name, _T("cur_col"), 0, inifilepath));
        templates_map_.insert(key_name, verilog_template);
    }

}

bool VerilogCmd::GetTemplate(const char* key, char ** content, int *cur_line, int *cur_col){
    if (templates_map_.contains(key)) {
        template_ = templates_map_[key];
        *content = template_.content.toUtf8().data();
        *cur_line = template_.cur_line;
        *cur_col = template_.cur_col;
        return true;
    } else {
        return false;
    }
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

int VerilogCmd::GetInstantiationTemplate(const char **buffer){
    return module_parser_.GetInstantiationTemplate(buffer);
}

int VerilogCmd::GetTestbenchTemplate(const char **buffer){
    return module_parser_.GetTestbenchTemplate(buffer);
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

int VerilogCmd::AlignPortList(const char *code, char** aligned_code, int indent){
    return instantiation_align_.GetAlignedCode(code, aligned_code, indent);
}

int VerilogCmd::AlignAssignment(const char *code, char **aligned_code, int indent){
    return assignment_align_.GetAlignedCode(code, aligned_code, indent);
}

int VerilogCmd::AlignUnblockingAssignment(const char *code, char **aligned_code, int indent){
    return unblocking_assign_align_.GetAlignedCode(code, aligned_code, indent);
}

int VerilogCmd::AlignVariableDecl(const char *code, char **aligned_code, int indent){
    return variable_align_.AlignVariableDecl(code, aligned_code, indent);
}

/*
unsigned int VerilogCmd::AlignPortList(const char *code, char** aligned_code){
    // calc buffer size
    unsigned int line_count(1);
    unsigned int line_len(0);
    unsigned int port_len(0), sig_len(0);
    unsigned int max_port_len(0), max_sig_len(0);
    unsigned int comment_len(0);
    unsigned int buffer_size(0);
    char delimiters[3] = {'.', '(', ')'};

    enum {
        DOT,
        PORT,
        SIGNAL,
        EOL
    } state;

    const char* c = code;
    state = DOT;
    while (*c != '\0') {
        switch (state) {
        case DOT: {
            if (*c == delimiters[0]) state = PORT;
            break;
        }
        case PORT: {
            if (*c == delimiters[1]) {
                state = SIGNAL;
                max_port_len = max_port_len < port_len ? port_len : max_port_len;
                port_len = 0;
            } else {
                if (*c != ' ') ++port_len;
            }
            break;
        }
        case SIGNAL: {
            if (*c == delimiters[2]) {
                state = EOL;
                max_sig_len = max_sig_len < sig_len ? sig_len : max_sig_len;
                sig_len = 0;
            } else {
                if (*c != ' ') ++sig_len;
            }
            break;
        }
        case EOL: {
            if (*c == '\r') {
                state = DOT;
                ++line_count;
                ++c;  // skip \r\n
            } else if (*c == '\n') {
                state = DOT;
                ++line_count;
            } else {
                ++comment_len;
            }
            break;
        }
        }
        ++c;
    }
    line_len = max_port_len + max_sig_len + 4 + 6;  // tab(4) + ". (  )"
    buffer_size = line_len + 2;  // + "\r\n"
    buffer_size *= line_count;
    buffer_size += comment_len;
    *aligned_code = new char[buffer_size+1];

    // generate aligned code
    c = code;
    char* p = *aligned_code;
    char* start_of_line = *aligned_code;
    unsigned int dot_pos(4);
    unsigned int left_bracket_pos(dot_pos + max_port_len + 2);
    unsigned int right_bracket_pos(left_bracket_pos + max_sig_len + 3);

    for (unsigned int i = 0; i < line_count; ++i) {
        start_of_line = p;
        for (unsigned int j=0; j<right_bracket_pos+1; ++j) *(p++) = ' ';
        // write port name
        p = start_of_line + dot_pos + 1;
        while (*c != delimiters[0] && *c != '\0') ++c;
        if (*c == '\0') break; else ++c;
        while (*c == ' ' || *c == '\t') ++c;
        while (!(*c == delimiters[1] || *c == ' '  || *c == '\t' || *c == '\0')) {
            *(p++) = *(c++);
        }
        // write signal name
        p = start_of_line + left_bracket_pos + 2;
        while (*c != delimiters[1] && *c != '\0') ++c;
        if (*c == '\0') break; else ++c;
        while (*c == ' ' || *c == '\t') ++c;
        while (!(*c == delimiters[2] || *c == ' '  || *c == '\t' || *c == '\0')) {
            *(p++) = *(c++);
        }
        // write the rest of current line
        p = start_of_line + right_bracket_pos + 1;
        while (*c != delimiters[2] && *c != '\0') ++c;
        if (*c == '\0') break; else ++c;
        while (*c != '\n' && *c != '\0') {
            *(p++) = *(c++);
        }
        if (*c != '\0') *(p++) = '\n';
        // write .()
        start_of_line[dot_pos] = delimiters[0];
        start_of_line[left_bracket_pos] = delimiters[1];
        start_of_line[right_bracket_pos] = delimiters[2];
    }
    *p = '\0';
    return buffer_size + 1;
}
*/
