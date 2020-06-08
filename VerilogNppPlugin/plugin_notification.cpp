#include "plugin_notification.h"

//===== Notepad++ Notification =====

void Notify::NppBeforeShutdown(){
}

void Notify::NppBufferActivated(uptr_t /* buffer_id */){
}

void Notify::NppCancelShutdown(){
}

void Notify::NppDocOrderChanged(void* /* new_index */, uptr_t /* buffer_id */){
}

void Notify::NppFileBeforeClose(uptr_t /* buffer_id */){
}

void Notify::NppFileBeforeDelete(uptr_t /* buffer_id */){
}

void Notify::NppFileBeforeLoad(){
}

void Notify::NppFileBeforeOpen(uptr_t /* buffer_id */){
}

void Notify::NppFileBeforeRename(uptr_t /* buffer_id */){
}

void Notify::NppFileBeforeSave(uptr_t /* buffer_id */){
}

void Notify::NppFileClosed(uptr_t /* buffer_id */){
}

void Notify::NppFileDeleteFailed(uptr_t /* buffer_id */){
}

void Notify::NppFileDeleted(uptr_t /* buffer_id */){
}

void Notify::NppFileLoadFailed(uptr_t /* buffer_id */){
}

void Notify::NppFileOpened(uptr_t /* buffer_id */){
}

void Notify::NppFileRenameCancel(uptr_t /* buffer_id */){
}

void Notify::NppFileRenamed(uptr_t /* buffer_id */){
}

void Notify::NppFileSaved(uptr_t /* buffer_id */){
}

void Notify::NppLangChanged(uptr_t /* buffer_id */){
}

void Notify::NppReadonlyChanged(void* /* buffer_id */, uptr_t /* doc_status */){
}

void Notify::NppReady(){
}

void Notify::NppShortcutRemapped(void* /* shortcut */, uptr_t /* cmd_id */){
}

void Notify::NppShutdown(){
}

void Notify::NppSnapshotDirtyFileLoaded(uptr_t /* buffer_id */){
}

void Notify::NppToolbarModification(){
}

void Notify::NppWordStylesUpdated(uptr_t /* buffer_id */){
}

//===== Scintilla Notifications =====

void Notify::AutoCompleteCancelled(){
}

void Notify::AutoCompleteCharDeleted(){
}

void Notify::AutoCompleteCompleted(int /*position*/, const char* /*text*/){
}

void Notify::AutoCompleteSelection(int /*position*/, const char* /*text*/){
    // QString::fromUtf8(text);
}

void Notify::AutoCompleteSelectionChange(int /*position*/, const char* /*text*/){
}

void Notify::CallTipClick(){
}

void Notify::CharAdded(int /*ch*/){
}

void Notify::DoubleClick(int /*position*/, int /*line*/){
}

void Notify::DwellEnd(int /*x*/, int /*y*/){
}

void Notify::DwellStart(int /*x*/, int /*y*/){
}

void Notify::FocusChanged(bool /*focused*/){
}

void Notify::HotSpotClick(int /*position*/, int /*modifiers*/){
}

void Notify::HotSpotDoubleClick(int /*position*/, int /*modifiers*/){
}

void Notify::HotSpotReleaseClick(int /*position*/, int /*modifiers*/){
}

void Notify::IndicatorClick(int /*position*/, int /*modifiers*/){
}

void Notify::IndicatorRelease(int /*position*/, int /*modifiers*/){
}

void Notify::Key(int /*key*/){
}

void Notify::LinesAdded(int /*linesAdded*/){
}

void Notify::MacroRecord(int /*message*/, uptr_t /*wParam*/, sptr_t /*lParam*/){
}

void Notify::MarginClicked(int /*position*/, int /*modifiers*/, int /*margin*/){
}

void Notify::MarginRightClick(int /*position*/, int /*modifiers*/, int /*margin*/){
}

void Notify::Modified(int /*type*/, int /*position*/, int /*length*/, int /*linesAdded*/,
              const char* /*text*/, int /*line*/, int /*foldNow*/, int /*foldPrev*/){
    // const QByteArray bytes = QByteArray::fromRawData(text, length);
}

void Notify::ModifyAttemptReadOnly(){
}

void Notify::NeedShown(int /*position*/, int /*length*/){
}

void Notify::Painted(){
}

void Notify::SavePointChanged(bool /*dirty*/){
}

void Notify::StyleNeeded(int /*position*/){
}

void Notify::TextAreaClicked(int /*line*/, int /*modifiers*/){
}

void Notify::UpdateUi(int /*updated*/){
}

void Notify::UriDropped(const char* /*uri*/){
    // QString::fromUtf8(uri);
}

void Notify::UserListSelection(){
}

void Notify::Zoom(int /*zoom*/){
}
