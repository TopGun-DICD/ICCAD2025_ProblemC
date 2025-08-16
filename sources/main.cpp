
#include "cmdline.hpp"
#include "verilog/Verilog.hpp"
#include "verilog/VerilogReader.hpp"
#include "lef/LEF.hpp"
#include "lef/LEF_READER.hpp"
#include "def/DEF.hpp"
#include "def/DEFReader.hpp"
#include "def/DEFWriter.hpp"
#include "liberty/Liberty.hpp"
#include "liberty/LibertyReader.hpp"
#include "algorithm/algorithm.hpp"

std::string printTimeStatistics(time_t start, time_t stop);

int main(int argc, char* argv[]) {
    CmdLine cmdLine;
    if (!cmdLine.parse(argc, argv))
        return EXIT_FAILURE;

    std::time_t timeStart = 0,
                timeStop = 0;
    //*
    std::cout << "Reading input LEF files, " << cmdLine.lefs.size() << " files to read.\n";
    lef::LEFData    lef;
    lef::LEF_READER lef_reader;
    time_t          timeSum = 0;
    for (const auto& lef_file : cmdLine.lefs) {
        std::cout << "  * Reading file '" << lef_file << "'...\n";
        timeStart = std::clock();
        if (!lef_reader.read(lef_file, lef))
            return EXIT_FAILURE;
        timeStop = std::clock() - timeStart;
        timeSum += timeStop;
        std::cout << "    Done in " << printTimeStatistics(timeStart, timeStop) << "\n";
    }
    std::cout << "Done reading " << cmdLine.lefs.size() << " LEF files. It took " << printTimeStatistics(0, timeSum) << " in total\n\n";
    //*/

    //*
    std::cout << "Reading input verilog file '" << cmdLine.verilog << "'...\n";
    timeStart = std::clock();
    verilog::Netlist        netlist;
    verilog::VerilogReader  verilogReader;
    if (!verilogReader.read(cmdLine.verilog, netlist, lef))
        return EXIT_FAILURE;
    timeStop = std::clock() - timeStart;
    std::cout << "Done reading input verilog file. File has been read in " << printTimeStatistics(timeStart, timeStop) << "\n\n";
    //*/

    //*
    std::cout << "Reading input DEF file '" << cmdLine.def << "'...\n";
    timeStart = std::clock();
    def::DEF_File   def;
    def::DEFReader  defReader;
    if (!defReader.ReadDEF(cmdLine.def, def, netlist))
        return EXIT_FAILURE;
    timeStop = std::clock() - timeStart;
    std::cout << "Done reading input DEF file. File has been read in " << printTimeStatistics(timeStart, timeStop) << "\n\n";
    //*/

    //*
    std::cout << "Reading input Liberty files, " << cmdLine.libs.size() << " files to read.\n";
    liberty::Liberty        liberty;
    liberty::LibertyReader  libReader;
    for (const auto &lib_file : cmdLine.libs) {
        std::cout << "  * Reading file '" << lib_file << "'...\n";
        timeStart = std::clock();
        if (!libReader.read(lib_file, liberty, netlist))
            return EXIT_FAILURE;
        timeStop = std::clock() - timeStart;
        timeSum += timeStop;
        std::cout << "    Done in " << printTimeStatistics(timeStart, timeStop) << "\n";
    }
    libReader.postProcessAfterAll();
    std::cout << "Done reading " << cmdLine.lefs.size() << " Liberty files. It took " << printTimeStatistics(0, timeSum) << " in total\n\n";
    //*/

    std::cout << "Prepare internal data for the algorithms...\n\n";
    verilogReader.postProcessAfterDEF();

    // Process the data...
    Algorithm algorithm(netlist, lef, def,liberty);

    std::cout << "Algoritm : performing step 1...\n";
    timeStart = std::clock();
    algorithm.step_1_SwapCells();
    timeStop = std::clock() - timeStart;
    std::cout << "Step 1 completed in " << printTimeStatistics(timeStart, timeStop) << "\n\n";

    std::cout << "Algoritm : performing step 2...\n";
    timeStart = std::clock();
    algorithm.step_2_MoveCells();
    timeStop = std::clock() - timeStart;
    std::cout << "Step 2 completed in " << printTimeStatistics(timeStart, timeStop) << "\n\n";

    std::cout << "Algoritm : performing step 3...\n";
    timeStart = std::clock();
    algorithm.step_3_OptimizeFanout();
    timeStop = std::clock() - timeStart;
    std::cout << "Step 3 completed in " << printTimeStatistics(timeStart, timeStop) << "\n\n";

    //*
    std::cout << "Writing the result to '" << cmdLine.outFile << "'...\n";
    timeStart = std::clock();
    def::DEFWriter defWriter;
    defWriter.OutDEF(cmdLine.outFile, def);
    timeStop = std::clock() - timeStart;
    std::cout << "Done. It took " << printTimeStatistics(timeStart, timeStop) << "\n\n";
    //*/
    return EXIT_SUCCESS;

}

std::string printTimeStatistics(time_t start, time_t stop) {
    std::time_t timeValMin = 0;
    std::time_t timeValSec = 0;
    std::time_t timeValMsec = stop;

    if (stop < 1000) {
        if (start == 0)
            timeValMsec = 1;
    }
    else {
        if (stop > 1000) {
            timeValSec = stop / 1000;
            timeValMsec = stop - timeValSec * 1000;
        }
        if (timeValSec > 60) {
            timeValMin = timeValSec / 60;
            timeValSec = timeValSec - (timeValMin * 60);
        }
    }

    if (!timeValSec)
        return std::to_string(timeValMsec) + " msec(s)";
    if (!timeValMin)
        return std::to_string(timeValSec) + " sec(s) " + std::to_string(timeValMsec) + " msec(s)";
    return std::to_string(timeValMin) + " min(s) " + std::to_string(timeValSec) + " sec(s) " + std::to_string(timeValMsec) + " msec(s)";
}
