#ifndef PLUGININTERFACE_H
#define PLUGININTERFACE_H

#include "notepad_plus_msgs.h"
#include "scintilla_editor.h"

#define DLL_EXPORT extern "C" __declspec(dllexport)

typedef void (__cdecl *PFUNCPLUGINCMD)();

struct NppData {
	HWND _nppHandle;
	HWND _scintillaMainHandle;
	HWND _scintillaSecondHandle;
};

struct ShortcutKey {
	bool _isCtrl;
	bool _isAlt;
	bool _isShift;
	UCHAR _key;
};

struct FuncItem {
    TCHAR _itemName[64];
	PFUNCPLUGINCMD _pFunc;
	int _cmdID;
	bool _init2Check;
    ShortcutKey* _pShKey;
};

DLL_EXPORT void setInfo(NppData);
DLL_EXPORT const TCHAR* getName();
DLL_EXPORT FuncItem* getFuncsArray(int* func_count);
DLL_EXPORT void beNotified(SCNotification* notify_code);
DLL_EXPORT LRESULT messageProc(UINT, WPARAM, LPARAM);
DLL_EXPORT BOOL isUnicode();

#endif //PLUGININTERFACE_H
