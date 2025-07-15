#include "CellReplacer.hpp"

CellReplacer::CellReplacer(lef::LEFData& lefData) : lefData(lefData) {}

void CellReplacer::replaceCellsBasedOnFanout(verilog::Netlist& netlist,const std::unordered_map<std::string, int>& fanoutCounts,def::DEF_File& defFile) {

    const auto rows = analyzeRows(defFile);

    for (const auto& [instanceName, fanout] : fanoutCounts) {

        auto* defComponent = defFile.get_component(instanceName);
        if (!defComponent) continue;

        std::string originalCell = defComponent->modelName;

        auto [baseCell,driveStr, suffix] = parseCellName(originalCell);

        std::vector<std::string> variants = findCellVariantsInLEF(baseCell, suffix);
        if (variants.empty()) continue;

        std::string newCell = selectOptimalVariant(variants, fanout, originalCell);

        if (newCell == originalCell) continue;

        const auto OldSize = lefData.getScaledMacroSize(originalCell);
        const auto NewSize = lefData.getScaledMacroSize(newCell);

        bool PlacementSuccess = false;

        auto currentRowIt = std::find_if(rows.begin(), rows.end(),
            [defComponent](const RowInfo& r) { return r.y == defComponent->POS.y; });

        if (currentRowIt != rows.end()) {
      
            if (canPlaceInRow(const_cast<RowInfo&>(*currentRowIt), defComponent, NewSize)) {
                PlacementSuccess = true;
            }

            else if (tryMoveToAdjacentRow(defFile, defComponent, NewSize, rows)) {
                PlacementSuccess = true;
            }
        }

        if (PlacementSuccess) {
            defComponent->modelName = newCell;
            updateDEFComponent(defFile, defComponent, originalCell, newCell, fanout);
        }
    }
    compactLayout(defFile);
}

std::tuple<std::string,int, std::string> CellReplacer::parseCellName(const std::string& cellName) {

    std::regex basePattern(R"((\w+?)x(\d+)?(_\w+)?$)");
    std::smatch matches;
    if (std::regex_search(cellName, matches, basePattern)) {
        int drive = 0;
        if (matches[2].matched) {
            drive = std::stoi(matches[2].str());
        }
        return { matches[1].str(), drive, matches[3].str() };
    }
    return { cellName, 0, "" };
}

std::vector<std::string> CellReplacer::findCellVariantsInLEF(const std::string& baseCell, const std::string& suffix) {
    std::vector<std::string> variants;

    std::regex variantPattern("^" + baseCell + "x\\d+" + suffix + "$");

    for (const auto* macro : lefData.getMacroes()) {

        if (!macro || macro->name.empty()) continue;

        const std::string& cellName = macro->name;
        if (std::regex_match(cellName, variantPattern)) {
            variants.push_back(cellName);
        }
    }

    return variants;
}

std::string CellReplacer::selectOptimalVariant(const std::vector<std::string>& variants, int fanout, const std::string& currentCell) const {

    auto currentSize = lefData.getMacroSize(currentCell);
    double currentArea = currentSize.first * currentSize.second;

    std::vector<std::pair<std::string, double>> candidates;

    for (const auto& variant : variants) {
        if (getDriveStrength(variant) < fanout) continue;

        auto size = lefData.getMacroSize(variant);
        double area = size.first * size.second;

        candidates.emplace_back(variant, area);
    }

    if (candidates.empty()) return currentCell;

    auto best = std::min_element(candidates.begin(), candidates.end(), [](const auto& a, const auto& b) {return a.second < b.second;});
    return best->first;
}

int CellReplacer::getDriveStrength(const std::string& cellName) const {
    std::regex drivePattern(R"(x(\d+))");
    std::smatch matches;
    if (std::regex_search(cellName, matches, drivePattern)) {
        return std::stoi(matches[1].str());
    }
    return 0;
}

double CellReplacer::getRowHeight() const {
    if (!lefData.getMacroes().empty()) {
        return lefData.getMacroes()[0]->sizeY;
    }
    return 0.0;
}

double CellReplacer::getMaxRowWidth() const {
    return maxRowWidth;
}

std::vector<CellReplacer::RowInfo> CellReplacer::analyzeRows(def::DEF_File& defFile) {
    std::vector<RowInfo> rows;
    std::unordered_map<double, RowInfo> rowMap;

    for (auto& comp : defFile.get_all_component()) {
        rowMap[comp->POS.y].components.push_back(comp);
        rowMap[comp->POS.y].y = comp->POS.y;
    }

    for (auto& [y,row] : rowMap) {
        row.startX = defFile.DIEAREA.x1;
        row.endX = defFile.DIEAREA.x2;
        rows.push_back(row);
    }

    std::sort(rows.begin(), rows.end(), [](const RowInfo& a, const RowInfo& b) {return a.y < b.y;});

    return rows;
}

bool CellReplacer::canPlaceInRow(RowInfo& row, def::COMPONENTS_class* comp, const std::pair<double, double>& newSize) {
    std::sort(row.components.begin(), row.components.end(), [](const def::COMPONENTS_class* a, const def::COMPONENTS_class* b) { return a->POS.x < b->POS.x;});

    double leftSpace = comp->POS.x - row.startX;
    double rightSpace = row.endX - (comp->POS.x + newSize.first);

    return(leftSpace >= newSize.first) || (rightSpace >= newSize.first);
}

