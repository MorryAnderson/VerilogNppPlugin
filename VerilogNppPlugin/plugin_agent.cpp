#include "plugin_agent.h"
#include "plugin_cmd.h"

//--------------------------------//
//-- STEP 1. DEFINE PLUGIN NAME --//
//--------------------------------//
const TCHAR kNppPluginName[] = TEXT("Verilog HDL");

//------------------------------------------//
//-- STEP 2. DEFINE PLUGIN COMMAND NUMBER --//
//------------------------------------------//
const int kFunCount = 2;

NppData npp_data;

static FuncItem func_item[kFunCount];

FuncItem* GetFuncItem(){return func_item;}

ScintillaEditor editor;

void PluginInit(HANDLE /*h_module*/){
}

void PluginCleanUp(){
}

void CommandMenuInit(){
    //---------------------------------------//
    //-- STEP 3. CUSTOMIZE PLUGIN COMMANDS --//
    //---------------------------------------//
    SetCommand(0, TEXT("Hello Notepad++"), Hello, nullptr, false);
    SetCommand(1, TEXT("Hello (with dialog)"), HelloDlg, nullptr, false);
}

void CommandMenuCleanUp(){
	// Don't forget to deallocate your shortcut here
}

bool SetCommand(size_t index, const TCHAR *cmd_name, PFUNCPLUGINCMD p_func, ShortcutKey *shortcut, bool check_on_init){
    if (index >= kFunCount) return false;
    if (!p_func) return false;

    lstrcpy(func_item[index]._itemName, cmd_name);
    func_item[index]._pFunc = p_func;
    func_item[index]._init2Check = check_on_init;
    func_item[index]._pShKey = shortcut;

    return true;
}

HWND GetCurrentScintilla(){
    int which = 0;
    SendMessage(npp_data._nppHandle, NPPM_GETCURRENTSCINTILLA, SCI_UNUSED, reinterpret_cast<LPARAM>(&which));
    return (which == 0) ? npp_data._scintillaMainHandle : npp_data._scintillaSecondHandle;
}
