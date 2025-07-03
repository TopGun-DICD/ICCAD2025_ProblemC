#include <iostream>

#include "cmdline.hpp"
#include "verilog/Verilog.hpp"
#include "verilog/VerilogReader.hpp"
#include "lef/LEF.hpp"
#include "lef/LEF_READER.hpp"


int main(int argc, char* argv[]) {
    CmdLine cmdLine;
    if (!cmdLine.parse(argc, argv))
        return EXIT_FAILURE;

    /*
    Netlist         netlist;
    VerilogReader   verilogReader;
    if (!verilogReader.read(cmdLine.verilog, netlist))
        return EXIT_FAILURE;*/

    LEF_READER   lef_reader;
    for (const auto& lef_file : cmdLine.lefs) {
        if (!lef_reader.read(lef_file)) {
            return EXIT_FAILURE;
        }
    }

    return EXIT_SUCCESS;

}