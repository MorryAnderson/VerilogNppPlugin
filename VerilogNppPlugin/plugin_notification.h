#ifndef PLUGIN_NOTIFICATION_H
#define PLUGIN_NOTIFICATION_H

#include "scintilla.h"

namespace Notify {

// Notepad++ Notifications
void NppBeforeShutdown();
void NppBufferActivated(uptr_t buffer_id);
void NppCancelShutdown();
void NppDocOrderChanged(void* new_index, uptr_t buffer_id);
void NppFileBeforeClose(uptr_t buffer_id);
void NppFileBeforeDelete(uptr_t buffer_id);
void NppFileBeforeLoad();
void NppFileBeforeOpen(uptr_t buffer_id);
void NppFileBeforeRename(uptr_t buffer_id);
void NppFileBeforeSave(uptr_t buffer_id);
void NppFileClosed(uptr_t buffer_id);
void NppFileDeleteFailed(uptr_t buffer_id);
void NppFileDeleted(uptr_t buffer_id);
void NppFileLoadFailed(uptr_t buffer_id);
void NppFileOpened(uptr_t buffer_id);
void NppFileRenameCancel(uptr_t buffer_id);
void NppFileRenamed(uptr_t buffer_id);
void NppFileSaved(uptr_t buffer_id);
void NppLangChanged(uptr_t buffer_id);
void NppReadonlyChanged(void* buffer_id, uptr_t doc_status);
void NppReady();
void NppShortcutRemapped(void* shortcut, uptr_t cmd_id);
void NppShutdown();
void NppSnapshotDirtyFileLoaded(uptr_t buffer_id);
void NppToolbarModification();
void NppWordStylesUpdated(uptr_t buffer_id);

// Scintilla Notifications
void AutoCompleteCancelled();
void AutoCompleteCharDeleted();
void AutoCompleteCompleted(int position, const char* text);
void AutoCompleteSelection(int position, const char* text);
void AutoCompleteSelectionChange(int position, const char* text);
void CallTipClick();
void CharAdded(int ch);
void DoubleClick(int position, int line);
void DwellEnd(int x, int y);
void DwellStart(int x, int y);
void FocusChanged(bool focused);
void HotSpotClick(int position, int modifiers);
void HotSpotDoubleClick(int position, int modifiers);
void HotSpotReleaseClick(int position, int modifiers);
void IndicatorClick(int position, int modifiers);
void IndicatorRelease(int position, int modifiers);
void Key(int key);
void LinesAdded(int linesAdded);
void MacroRecord(int message, uptr_t wParam, sptr_t lParam);
void MarginClicked(int position, int modifiers, int margin);
void MarginRightClick(int position, int modifiers, int margin);
void Modified(int type, int position, int length, int linesAdded,
              const char* text, int line, int foldNow, int foldPrev);
void ModifyAttemptReadOnly();
void NeedShown(int position, int length);
void Painted();
void SavePointChanged(bool dirty);
void StyleNeeded(int position);
void TextAreaClicked(int line, int modifiers);
void UpdateUi(int updated);
void UriDropped(const char* uri);
void UserListSelection(); // Wants some args.
void Zoom(int zoom);

}

#endif // PLUGIN_NOTIFICATION_H
