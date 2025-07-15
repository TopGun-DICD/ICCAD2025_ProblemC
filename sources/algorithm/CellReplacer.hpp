#pragma once

#include "../verilog/Verilog.hpp"
#include "../def/DEF.hpp"
#include "../lef/LEF.hpp"
#include <iostream>
#include <unordered_map>
#include <regex>
#include <algorithm>
#include <utility>

class CellReplacer {
public:
    CellReplacer(lef::LEFData& lefData);

    void replaceCellsBasedOnFanout(verilog::Netlist& netlist,const std::unordered_map<std::string, int>& fanoutCounts,def::DEF_File& defFile);
    std::tuple<std::string,int, std::string> parseCellName(const std::string& cellName);
    std::vector<std::string> findCellVariantsInLEF(const std::string& baseCell, const std::string& suffix);
    std::string selectOptimalVariant(const std::vector<std::string>& variants, int fanout, const std::string& currentCell) const ;
    void updateDEFComponent(def::DEF_File& defFile, def::COMPONENTS_class* component, const std::string& OldCell, const std::string& NewCell, int fanout);
    void compactLayout(def::DEF_File& defFile);
    int getDriveStrength(const std::string& cellName) const;
    double getRowHeight() const;
    double getMaxRowWidth() const;


private:
    lef::LEFData& lefData;
    std::unordered_map<std::string, std::vector<std::string>> cellVariants;

    struct RowInfo {
        double y;
        double startX;
        double endX;

        std::vector<def::COMPONENTS_class*> components;
    };
    std::vector<RowInfo> analyzeRows(def::DEF_File& defFile);
    bool canPlaceInRow(RowInfo& row, def::COMPONENTS_class* comp, const std::pair<double, double>& newSize);
    bool tryMoveToAdjacentRow(def::DEF_File& defFile, def::COMPONENTS_class* comp, const std::pair<double, double>& newSize, const std::vector<RowInfo>& rows);
    void adjustPlacementWithOrientation(def::DEF_File& defFile, def::COMPONENTS_class* component, const std::pair<double, double>& oldSize, const std::pair<double, double>& newSize);
    const double maxRowWidth = 100.0;
};