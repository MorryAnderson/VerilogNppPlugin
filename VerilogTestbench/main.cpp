#include <QCoreApplication>
#include <../VerilogNppPlugin/verilog_cmd.h>
#include <../VerilogNppPlugin/module_parser.h>
using namespace Verilog;
#include <QDebug>
#include <Windows.h>

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
    Verilog::ModuleParser parser;
    VerilogCmd verilog(L"C:\\Users\\Morry\\AppData\\Roaming\\Notepad++\\plugins\\config");
    /*
    const char code[] = {
        "module module_name #(WIDTH=8)(\n"
        "    I_CLK, I_RST_N, // 要静音 \n"
        "    [8] I_data_in,\n"
        "    [WIDTH] O_data_out,\n"
        "    // head comment\n"
        "    input port_1 ,\n"
        "    reg O_port_2,  // line comment\n"
        "    output reg signed [7:0] port_3 ,\n"
        "    inout  wire             port_4 // ??\n"
        ");"
    };
    //*/
    //*/
    const char code[] = {
        "module found #(a,b)();\n"
    };
    //*/
    parser.ParseModule(code);
    ModuleParser::GrammarError error;
    parser.GetLastError(&error);
    switch (error) {
        case ModuleParser::ERROR_NONE: qDebug("Parsing Complete: no error"); break;
        case ModuleParser::ERROR_MODULE_NAME: qDebug("ERROR: invalid module name"); break;
        case ModuleParser::ERROR_PORT: qDebug("ERROR: expected \"(\" before port list"); break;
        case ModuleParser::ERROR_VAR_NAME: qDebug("ERROR: invalid variable name"); break;
        case ModuleParser::ERROR_PORT_END: qDebug("ERROR: expected \",\" or \")\" after port declaration"); break;
        case ModuleParser::ERROR_PARAM_BRACKET: qDebug("ERROR: expected \"#(\" before param list"); break;
        case ModuleParser::ERROR_PARAM_EQUAL: qDebug("ERROR: expected \"=\" in param declaration"); break;
    }
    const char* formatted_code(nullptr);
    parser.GetFormattedCode(&formatted_code);
    qDebug() << formatted_code;
    return a.exec();
}

