#ifndef PLUGININTERFACE_H
#define PLUGININTERFACE_H

#include "notepad_plus.h"
#include "scintilla_editor.h"

#define DLL_EXPORT extern "C" __declspec(dllexport)

DLL_EXPORT void setInfo(NppData);
DLL_EXPORT const TCHAR* getName();
DLL_EXPORT FuncItem* getFuncsArray(int* func_count);
DLL_EXPORT void beNotified(SCNotification* notify_code);
DLL_EXPORT LRESULT messageProc(UINT, WPARAM, LPARAM);
DLL_EXPORT BOOL isUnicode();

#endif //PLUGININTERFACE_H
