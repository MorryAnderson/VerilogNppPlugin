#include "plugin_cmd.h"
#include "plugin_agent.h"
#include "menu_cmd.h"

//----------------------------------------------//
//-- STEP 4. DEFINE YOUR ASSOCIATED FUNCTIONS --//
//----------------------------------------------//
void Hello(){
    ::SendMessage(npp_data._nppHandle, NPPM_MENUCOMMAND, 0, IDM_FILE_NEW);
    editor.SetText("Hello, Notepad++!");
}

void HelloDlg(){
    ::MessageBox(nullptr, TEXT("Hello, Notepad++!"), TEXT("Notepad++ Plugin Template"), MB_OK);
}
