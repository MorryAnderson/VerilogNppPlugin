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

void SetEnabled(bool new_state){
    verilog.set_enabled(new_state);
    ::SendMessage(npp_data._nppHandle, NPPM_SETMENUITEMCHECK, static_cast<WPARAM>(GetFuncItem()[0]._cmdID), new_state);
}

void GetLangName(){
    int lang_type(0);
    int lang_name_len(0);
    wchar_t* lang_name(nullptr);

    ::SendMessage(npp_data._nppHandle, NPPM_GETCURRENTLANGTYPE, 0, reinterpret_cast<LPARAM>(&lang_type));
    lang_name_len = ::SendMessage(npp_data._nppHandle, NPPM_GETLANGUAGENAME, static_cast<WPARAM>(lang_type), reinterpret_cast<LPARAM>(nullptr));
    lang_name = new wchar_t[static_cast<unsigned int>(lang_name_len+1)];
    ::SendMessage(npp_data._nppHandle, NPPM_GETLANGUAGENAME, static_cast<WPARAM>(lang_type), reinterpret_cast<LPARAM>(lang_name));

    if (wcscmp(lang_name, TEXT("udf - VerilogHDL")) == 0 || wcscmp(lang_name, TEXT("Verilog")) == 0) {
        SetEnabled(true);
    } else {
        SetEnabled(false);
    }

    delete lang_name;
    lang_name = nullptr;
}
