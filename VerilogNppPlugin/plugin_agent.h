#ifndef PLUGINDEFINITION_H
#define PLUGINDEFINITION_H

#include "plugin_interface.h"
#include "verilog_cmd.h"

/// plugin name
extern const TCHAR kNppPluginName[];

/// the number of plugin commands
extern const int kFunCount;

/// the plugin commands data
FuncItem* GetFuncItem();

/// The data of Notepad++ used in plugin commands
extern NppData npp_data;

/// The class that represents the Scintilla Editor
extern ScintillaEditor editor;

extern VerilogCmd verilog;

/// Initialization of plugin data
/** @note It will be called while plugin loading */
void PluginInit(HANDLE);

/// Cleaning of plugin
/** Here you can do the clean up, save the parameters (if any) for the next session.
 *  @note It will be called while plugin unloading
 */
void PluginCleanUp();

/// Initialization of plugin commands
/** fill plugin commands here */
void CommandMenuInit();

/// Clean up plugin commands allocation (if any, especially for the shortcut)
void CommandMenuCleanUp();

/// Function which sets command
/**
 * @param index           zero based number to indicate the order of command
 * @param cmd_name        the command name that you want to see in plugin menu
 * @param p_func          the symbol of function (function pointer) associated with this command.
 * @param shortcut        optional. Define a shortcut to trigger this command
 * @param check_on_init  optional. Make this menu item be checked visually
 */
bool SetCommand(size_t index, const TCHAR *cmd_name, PFUNCPLUGINCMD p_func, ShortcutKey* shortcut = nullptr, bool check_on_init = false);


HWND GetCurrentScintilla();

#endif //PLUGINDEFINITION_H
