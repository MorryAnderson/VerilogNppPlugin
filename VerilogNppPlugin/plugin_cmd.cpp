#include "plugin_cmd.h"
#include "plugin_agent.h"
#include "menu_cmd.h"
#include "verilog_cmd.h"

static const TCHAR VERILOG_LANG_NAME_1[] = _T("Verilog");
static const TCHAR VERILOG_LANG_NAME_2[] = _T("udf - VerilogHDL");

//-----------------------------------------//
//-- STEP 4. DEFINE ASSOCIATED FUNCTIONS --//
//-----------------------------------------//
void Enabled(){
    SetEnabled(!verilog.get_enabled());
}

void ReplaceModuleDeclaration(){
    int start(0), end(0);
    const char* formatted_code(nullptr);

    if (RetrieveAndParseModule(&start, &end) == false) return;

    verilog.GetFormattedCode(&formatted_code);
    editor.SetTargetRange(start, end);
    editor.ReplaceTarget(-1, formatted_code);
    ::MessageBoxW(nullptr, _T("module code block replaced"), kNppPluginName, MB_OK);
}

void SetEnabled(bool new_state){
    verilog.set_enabled(new_state);
    ::SendMessage(npp_data._nppHandle, NPPM_SETMENUITEMCHECK, static_cast<WPARAM>(GetFuncItem()[0]._cmdID), new_state);
}

void TrackLangName(){
    int lang_type(0);
    int lang_name_len(0);
    wchar_t* lang_name(nullptr);

    ::SendMessage(npp_data._nppHandle, NPPM_GETCURRENTLANGTYPE, 0, reinterpret_cast<LPARAM>(&lang_type));
    lang_name_len = ::SendMessage(npp_data._nppHandle, NPPM_GETLANGUAGENAME, static_cast<WPARAM>(lang_type), reinterpret_cast<LPARAM>(nullptr));
    lang_name = new wchar_t[static_cast<unsigned int>(lang_name_len+1)];
    ::SendMessage(npp_data._nppHandle, NPPM_GETLANGUAGENAME, static_cast<WPARAM>(lang_type), reinterpret_cast<LPARAM>(lang_name));

    if (wcscmp(lang_name, VERILOG_LANG_NAME_1) == 0 || wcscmp(lang_name, VERILOG_LANG_NAME_2) == 0) {
        SetEnabled(true);
    } else {
        SetEnabled(false);
    }

    delete lang_name;
    lang_name = nullptr;
}

int RetrieveModuleBlock(char** code, int *start, int *end){
    int start_pos(0), end_pos(0);
    int search_flag = editor.GetSearchFlags();

    editor.SetSearchFlags(SCFIND_WHOLEWORD);
    editor.TargetWholeDocument();
    start_pos = editor.SearchInTarget("module");
    if (-1 == start_pos) return 0;

    editor.SetSearchFlags(SCFIND_NONE);
    editor.TargetWholeDocument();
    editor.SetTargetStart(start_pos);
    end_pos = editor.SearchInTarget(1, ";");
    if (-1 == end_pos) return 0;

    int module_len = ++end_pos - start_pos;

    *code = new char[static_cast<unsigned int>(module_len+1)];
    Sci_TextRange range = {{start_pos, end_pos}, *code};
    editor.GetTextRange(&range);

    editor.SetSearchFlags(search_flag);

    if (start) *start = start_pos;
    if (end) *end = end_pos;
    return module_len;
}

bool RetrieveAndParseModule(int *start, int *end){
    char* code(nullptr);
    int start_pos(0), end_pos(0);

    if (RetrieveModuleBlock(&code, &start_pos, &end_pos) == 0) {
        ::MessageBoxW(nullptr, verilog.GetMessageNoModule(), kNppPluginName, MB_OK);
        return false;
    }

    if (!verilog.ParseModule(code)){
        Verilog::ModuleParser::GrammarError error;
        int error_pos = verilog.GetLastError(&error) + start_pos;
        int line = editor.LineFromPosition(error_pos) + 1;
        int column = editor.GetColumn(error_pos) + 1;
        verilog.GetErrorMessage(error, line, column);
        editor.GotoPos(error_pos);
        ::MessageBoxW(nullptr, verilog.GetErrorMessage(error, line, column), kNppPluginName, MB_OK);
        return false;
    }
    if (start) *start = start_pos ;
    if (end) *end = end_pos;
    delete code;
    return true;
}

void CopyInstantiationTemplate(){
    if (RetrieveAndParseModule() == false) return;
    const char* instantiastion_code(nullptr);
    int length = verilog.GetInstantiationTemplate(&instantiastion_code);
    editor.CopyText(length, instantiastion_code);
    ::MessageBoxW(nullptr, L"Instantiation Code Copied to Clipboard", kNppPluginName, MB_OK);
}

void CreateTestbench(){
    if (RetrieveAndParseModule() == false) return;
    const char* testbench_code(nullptr);
    verilog.GetTestbenchTemplate(&testbench_code);
    ::SendMessage(npp_data._nppHandle, NPPM_MENUCOMMAND, 0, IDM_FILE_NEW);
    editor.SetText(testbench_code);
}

void AlignPortList(){
    int current_line = editor.LineFromPosition(editor.GetCurrentPos());
    char c = GetFirstCharInLine(current_line);
    if (c != '.') return;
    int line_count = editor.GetLineCount();
    int start_line(current_line), end_line(current_line);

    for (int i = current_line-1; i >= 0; --i) {
        if (GetFirstCharInLine(i) != '.') {
            start_line = i + 1;
            break;
        }
    }
    for (int i = current_line+1; i < line_count; ++i) {
        if (GetFirstCharInLine(i) != '.') {
            end_line = i - 1;
            break;
        }
    }

    int start_pos = editor.PositionFromLine(start_line);
    int end_pos = editor.GetLineEndPosition(end_line);
    editor.SetTargetStart(start_pos);
    editor.SetTargetEnd(end_pos);

    char* code(nullptr);
    code = new char[static_cast<unsigned int>(end_pos - start_pos + 2)];
    Sci_TextRange range = {{start_pos, end_pos}, code};
    editor.GetTextRange(&range);

    char* aligned_code(nullptr);
    verilog.AlignPortList(code, &aligned_code);
    editor.SetTargetRange(start_pos, end_pos);
    editor.ReplaceTarget(-1, aligned_code);

    delete aligned_code;
    delete code;
}

char GetFirstCharInLine(int line){
    int start_pos = editor.PositionFromLine(line);
    int end_pos = editor.GetLineEndPosition(line);
    char c('\0');
    for (int i = start_pos; i <= end_pos; ++i) {
        c = static_cast<char>(editor.GetCharAt(i));
        if (c != ' ') return c;
    }
    return '\0';
}
