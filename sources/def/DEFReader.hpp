#pragma once

#include <string>
#include <fstream>

#include "DEF.hpp"
#include "../verilog/Verilog.hpp"

namespace def {

class DEFReader {
public:
    bool ReadDEF(const std::string &nameInFile, DEF_File &def, verilog::Netlist &netlist);
private:
    void parsePROPERTYDEFINITIONS(std::ifstream* inFile, DEF_File& def, int* i);
    void parseComponents(std::ifstream* inFile, DEF_File& def, int* i, verilog::Netlist &netlist);
    void parseNets(std::ifstream* inFile, DEF_File& def, int* i);
    void parseSpecialnets(std::ifstream* inFile, DEF_File& def, int* i);
    void parsePins(std::ifstream* inFile, DEF_File& def, int* i);
    void parseVias(std::ifstream* inFile, DEF_File& def, int* i);
};

}