#include "DEFReader.hpp"
#include "DEFWriter.hpp"

#include "cmdline.hpp"
#include "verilog/Verilog.hpp"
#include "verilog/VerilogReader.hpp"

int main(int argc, char *argv[]) {
    CmdLine cmdLine;
    if (!cmdLine.parse(argc, argv))
        return EXIT_FAILURE;

   
    DEF_File* def;
    def = new DEF_File;
    ReadDEF(def, cmdLine.def);
    OutDEF(def,"C:\\out.def");
   // return EXIT_SUCCESS;
}