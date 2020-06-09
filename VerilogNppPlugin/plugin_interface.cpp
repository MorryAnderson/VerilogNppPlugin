#include "plugin_agent.h"
#include "plugin_notification.h"

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

void beNotified(SCNotification* scn){
    switch (scn->nmhdr.code) {
        // Notepad++ Notifications

        /*
        case NPPN_BEFORESHUTDOWN:
            Notify::NppBeforeShutdown();
            break;
        //*/

        //*
        case NPPN_BUFFERACTIVATED:
            editor.SetScintillaInstance(GetCurrentScintilla());
            Notify::NppBufferActivated(scn->nmhdr.idFrom);
            break;
        //*/

        /*
        case NPPN_CANCELSHUTDOWN:
            Notify::NppCancelShutdown();
            break;
        //*/

        /*
        case NPPN_DOCORDERCHANGED:
            Notify::NppDocOrderChanged(scn->nmhdr.hwndFrom ,scn->nmhdr.idFrom);
            break;
        //*/

        /*
        case NPPN_FILEBEFORECLOSE:
            Notify::NppFileBeforeClose(scn->nmhdr.idFrom);
            break;
        //*/

        /*
        case NPPN_FILEBEFOREDELETE:
            Notify::NppFileBeforeDelete(scn->nmhdr.idFrom);
            break;
        //*/

        /*
        case NPPN_FILEBEFORELOAD:
            Notify::NppFileBeforeLoad();
            break;
        //*/

        /*
        case NPPN_FILEBEFOREOPEN:
            Notify::NppFileBeforeOpen(scn->nmhdr.idFrom);
            break;
        //*/

        /*
        case NPPN_FILEBEFORERENAME:
            Notify::NppFileBeforeRename(scn->nmhdr.idFrom);
            break;
        //*/

        /*
        case NPPN_FILEBEFORESAVE:
            Notify::NppFileBeforeSave(scn->nmhdr.idFrom);
            break;
        //*/

        /*
        case NPPN_FILECLOSED:
            Notify::NppFileClosed(scn->nmhdr.idFrom);
            break;
        //*/

        /*
        case NPPN_FILEDELETED:
            Notify::NppFileDeleted(scn->nmhdr.idFrom);
            break;
        //*/

        /*
        case NPPN_FILEDELETEFAILED:
            Notify::NppFileDeleteFailed(scn->nmhdr.idFrom);
            break;
        //*/

        /*
        case NPPN_FILELOADFAILED:
            Notify::NppFileLoadFailed(scn->nmhdr.idFrom);
            break;
        //*/

        /*
        case NPPN_FILEOPENED:
            Notify::NppFileOpened(scn->nmhdr.idFrom);
            break;
        //*/

        /*
        case NPPN_FILERENAMECANCEL:
            Notify::NppFileRenameCancel(scn->nmhdr.idFrom);
            break;
        //*/

        /*
        case NPPN_FILERENAMED:
            Notify::NppFileRenamed(scn->nmhdr.idFrom);
            break;
        //*/

        /*
        case NPPN_FILESAVED:
            Notify::NppFileSaved(scn->nmhdr.idFrom);
            break;
        //*/

        //*
        case NPPN_LANGCHANGED:
            Notify::NppLangChanged(scn->nmhdr.idFrom);
            break;
        //*/

        /*
        case NPPN_READONLYCHANGED:
            Notify::NppReadonlyChanged(scn->nmhdr.hwndFrom, scn->nmhdr.idFrom);
            break;
        //*/

        /*
        case NPPN_READY:
            Notify::NppReady();
            break;
        //*/

        /*
        case NPPN_SHORTCUTREMAPPED:
            Notify::NppShortcutRemapped(scn->nmhdr.hwndFrom, scn->nmhdr.idFrom);
            break;
        //*/

        //*
        case NPPN_SHUTDOWN:
            CommandMenuCleanUp();
            Notify::NppShutdown();
            break;
        //*/

        /*
        case NPPN_SNAPSHOTDIRTYFILELOADED:
            Notify::NppSnapshotDirtyFileLoaded(scn->nmhdr.idFrom);
            break;
        //*/

        /*
        case NPPN_TBMODIFICATION:
            Notify::NppToolbarModification();
            break;
        //*/

        /*
        case NPPN_WORDSTYLESUPDATED:
            Notify::NppWordStylesUpdated(scn->nmhdr.idFrom);
            break;
        //*/

        // Scintilla Notifications
        /*
        case SCN_AUTOCCANCELLED:
            Notify::AutoCompleteCancelled();
            break;
        //*/

        /*
        case SCN_AUTOCCHARDELETED:
            Notify::AutoCompleteCharDeleted();
            break;
        //*/

        /*
        case SCN_AUTOCCOMPLETED:
            Notify::AutoCompleteCompleted(scn->position, scn->text);
            break;
        //*/

        /*
        case SCN_AUTOCSELECTION:
            Notify::AutoCompleteSelection(scn->position, scn->text);
            break;
        //*/

        /*
        case SCN_AUTOCSELECTIONCHANGE:
            Notify::AutoCompleteSelectionChange(scn->lParam, scn->text);
            break;
        //*/

        /*
        case SCN_CALLTIPCLICK:
            Notify::CallTipClick();
            break;
        //*/

        /*
        case SCN_CHARADDED:
            Notify::CharAdded(scn->ch);
            break;
        //*/

        /*
        case SCN_DOUBLECLICK:
            Notify::DoubleClick(scn->position, scn->line);
            break;
        //*/

        /*
        case SCN_DWELLEND:
            Notify::DwellEnd(scn->x, scn->y);
            break;
        //*/

        /*
        case SCN_DWELLSTART:
            Notify::DwellStart(scn->x, scn->y);
            break;
        //*/

        /*
        case SCN_FOCUSIN:
            Notify::FocusChanged(true);
            break;
        //*/

        /*
        case SCN_FOCUSOUT:
            Notify::FocusChanged(false);
            break;
        //*/

        /*
        case SCN_HOTSPOTCLICK:
            Notify::HotSpotClick(scn->position, scn->modifiers);
            break;
        //*/

        /*
        case SCN_HOTSPOTDOUBLECLICK:
            Notify::HotSpotDoubleClick(scn->position, scn->modifiers);
            break;
        //*/

        /*
        case SCN_HOTSPOTRELEASECLICK:
            Notify::HotSpotReleaseClick(scn->position, scn->modifiers);
            break;
        //*/

        /*
        case SCN_INDICATORCLICK:
            Notify::IndicatorClick(scn->position, scn->modifiers);
            break;
        //*/

        /*
        case SCN_INDICATORRELEASE:
            Notify::IndicatorRelease(scn->position, scn->modifiers);
            break;
        //*/

        /*
        case SCN_KEY:
            Notify::Key(scn->ch);
            break;
        //*/

        /*
        case SCN_MACRORECORD:
            Notify::MacroRecord(scn->message, scn->wParam, scn->lParam);
            break;
        //*/

        /*
        case SCN_MARGINCLICK:
            Notify::MarginClicked(scn->position, scn->modifiers, scn->margin);
            break;
        //*/

        /*
        case SCN_MARGINRIGHTCLICK:
            Notify::MarginRightClick(scn->position, scn->modifiers, scn->margin);
            break;
        //*/

        /*
        case SCN_MODIFIED: {
            bool added = scn->modificationType & SC_MOD_INSERTTEXT;
            bool deleted = scn->modificationType & SC_MOD_DELETETEXT;

            int length = editor.GetTextLength();
            bool firstLineAdded = (added && length == 1) ||
                                  (deleted && length == 0);

            if (scn->linesAdded != 0) {
                LinesAdded(scn->linesAdded);
            } else if (firstLineAdded) {
                LinesAdded(added ? 1 : -1);
            }

            Modified(scn->modificationType, scn->position, scn->length,
                          scn->linesAdded, scn->text, scn->line,
                          scn->foldLevelNow, scn->foldLevelPrev);
            break;
        }
        //*/

        /*
        case SCN_MODIFYATTEMPTRO:
            Notify::ModifyAttemptReadOnly();
            break;
        //*/

        /*
        case SCN_NEEDSHOWN:
            Notify::NeedShown(scn->position, scn->length);
            break;
        //*/

        /*
        case SCN_PAINTED:
            Notify::Painted();
            break;
        //*/

        /*
        case SCN_SAVEPOINTLEFT:
            Notify::SavePointChanged(true);
            break;
        //*/

        /*
        case SCN_SAVEPOINTREACHED:
            Notify::SavePointChanged(false);
            break;
        //*/

        /*
        case SCN_STYLENEEDED:
            Notify::StyleNeeded(scn->position);
            break;
        //*/

        /*
        case SCN_UPDATEUI:
            Notify::UpdateUi(scn->updated);
            break;
        //*/

        /*
        case SCN_URIDROPPED:
            Notify::UriDropped(scn->text);
            break;
        //*/

        /*
        case SCN_USERLISTSELECTION:
            Notify::UserListSelection();
            break;
        //*/

        /*
        case SCN_ZOOM:
            Notify::Zoom(editor.GetZoom());
            break;
        //*/

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
