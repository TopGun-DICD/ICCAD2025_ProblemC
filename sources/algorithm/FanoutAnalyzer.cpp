#include "FanoutAnalyzer.hpp"

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
}