#pragma once

#include "../verilog/Verilog.hpp"
#include "../liberty/Liberty.hpp"
#include <unordered_map>

class FanoutAnalyzer {
public:
    //void analyzeFanout(verilog::Netlist& netlist);
    //int getFanout(const std::string& instanceName) const;
    //const std::unordered_map<std::string, int>& getFanoutCounts() const;

    void analyzeCapacitance(verilog::Netlist& netlist);
    double getTotalCapacitance(const std::string& instanceName) const;
    const std::unordered_map<std::string, double>& getCapacitanceMap() const;

private:
    //std::unordered_map<std::string, int> fanoutCounts;

    std::unordered_map<std::string, double> capacitanceMap;
};