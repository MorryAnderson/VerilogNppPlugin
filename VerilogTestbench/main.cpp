#include <QCoreApplication>
#include <../VerilogNppPlugin/verilog_cmd.h>
#include <../VerilogNppPlugin/module_parser.h>
#include <../VerilogNppPlugin/align.h>
using namespace Verilog;
#include <QDebug>
#include <QString>
#include <Windows.h>

#define DEBUG_PARSER
//#define DEBUG_ALIGN

#include <QMap>

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
    Verilog::ModuleParser parser;
    VerilogCmd verilog(L"C:\\Users\\Morry\\AppData\\Roaming\\Notepad++\\plugins\\config");
    //

#ifdef DEBUG_PARSER
    //*
    const char code[] = {
        "module FileDataReader #(\r\n"
        "    parameter PAR_FRAME_SIZE = 128,\r\n"
        "    parameter PAR_DATA_WIDTH = 8,\r\n"
        "    //\r\n"
        "    parameter PAR_INDEX_WIDTH = clogb2((PAR_FRAME_SIZE))\r\n"
        ")(\r\n"
        "input integer fid,\r\n"
        "    I_CLK,\r\n"
        "    I_RST_N,\r\n"
        "    I_RDY,\r\n"
        "    O_VLD,\r\n"
        "    O_LST,\r\n"
        "    [PAR_DATA_WIDTH] O_DAT,\r\n"
        "    [PAR_INDEX_WIDTH] O_IDX,\r\n"
        "    O_EOF\r\n"
        ");\r\n"
    };
    //*/
    /*/
    const char code[] = {
        "module found(output reg a, input b);\n"
    };
    //*/
    /*
    const char code[] = {
        "module AbnormalDetector(\n"
        "    input clk, rst_N,   ///< clk = 50MHz\n"
        "    output reg O_mute,  ///< 高电平表示需要静音\n"
        "    // [Interface] IIS\n"
        "    input I_bclk,\n"
        "    input I_data,\n"
        "    input I_ws\n"
        ");\n"
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
         case ModuleParser::ERROR_PARAM_VALUE: qDebug("ERROR: invalid parameter value"); break;
    }
    const char* formatted_code(nullptr);
    parser.GetFormattedCode(&formatted_code);
    qDebug() << formatted_code << '\n';

    const char* instantiation_code(nullptr);
    parser.GetInstantiationTemplate(&instantiation_code);
    qDebug() << instantiation_code << '\n';

    const char* testbench_code(nullptr);
    parser.GetTestbenchTemplate(&testbench_code);
    qDebug() << testbench_code << '\n';
#endif
#ifdef DEBUG_ALIGN
    const char port_list[] = {
        "    .clk ( clk ),\n"
        "    .rst_N ( rst_N ), // comment\n"
        "    .O_mute ( O_mute ), \n"
        "    .I_bclk ( I_bclk ),  // 中文注释\n"
        "    .I_data ( I_data ),\n"
        "    .I_ws ( I_ws ) "
    };
    //*
    const char str[] = {
        "wire ram_CLK = I_AXIS_ACLK;//c\r\n"
        "wire A_ENA = I_S_AXIS_TVALID;// c\r\n"
        "wire A_WR_ENA = 1'b0;//c\r\n"
        "wire A_ADDR  = I_S_AXIS_TUSER[PAR_RAM_ADDR_WIDTH-1:0];// c\r\n"
    };
    //*/
    char* aligned_code(nullptr);
    Verilog::Align align;
    align.AddDelimeter("[", 1, 0);
    align.AddDelimeter(":", 0, 0);
    align.AddDelimeter("]", 0, 1);
    align.AddDelimeter(";", 1, 0);
    align.AddDelimeter("///<", 2, 1);
    align.AddDelimeter("//", 2, 1);
    align.AlignVariableDecl(str, &aligned_code, 0);
    qDebug() << aligned_code;
#endif
    return a.exec();
}

