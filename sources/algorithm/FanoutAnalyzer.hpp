#pragma once

#include "../verilog/Verilog.hpp"
#include <unordered_map>

class FanoutAnalyzer {
public:
    void analyzeFanout(verilog::Netlist& netlist);
    int getFanout(const std::string& instanceName) const;
    const std::unordered_map<std::string, int>& getFanoutCounts() const;

private:
    std::unordered_map<std::string, int> fanoutCounts;
};