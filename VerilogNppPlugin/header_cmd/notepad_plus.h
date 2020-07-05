#ifndef NOTEPAD_PLUS_H
#define NOTEPAD_PLUS_H

#include "notepad_plus_msgs.h"
#include "menu_cmd.h"

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

class NotepadPlus final {
  private:
    NppData npp_data_;

  public:
    NotepadPlus() {
        npp_data_._nppHandle = nullptr;
        npp_data_._scintillaMainHandle = nullptr;
        npp_data_._scintillaSecondHandle = nullptr;
    }

    explicit NotepadPlus(NppData notpad_plus_data) : NotepadPlus() {
        SetNppData(notpad_plus_data);
    }

    void SetNppData(NppData notepad_plus_data) {
        npp_data_ = notepad_plus_data;
    }

    NppData GetNppData() const {
        return npp_data_;
    }

    // commands
    LRESULT AddToolbarIcon(int cmd_id, toolbarIcons* icon){
        return ::SendMessage(npp_data_._nppHandle, NPPM_ADDTOOLBARICON,
                             static_cast<WPARAM>(cmd_id),
                             reinterpret_cast<LPARAM>(icon));
    }

    LRESULT GetCurrentLangType(int* lang_type){
        return ::SendMessage(npp_data_._nppHandle, NPPM_GETCURRENTLANGTYPE,
                             0, reinterpret_cast<LPARAM>(lang_type));
    }

    LRESULT GetCurrentScintilla(int* current_edit){
        return ::SendMessage(npp_data_._nppHandle, NPPM_GETCURRENTSCINTILLA,
                             0, reinterpret_cast<LPARAM>(&current_edit));
    }

    LRESULT GetLanguageName(int lang_type_id, TCHAR* buffer){
        return ::SendMessage(npp_data_._nppHandle, NPPM_GETLANGUAGENAME,
                             static_cast<WPARAM>(lang_type_id),
                             reinterpret_cast<LPARAM>(buffer));
    }

    LRESULT GetPluginsConfigDir(int srt_len, TCHAR *plugins_conf_dir){
        return ::SendMessage(npp_data_._nppHandle, NPPM_GETPLUGINSCONFIGDIR,
                             static_cast<WPARAM>(srt_len),
                             reinterpret_cast<LPARAM>(plugins_conf_dir));
    }

    LRESULT MenuCommand(int command_id){
        return ::SendMessage(npp_data_._nppHandle, NPPM_MENUCOMMAND, 0, command_id);

    }

    LRESULT SetMenuItemCheck(int cmd_id, BOOL do_check){
        return ::SendMessage(npp_data_._nppHandle, NPPM_SETMENUITEMCHECK,
                             static_cast<WPARAM>(cmd_id),
                             static_cast<LPARAM>(do_check));
    }

};

#endif // NOTEPAD_PLUS_H
