#include "plugin_cmd.h"
#include "plugin_agent.h"
#include "menu_cmd.h"
#include "verilog_cmd.h"

//----------------------------------------------//
//-- STEP 4. DEFINE YOUR ASSOCIATED FUNCTIONS --//
//----------------------------------------------//
void Enabled(){
    SetEnabled(!verilog.get_enabled());
}


void Module(){
    char* code(nullptr);
    RetrieveModuleBlock(&code);
    verilog.ParseModule(code);
    ::MessageBoxA(nullptr, code, "module", MB_OK);
    delete code;
}

void SetEnabled(bool new_state){
    verilog.set_enabled(new_state);
    ::SendMessage(npp_data._nppHandle, NPPM_SETMENUITEMCHECK, static_cast<WPARAM>(GetFuncItem()[0]._cmdID), new_state);
}


void TrackLangName(){
    int lang_type(0);
    int lang_name_len(0);
    wchar_t* lang_name(nullptr);

    ::SendMessage(npp_data._nppHandle, NPPM_GETCURRENTLANGTYPE, 0, reinterpret_cast<LPARAM>(&lang_type));
    lang_name_len = ::SendMessage(npp_data._nppHandle, NPPM_GETLANGUAGENAME, static_cast<WPARAM>(lang_type), reinterpret_cast<LPARAM>(nullptr));
    lang_name = new wchar_t[static_cast<unsigned int>(lang_name_len+1)];
    ::SendMessage(npp_data._nppHandle, NPPM_GETLANGUAGENAME, static_cast<WPARAM>(lang_type), reinterpret_cast<LPARAM>(lang_name));

    if (wcscmp(lang_name, _T("udf - VerilogHDL")) == 0 || wcscmp(lang_name, _T("Verilog")) == 0) {
        SetEnabled(true);
    } else {
        SetEnabled(false);
    }

    delete lang_name;
    lang_name = nullptr;
}

int RetrieveModuleBlock(char** code, int *start, int *end){
    int start_pos(0), end_pos(0);
    int search_flag = editor.GetSearchFlags();

    editor.SetSearchFlags(SCFIND_WHOLEWORD);
    editor.TargetWholeDocument();
    start_pos = editor.SearchInTarget("module");
    if (-1 == start_pos) return 0;

    editor.SetSearchFlags(SCFIND_NONE);
    editor.TargetWholeDocument();
    editor.SetTargetStart(start_pos);
    end_pos = editor.SearchInTarget(1, ";");
    if (-1 == end_pos) return 0;

    int module_len = end_pos - start_pos;

    *code = new char[static_cast<unsigned int>(module_len+1)];
    Sci_TextRange range = {{start_pos, end_pos}, *code};
    editor.GetTextRange(&range);

    editor.SetSearchFlags(search_flag);

    if (start) *start = start_pos;
    if (end) *end = end_pos;
    return module_len;
}
