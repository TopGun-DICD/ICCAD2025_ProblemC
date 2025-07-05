#pragma once

#include <string>
#include <fstream>

#include "DEF.hpp"

namespace def {

class DEFReader {
public:
    bool ReadDEF(const std::string &nameInFile, DEF_File &def);
private:
    void parsePROPERTYDEFINITIONS(std::ifstream* inFile, DEF_File& def, int* i);
    void parseComponents(std::ifstream* inFile, DEF_File& def, int* i);
    void parseNets(std::ifstream* inFile, DEF_File& def, int* i);
    void parseSpecialnets(std::ifstream* inFile, DEF_File& def, int* i);
    void parsePins(std::ifstream* inFile, DEF_File& def, int* i);
    void parseVias(std::ifstream* inFile, DEF_File& def, int* i);
    //Position return_component_pos(DEF_File& def, std::string name);
    //void swap_comp(DEF_File& def, std::string name1, std::string name2);
};

}