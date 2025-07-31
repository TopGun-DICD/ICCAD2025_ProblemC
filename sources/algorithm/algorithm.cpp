#include "algorithm.hpp"
#include "../step_1_SwapCells.hpp"

Algorithm::Algorithm(verilog::Netlist &_netlist, lef::LEFData &_lef, def::DEF_File &_def) : netlist(_netlist), lef(_lef), def(_def) {
}

void Algorithm::step_1_SwapCells() {
    step_1_SwapCells_u(netlist.top, &def);
}

void Algorithm::step_2_MoveCells() {}

void Algorithm::step_3_OptimizeFanout() {}


void Algorithm::swap_cells(def::DEF_File &def, const std::string &name1, const std::string &name2) {
    def::COMPONENTS_class *component1 = def.get_component(name1);
    def::COMPONENTS_class *component2 = def.get_component(name2);
    swap_cells(def, component1, component2);
}

void Algorithm::swap_cells(def::DEF_File& def, def::COMPONENTS_class *cell1, def::COMPONENTS_class *cell2) {
    def::COMPONENTS_class tempCell = *cell1;
    *cell1 = *cell2;
    *cell2 = tempCell;
}

