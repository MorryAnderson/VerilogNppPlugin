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
        "module Spectrum_v1_0_M00_AXIS # \r\n"
        "( \r\n"
        "    // Users to add parameters here \r\n"
        " \r\n"
        "    // User parameters ends \r\n"
        "    // Do not modify the parameters beyond this line \r\n"
        " \r\n"
        "    // Width of S_AXIS address bus. The slave accepts the read and write addresses of width C_M_AXIS_TDATA_WIDTH. \r\n"
        "    parameter integer C_M_AXIS_TDATA_WIDTH	= 32, \r\n"
        "    // Start count is the number of clock cycles the master will wait before initiating/issuing any transaction. \r\n"
        "    parameter integer C_M_START_COUNT	= 32 \r\n"
        ") \r\n"
        "( \r\n"
        "    // Users to add ports here \r\n"
        " \r\n"
        "    // User ports ends \r\n"
        "    // Do not modify the ports beyond this line \r\n"
        " \r\n"
        "    // Global ports \r\n"
        "    input wire  M_AXIS_ACLK, \r\n"
        "    // \r\n"
        "    input wire  M_AXIS_ARESETN, \r\n"
        "    // Master Stream Ports. TVALID indicates that the master is driving a valid transfer, A transfer takes place when both TVALID and TREADY are asserted. \r\n"
        "    output wire  M_AXIS_TVALID, \r\n"
        "    // TDATA is the primary payload that is used to provide the data that is passing across the interface from the master. \r\n"
        "    output wire [C_M_AXIS_TDATA_WIDTH-1 : 0] M_AXIS_TDATA, \r\n"
        "    // TSTRB is the byte qualifier that indicates whether the content of the associated byte of TDATA is processed as a data byte or a position byte. \r\n"
        "    output wire [(C_M_AXIS_TDATA_WIDTH/8)-1 : 0] M_AXIS_TSTRB, \r\n"
        "    // TLAST indicates the boundary of a packet. \r\n"
        "    output wire  M_AXIS_TLAST, \r\n"
        "    // TREADY indicates that the slave can accept a transfer in the current cycle. \r\n"
        "    input wire  M_AXIS_TREADY \r\n"
        ");"
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
       "wire [63:0] flash_id;\n"
       "wire flash_id_valid;"
    };
    //*/
    char* aligned_code(nullptr);
    Verilog::Align align;
    align.AddDelimeter("[", 1, 0);
    align.AddDelimeter(":", 0, 0);
    align.AddDelimeter("]", 0, 1);
    align.AddDelimeter(";", 1, 0);
    align.AddDelimeter("//", 2, 1);
    align.AlignVariableDecl(str, &aligned_code, 0);
    qDebug() << aligned_code;
#endif
    return a.exec();
}

