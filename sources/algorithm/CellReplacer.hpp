#pragma once

#include "../verilog/Verilog.hpp"
#include "../def/DEF.hpp"
#include "../def/DEFWriter.hpp"
#include "../lef/LEF.hpp"
#include "../liberty/Liberty.hpp"
#include <iostream>
#include <unordered_map>
#include <regex>
#include <algorithm>
#include <utility>

class CellReplacer {
public:
    CellReplacer(lef::LEFData& lefData);

    void replaceCellsBasedOnCapacitance(verilog::Netlist& netlist, const liberty::Liberty& liberty, const std::unordered_map<std::string, double>& capacitanceMap, def::DEF_File& defFile);
    std::tuple<std::string, std::string, std::string> parseCellName(const std::string& cellName);
    std::vector<std::string> findCellVariantsInLEF(const std::string& baseCell, const std::string& suffix);
    std::string selectOptimalVariant(const std::vector<std::string>& variants, int fanout, const std::string& currentCell) const;
    void updateDEFComponent(def::DEF_File& defFile, def::COMPONENTS_class* component, const std::string& OldCell, const std::string& NewCell, double capacitance);
    bool compactLayout(def::DEF_File& defFile, const std::unordered_map<std::string, std::pair<std::string, std::string>>& replacementMap);
    int getDriveStrength(const std::string& cellName) const;
    double getRowHeight() const;
    double getMaxRowWidth() const;

    const std::vector<std::tuple<std::string, std::string, std::string, double, double, std::string>>& getReplacements() const;

private:
    std::vector<std::tuple<std::string, std::string, std::string, double, double, std::string>> replacements;

    lef::LEFData& lefData;
    std::unordered_map<std::string, std::vector<std::string>> cellVariants;

    liberty::Cell* findLibertyCell(const liberty::Liberty& liberty, const std::string& cellName);
    double getMaxOutputCapacitance(const liberty::Cell* cell);

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