
#include "cmdline.hpp"
#include "verilog/Verilog.hpp"
#include "verilog/VerilogReader.hpp"
#include "lef/LEF.hpp"
#include "lef/LEF_READER.hpp"
#include "def/DEFReader.hpp"
#include "def/DEFWriter.hpp"

int main(int argc, char* argv[]) {
    CmdLine cmdLine;
    if (!cmdLine.parse(argc, argv))
        return EXIT_FAILURE;

    //*
    verilog::Netlist        netlist;
    verilog::VerilogReader  verilogReader;
    if (!verilogReader.read(cmdLine.verilog, netlist))
        return EXIT_FAILURE;
    //*/

    //*
    lef::LEFData    lef;
    lef::LEF_READER lef_reader;
    for (const auto& lef_file : cmdLine.lefs) {
        if (!lef_reader.read(lef_file, lef)) {
            return EXIT_FAILURE;
        }
    }
    //*/

    //*
    def::DEF_File   def;
    def::DEFReader  defReader;
    if (!defReader.ReadDEF(cmdLine.def, def))
        return EXIT_FAILURE;
    //*/

    // Process loaded data

    //*
    def::DEFWriter defWriter;
    defWriter.OutDEF("D:\\out.def", def);
    //*/
    return EXIT_SUCCESS;

}