bool CellReplacer::tryMoveToAdjacentRow(def::DEF_File& defFile, def::COMPONENTS_class* comp, const std::pair<double, double>& newSize, const std::vector<RowInfo>& rows) {
    auto currentRowIt = std::find_if(rows.begin(), rows.end(), [comp](const RowInfo& r) {return r.y == comp->POS.y;});

    if (currentRowIt == rows.end()) return false;

    if (currentRowIt != rows.begin()) {
        auto& prevRow = *(currentRowIt - 1);
        if (canPlaceInRow(const_cast<RowInfo&>(prevRow), comp, newSize)) {
            comp->POS.y = prevRow.y;
            return true;
        }
    }

    if (currentRowIt + 1 != rows.end()) {
        auto& nextRow = *(currentRowIt + 1);
        if (canPlaceInRow(const_cast<RowInfo&>(nextRow), comp, newSize)) {
            comp->POS.y = nextRow.y;
            return true;
        }
    }
    return false;
}

void CellReplacer::adjustPlacementWithOrientation(def::DEF_File& defFile, def::COMPONENTS_class* component, const std::pair<double, double>& oldSize, const std::pair<double, double>& newSize) {
    double deltaX = newSize.first - oldSize.first;
    double deltaY = newSize.second - oldSize.second;

    if (deltaX != 0 || deltaY != 0) {
        switch (component->POS.orientation) {
        case def::Orientation::N: component->POS.orientation = def::Orientation::S; break;
        case def::Orientation::S: component->POS.orientation = def::Orientation::N; break;
        case def::Orientation::E: component->POS.orientation = def::Orientation::W; break;
        case def::Orientation::W: component->POS.orientation = def::Orientation::E; break;
        case def::Orientation::FN: component->POS.orientation = def::Orientation::FS; break;
        case def::Orientation::FS: component->POS.orientation = def::Orientation::FN; break;
        case def::Orientation::FE: component->POS.orientation = def::Orientation::FW; break;
        case def::Orientation::FW: component->POS.orientation = def::Orientation::FE; break;
        }

        for (auto* other : defFile.get_all_component()) {
            if (other == component) continue;
            if (other->POS.y == component->POS.y && other->POS.x > component->POS.x) {
                other->POS.x += deltaX;  
            }
        }
    }

    if (component->POS.orientation == def::Orientation::FS) {
        component->POS.y -= newSize.second;
    }
}

void CellReplacer::updateDEFComponent(def::DEF_File& defFile,def::COMPONENTS_class* component,const std::string& OldCell,const std::string& NewCell,int fanout) {

    const auto OldSize = lefData.getScaledMacroSize(OldCell);
    const auto NewSize = lefData.getScaledMacroSize(NewCell);

    std::cout << "Replacing " << component->compName
        << " from " << OldCell << " to " << NewCell
        << " (fanout = " << fanout << ")" << std::endl;

    component->modelName = NewCell;

    if (OldSize != NewSize) {
        adjustPlacementWithOrientation(defFile, component, OldSize, NewSize);

        if (component->POS.orientation == def::Orientation::FS) {
            component->POS.y -= NewSize.second;
        }

        /*
        std::cout << "Adjusted placement for " << component->compName
            << ": new position (" << component->POS.x << ", " << component->POS.y << ")"
            << ", orientation: " << static_cast<int>(component->POS.orientation)
            << ", size: " << NewSize.first << "x" << NewSize.second << " nm" << std::endl;

        std::cout << "Before adjustment - Neighboring cells:\n";
        for (auto* other : defFile.get_all_component()) {
            if (other->POS.y == component->POS.y && other != component) {
                std::cout << "  " << other->compName << ": (" << other->POS.x << ", " << other->POS.y << ")\n";
            }
        }*/
    }

    if (component->FIXED == def::FIXED_class::UNPLACED) {
        component->FIXED = def::FIXED_class::PLACED;
        component->SOURCE = def::SOURCE_class::TIMING;
    }
}

void CellReplacer::compactLayout(def::DEF_File& defFile) {
    auto& components = const_cast<std::vector<def::COMPONENTS_class*>&>(defFile.get_all_component());
    std::sort(components.begin(), components.end(),
        [](const def::COMPONENTS_class* a, const def::COMPONENTS_class* b) {
            return std::tie(a->POS.y, a->POS.x) < std::tie(b->POS.y, b->POS.x);
        });

    double currentY = components.empty() ? 0 : components[0]->POS.y;
    double currentX = 0;
    double rowHeight = getRowHeight();
    double maxWidth = getMaxRowWidth();

    for (auto* comp : components) {
        auto size = lefData.getMacroSize(comp->modelName);
        if (currentX + size.first > maxWidth) {
            currentY += rowHeight;
            currentX = 0;
        }

        for (auto* other : components) {
            if (other == comp) continue;
            if (other->POS.y == currentY &&
                other->POS.x >= currentX &&
                other->POS.x < currentX + size.first) {
                currentX = other->POS.x + lefData.getMacroSize(other->modelName).first;
            }
        }
        comp->POS.x = currentX;
        comp->POS.y = currentY;
        currentX += size.first;
    }
}



