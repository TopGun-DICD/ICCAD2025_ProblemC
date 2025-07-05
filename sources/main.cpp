#include <iostream>

#include "cmdline.hpp"
#include "verilog/Verilog.hpp"
#include "verilog/VerilogReader.hpp"

int main(int argc, char *argv[]) {
    CmdLine cmdLine;
    if (!cmdLine.parse(argc, argv))
        return EXIT_FAILURE;

    verilog::Netlist        netlist;
    verilog::VerilogReader  verilogReader;
    if (!verilogReader.read(cmdLine.verilog, netlist))
        return EXIT_FAILURE;

    return EXIT_SUCCESS;
}