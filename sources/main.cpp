#include <iostream>

#include "cmdline.hpp"
#include "verilog/Verilog.hpp"
#include "verilog/VerilogReader.hpp"
#include "lef/LEF.hpp"


int main(int argc, char *argv[]) {
    CmdLine cmdLine;
    if (!cmdLine.parse(argc, argv))
        return EXIT_FAILURE;

    Netlist         netlist;
    VerilogReader   verilogReader;
    if (!verilogReader.read(cmdLine.verilog, netlist))
        return EXIT_FAILURE;

    LEF_READER lef_reader;

    return EXIT_SUCCESS;

}