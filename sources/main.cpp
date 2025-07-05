
#include "cmdline.hpp"
#include "verilog/Verilog.hpp"
#include "verilog/VerilogReader.hpp"
#include "def/DEFReader.hpp"
#include "def/DEFWriter.hpp"

int main(int argc, char *argv[]) {
    CmdLine cmdLine;
    if (!cmdLine.parse(argc, argv))
        return EXIT_FAILURE;
   
    def::DEF_File   def;
    def::DEFReader  defReader;
    if (!defReader.ReadDEF(cmdLine.def, def))
        return EXIT_FAILURE;

    def::DEFWriter defWriter;
    defWriter.OutDEF("D:\\out.def", def);
   
    return EXIT_SUCCESS;
}