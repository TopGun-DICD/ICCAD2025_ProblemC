#include "algorithm.hpp"
#include "step_1_SwapCells.hpp"

Algorithm::Algorithm(verilog::Netlist& _netlist, lef::LEFData& _lef, def::DEF_File& _def, liberty::Liberty& _liberty) : netlist(_netlist), lef(_lef), def(_def), liberty(_liberty), cellReplacer(_lef) {
}

void Algorithm::step_1_SwapCells() {
    step_1_SwapCells_u(netlist.top, &def);
}

void Algorithm::step_2_MoveCells() {}

void Algorithm::step_3_OptimizeFanout() {

    WireLengthAnalyzer wireAnalyzerBefore(netlist, def);
    auto originalLengths = wireAnalyzerBefore.getInstanceWireLengths();

    FanoutAnalyzer analyzer;
    analyzer.analyzeCapacitance(netlist);
    cellReplacer.replaceCellsBasedOnCapacitance(netlist, liberty, analyzer.getCapacitanceMap(), def);

    WireLengthAnalyzer wireAnalyzerAfter(netlist, def);
    wireAnalyzerAfter.compareWireLengths(originalLengths);
}

void Algorithm::swap_cells(def::DEF_File& def, const std::string& name1, const std::string& name2) {
    def::COMPONENTS_class* component1 = def.get_component(name1);
    def::COMPONENTS_class* component2 = def.get_component(name2);
    swap_cells(def, component1, component2);
}

void Algorithm::swap_cells(def::DEF_File& def, def::COMPONENTS_class* cell1, def::COMPONENTS_class* cell2) {
    def::COMPONENTS_class tempCell = *cell1;
    *cell1 = *cell2;
    *cell2 = tempCell;
}

uint64_t Algorithm::calcTotalWirelength(bool withIOPads) {
    uint64_t totalLength = 0;
    for (verilog::Instance * instance : netlist.top->instances) {
        // ≈сли это внешний пин и мы хотим считать длину включа€ рассто€ние до пинов и это выходной пин
        if (!instance->instanceOf && (withIOPads && !instance->ins.empty())) {
            for (verilog::Net *net : instance->ins) {
                if (net->driver->placement.pin) {
                    if (withIOPads)
                        totalLength +=  abs(instance->placement.pin->POS.x - net->driver->placement.pin->POS.x) +
                                        abs(instance->placement.pin->POS.y - net->driver->placement.pin->POS.y);
                }
                else {
                    totalLength +=  abs(instance->placement.pin->POS.x - net->driver->placement.component->POS.x) +
                                    abs(instance->placement.pin->POS.y - net->driver->placement.component->POS.y);
                }
            }
        }
        else {
            for (verilog::Net *net : instance->ins) {
                if (net->driver->placement.pin) {
                    if (withIOPads)
                        totalLength +=  abs(instance->placement.component->POS.x - net->driver->placement.pin->POS.x) +
                                        abs(instance->placement.component->POS.y - net->driver->placement.pin->POS.y);
                }
                else {
                    totalLength +=  abs(instance->placement.component->POS.x - net->driver->placement.component->POS.x) +
                                    abs(instance->placement.component->POS.y - net->driver->placement.component->POS.y);
                }
            }
        }
    }
    return totalLength;
}
