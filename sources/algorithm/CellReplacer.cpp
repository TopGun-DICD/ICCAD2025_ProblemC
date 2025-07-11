#include "CellReplacer.hpp"
#include <regex>
#include <algorithm>
#include <utility>

CellReplacer::CellReplacer(lef::LEFData& lefData) : lefData(lefData) {}

void CellReplacer::replaceCellsBasedOnFanout(verilog::Netlist& netlist,const std::unordered_map<std::string, int>& fanoutCounts,def::DEF_File& defFile) {

    for (const auto& [instanceName, fanout] : fanoutCounts) {

        auto* defComponent = defFile.get_component(instanceName);
        if (!defComponent) continue;

        std::string originalCell = defComponent->modelName;

        auto [baseCell,driveStr, suffix] = parseCellName(originalCell);

        std::vector<std::string> variants = findCellVariantsInLEF(baseCell, suffix);
        if (variants.empty()) continue;

        std::string newCell = selectOptimalVariant(variants, fanout, originalCell);

        if (newCell != originalCell) {
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

void CellReplacer::updateDEFComponent(def::DEF_File& defFile, def::COMPONENTS_class* component, const std::string& OldCell, const std::string& NewCell, int fanout) {

    std::cout << "Replacing " << component->compName << " from " << OldCell << " to " << NewCell << " (fanout = " << fanout << ")" << std::endl;

    component->modelName = NewCell;

    adjustPlacementIfNeeded(defFile, component, lefData.getMacroSize(OldCell), lefData.getMacroSize(NewCell));
}

void CellReplacer::adjustPlacementIfNeeded(def::DEF_File& defFile, def::COMPONENTS_class* modcomponent, const std::pair<double, double>& OldSize, const std::pair<double, double>& NewSize) {

    double deltaX = NewSize.first - OldSize.first;
    double deltaY = NewSize.second - OldSize.second;

    double x = modcomponent->POS.x;
    double y = modcomponent->POS.y;

    if (deltaX != 0) {
        for (auto* comp : defFile.get_all_component()) {
            if (comp == modcomponent) continue;

            if (comp->POS.y == y && comp->POS.x > x) {
                comp->POS.x += deltaX;
            }
        }
    }

    if (deltaY != 0) {
        for (auto* comp : defFile.get_all_component()) {
            if (!comp || comp == modcomponent) continue;

            if (comp->POS.x == x && comp->POS.y > y) {
                comp->POS.y += deltaY;
            }
        }
    }
    std::cout << "Adjusted placement for " << modcomponent->compName << ": dx=" << deltaX << ", dy=" << deltaY << std::endl;
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
        comp->POS.x = currentX;
        comp->POS.y = currentY;
        currentX += size.first;
    }
}



