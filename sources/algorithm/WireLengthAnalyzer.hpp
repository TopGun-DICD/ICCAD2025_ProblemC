#pragma once

#include "../verilog/Verilog.hpp"
#include "../def/DEF.hpp"
#include <unordered_map>
#include <cmath>

class WireLengthAnalyzer {
public:
    WireLengthAnalyzer(const verilog::Netlist& netlist, const def::DEF_File& def);

    double calculateTotalWireLength() const;
    const std::unordered_map<std::string, double>& getInstanceWireLengths() const;
    void compareWireLengths(const std::unordered_map<std::string, double>& originalLengths) const;

private:
    const verilog::Netlist& netlist;
    const def::DEF_File& def;
    std::unordered_map<std::string, double> instanceWireLengths;

    double calculateInstanceWireLength(const verilog::Instance* instance) const;
};