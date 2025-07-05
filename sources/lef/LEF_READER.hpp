#ifndef LEF_READER_HPP
#define LEF_READER_HPP

#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <memory>

#include "LEF.hpp"

namespace lef {

    class LEF_READER {
    public:
        ~LEF_READER();

        bool read(const std::string& filename, LEFData &lefs);
        //Macro* getMacroByName(const std::string& macroName) const;
        //void exportToFile(const std::string& filename) const;
    private:
        static std::string trim(const std::string& str);
        static std::vector<std::string> tokenize(const std::string& line);
        static double safeStod(const std::string& str);

        void parseRect(const std::vector<std::string>& tokens, std::vector<Rect> &rects) const;
        Port* parsePort(std::ifstream& file) const;
        void parsePin(std::ifstream& file, const std::string& pinName, Macro *macro) const;
        void parseObs(std::ifstream& file, Macro *macro) const;
        void parseMacro(std::ifstream& file, const std::string& name, LEFData& lefs);
        void parseSite(std::ifstream& file, const std::string& name, LEFData& lefs);

        //void clear();
    };

}

#endif