#pragma once

#include "../def/DEF.hpp"
#include "../def/DEFWriter.hpp"
#include "../lef/LEF.hpp"
#include "../verilog/Verilog.hpp"
#include "CellReplacer.hpp"
#include "FanoutAnalyzer.hpp"
#include "WireLengthAnalyzer.hpp"
#include "../step_1_SwapCells.hpp"

class Algorithm {
    verilog::Netlist   &netlist;
    lef::LEFData       &lef;
    def::DEF_File      &def;

    liberty::Liberty& liberty;
    FanoutAnalyzer fanoutAnalyzer;
    CellReplacer cellReplacer;

public:
    Algorithm(verilog::Netlist &_netlist, lef::LEFData &_lef, def::DEF_File &_def, liberty::Liberty& _liberty);
public:
    void step_1_SwapCells();
    void step_2_MoveCells();
    void step_3_OptimizeFanout();
private:
    void swap_cells(def::DEF_File &def, const std::string &name1, const std::string &name2);
    void swap_cells(def::DEF_File& def, def::COMPONENTS_class *cell1, def::COMPONENTS_class *cell2);
};