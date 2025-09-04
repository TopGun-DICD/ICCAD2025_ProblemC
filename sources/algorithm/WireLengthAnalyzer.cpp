#include "WireLengthAnalyzer.hpp"
#include <iostream>

WireLengthAnalyzer::WireLengthAnalyzer(const verilog::Netlist& netlist, const def::DEF_File& def) : netlist(netlist), def(def) {
    for (const auto* instance : netlist.top->instances) {
        instanceWireLengths[instance->name] = calculateInstanceWireLength(instance);
    }
}

double WireLengthAnalyzer::calculateTotalWireLength() const {
    double totalLength = 0.0;
    for (const auto& [name, length] : instanceWireLengths) {
        totalLength += length;
    }
    return totalLength;
}

const std::unordered_map<std::string, double>& WireLengthAnalyzer::getInstanceWireLengths() const {
    return instanceWireLengths;
}

void WireLengthAnalyzer::compareWireLengths(const std::unordered_map<std::string, double>& originalLengths) const {
    double totalOriginal = 0.0;
    double totalCurrent = 0.0;

    
    //std::cout << "Wire length changes after cell replacement:\n";
    //std::cout << "------------------------------------------\n";
    

    for (const auto& [name, currentLength] : instanceWireLengths) {
        auto it = originalLengths.find(name);
        if (it != originalLengths.end()) {
            double originalLength = it->second;
            double delta = currentLength - originalLength;

            //std::cout << "Instance: " << name << " | Original length: " << originalLength<< " | New length: " << currentLength<< " | Delta: " << delta << "\n";

            totalOriginal += originalLength;
            totalCurrent += currentLength;
        }
    }

    
    //std::cout << "------------------------------------------\n";
    //std::cout << "Total original wire length: " << totalOriginal << "\n";
    //std::cout << "Total new wire length: " << totalCurrent << "\n";
    //std::cout << "Total change: " << (totalCurrent - totalOriginal) << "\n";
    
}

double WireLengthAnalyzer::calculateInstanceWireLength(const verilog::Instance* instance) const {
    double length = 0.0;

    for (size_t i = 0; i < instance->ins.size(); ++i) {
        if (instance->ins[i]->driver) {
            const auto* driverComp = instance->ins[i]->driver->placement.component;
            const auto* thisComp = instance->placement.component;

            if (driverComp && thisComp) {
                double dx = driverComp->POS.x - thisComp->POS.x;
                double dy = driverComp->POS.y - thisComp->POS.y;
                length += std::sqrt(dx * dx + dy * dy);
            }
        }
    }

    for (size_t i = 0; i < instance->outs.size(); ++i) {
        for (const auto* sink : instance->outs[i]->sourceFor) {
            if (sink) {
                const auto* sinkComp = sink->placement.component;
                const auto* thisComp = instance->placement.component;

                if (sinkComp && thisComp) {
                    double dx = sinkComp->POS.x - thisComp->POS.x;
                    double dy = sinkComp->POS.y - thisComp->POS.y;
                    length += std::sqrt(dx * dx + dy * dy);
                }
            }
        }
    }

    return length;
}