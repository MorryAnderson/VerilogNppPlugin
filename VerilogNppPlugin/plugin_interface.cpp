#include "plugin_agent.h"

ScintillaEditor editor;

BOOL APIENTRY DllMain(HANDLE h_module, DWORD  reason_for_call, LPVOID /*lpReserved*/){
    switch (reason_for_call) {
      case DLL_PROCESS_ATTACH:
        PluginInit(h_module);
        break;

      case DLL_PROCESS_DETACH:
        PluginCleanUp();
        break;

      case DLL_THREAD_ATTACH:
        break;

      case DLL_THREAD_DETACH:
        break;
    }
    return TRUE;
}

void setInfo(NppData notpad_plus_data){
    npp_data = notpad_plus_data;
    // Set this as early as possible so it's in a valid state
    editor.SetScintillaInstance(npp_data._scintillaMainHandle);
    CommandMenuInit();
}

const TCHAR * getName(){
    return kNppPluginName;
}

FuncItem * getFuncsArray(int *func_count){
    *func_count = kFunCount;
    return GetFuncItem();
}

void beNotified(SCNotification* notify_code){
    switch (notify_code->nmhdr.code) {
        case NPPN_SHUTDOWN:	{
            CommandMenuCleanUp();
            break;
		}
        case NPPN_BUFFERACTIVATED: {
            editor.SetScintillaInstance(GetCurrentScintilla());
            break;
        }
        default: return;
	}
}

/// process the Npp Messages
LRESULT messageProc(UINT /*message*/, WPARAM /*w_param*/, LPARAM /*l_param*/){
    /*
    if (message == WM_MOVE)	{
        ::MessageBox(NULL, L"move", L"", MB_OK);
	}
    */
	return TRUE;
}

#ifdef UNICODE
BOOL isUnicode(){
    // This API return always true now, since Notepad++ isn't compiled in ANSI mode anymore
    return TRUE;
}
#endif //UNICODE
