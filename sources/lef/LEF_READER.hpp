#ifndef LEF_READER_HPP
#define LEF_READER_HPP

#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <memory>

#include "LEF.hpp"

class LEF_READER {
    std::vector<Macro*> macros;
    std::vector<Site*> sites;
public:
    ~LEF_READER();

    bool read(const std::string& filename);
    Macro* getMacroByName(const std::string& macroName) const;
    void exportToFile(const std::string& filename) const;
private:
    static std::string trim(const std::string& str);
    static std::vector<std::string> tokenize(const std::string& line);
    static double safeStod(const std::string& str);

    Rect* parseRect(const std::vector<std::string>& tokens) const;
    Port_lef* parsePort(std::ifstream& file) const;
    Pin* parsePin(std::ifstream& file, const std::string& pinName) const;
    Obs* parseObs(std::ifstream& file) const;
    Macro* parseMacro(std::ifstream& file, const std::string& name);
    Site* parseSite(std::ifstream& file, const std::string& name, const std::vector<Site*>& existingSite);

    void clear();
};
#endif