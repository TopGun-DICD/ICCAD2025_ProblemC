#include "FanoutAnalyzer.hpp"

void FanoutAnalyzer::analyzeCapacitance(verilog::Netlist& netlist) {
    capacitanceMap.clear();

    if (!netlist.top) return;

    for (auto* instance : netlist.top->instances) {
        if (!instance->libertyCell) continue;

        for(size_t i = 0;i<instance->outs.size();i++){
            auto* outNet = instance->outs[i];
            if (!outNet) continue;

            double maxCapacitance = 0.0;

            for (auto* sinkInstance : outNet->sourceFor) {
                if (sinkInstance && sinkInstance->libertyCell) {
                    for (auto* inPin : sinkInstance->libertyIns) {
                                maxCapacitance += inPin->capacitance;
                    }
                }
            }
            capacitanceMap[instance->name] = maxCapacitance;
        }
    }
}

double FanoutAnalyzer::getTotalCapacitance(const std::string& instanceName) const {
    auto it = capacitanceMap.find(instanceName);
    return it != capacitanceMap.end() ? it->second : 0.0;
}

const std::unordered_map<std::string, double>& FanoutAnalyzer::getCapacitanceMap() const {
    return capacitanceMap;
}
/*
void FanoutAnalyzer::analyzeFanout(verilog::Netlist& netlist) {
    fanoutCounts.clear();

    if (!netlist.top) return;

    for (auto* instance : netlist.top->instances) {
        for (size_t i = 0; i < instance->ins.size(); ++i) {
            if (instance->instanceOf->ports[i]->direction == verilog::PortDirection::output) {
                fanoutCounts[instance->name] += instance->ins[i]->sourceFor.size();
            }
        }
    }
}

int FanoutAnalyzer::getFanout(const std::string& instanceName) const {
    auto it = fanoutCounts.find(instanceName);
    return it != fanoutCounts.end() ? it->second : 0;
}

const std::unordered_map<std::string, int>& FanoutAnalyzer::getFanoutCounts() const {
    return fanoutCounts;
}*/