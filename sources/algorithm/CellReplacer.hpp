#pragma once

#include "../verilog/Verilog.hpp"
#include "../def/DEF.hpp"
#include "../lef/LEF.hpp"
#include <iostream>
#include <unordered_map>

class CellReplacer {
public:
    CellReplacer(lef::LEFData& lefData);

    void replaceCellsBasedOnFanout(verilog::Netlist& netlist,const std::unordered_map<std::string, int>& fanoutCounts,def::DEF_File& defFile);
    std::tuple<std::string,int, std::string> parseCellName(const std::string& cellName);
    std::vector<std::string> findCellVariantsInLEF(const std::string& baseCell, const std::string& suffix);
    std::string selectOptimalVariant(const std::vector<std::string>& variants, int fanout, const std::string& currentCell) const ;
    void updateDEFComponent(def::DEF_File& defFile, def::COMPONENTS_class* component, const std::string& OldCell, const std::string& NewCell, int fanout);
    void adjustPlacementIfNeeded(def::DEF_File& defFile, def::COMPONENTS_class* component, const std::pair<double, double>& OldSize, const std::pair<double, double>& NewSize);
    void compactLayout(def::DEF_File& defFile);
    int getDriveStrength(const std::string& cellName) const;
    double getRowHeight() const;
    double getMaxRowWidth() const;

private:
    lef::LEFData& lefData;
    std::unordered_map<std::string, std::vector<std::string>> cellVariants;

    const double maxRowWidth = 100.0;
};