#pragma once

#include "../def/DEF.hpp"
#include "../lef/LEF.hpp"
#include "../verilog/Verilog.hpp"

class Algorithm {
    verilog::Netlist   &netlist;
    lef::LEFData       &lef;
    def::DEF_File      &def;
public:
    Algorithm(verilog::Netlist &_netlist, lef::LEFData &_lef, def::DEF_File &_def);
public:
    void step_1_SwapCells();
    void step_2_MoveCells();
    void step_3_OptimizeFanout();
private:
    void swap_cells(def::DEF_File &def, const std::string &name1, const std::string &name2);
    void swap_cells(def::DEF_File& def, def::COMPONENTS_class *cell1, def::COMPONENTS_class *cell2);
};