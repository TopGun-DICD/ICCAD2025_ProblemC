#include "CellReplacer.hpp"

CellReplacer::CellReplacer(lef::LEFData& lefData) : lefData(lefData) {}

std::vector<std::tuple<std::string, std::string, std::string, double, double, std::string>> replacements;

void CellReplacer::replaceCellsBasedOnCapacitance(verilog::Netlist& netlist, const liberty::Liberty& liberty, const std::unordered_map<std::string, double>& capacitanceMap, def::DEF_File& defFile) {
    const auto rows = analyzeRows(defFile);
    replacements.clear();
    std::unordered_map<std::string, std::pair<std::string, std::string>> replacementMap;

    for (const auto& [instanceName, maxCapacitance] : capacitanceMap) {
        auto* defComponent = defFile.get_component(instanceName);
        if (!defComponent) continue;

        auto* instance = netlist.top->getInstanceByName(instanceName);
        if (!instance || !instance->libertyCell) continue;

        std::string originalCell = defComponent->modelName;
        auto [baseCell, driveStr, suffix] = parseCellName(originalCell);

        std::vector<std::string> variants = findCellVariantsInLEF(baseCell, suffix);

        if (variants.empty()) continue;

        double originalMaxCap = getMaxOutputCapacitance(instance->libertyCell);

        if (maxCapacitance > originalMaxCap) {
            std::string bestReplacement;
            double bestCap = originalMaxCap;
            std::pair<double, double> bestSize;

            for (const auto& variant : variants) {
                if (variant == originalCell) continue;

                liberty::Cell* variantCell = findLibertyCell(liberty, variant);
                if (!variantCell) continue;

                double variantCap = getMaxOutputCapacitance(variantCell);
                auto variantSize = lefData.getScaledMacroSize(variant);

                if (variantCap >= maxCapacitance) {
                    if (bestReplacement.empty() || variantCap < bestCap) {
                        bestReplacement = variant;
                        bestCap = variantCap;
                        bestSize = variantSize;
                    }
                }
            }

            if (bestReplacement.empty()) {
                auto currentRowIt = std::find_if(rows.begin(), rows.end(), [defComponent](const RowInfo& r) { return r.y == defComponent->POS.y; });
                if (currentRowIt != rows.end()) {
                    RowInfo rowCopy = *currentRowIt;
                    if (canPlaceInRow(rowCopy, defComponent, bestSize)) {
                        replacements.emplace_back(
                            defComponent->compName,
                            originalCell,
                            bestReplacement,
                            defComponent->POS.x,
                            defComponent->POS.y,
                            def::DEFWriter::Orientation_transform(defComponent->POS.orientation)
                        );
                        defComponent->modelName = bestReplacement;
                        updateDEFComponent(defFile, defComponent, originalCell, bestReplacement, maxCapacitance);
                    }
                }
            }
        }

        else if (maxCapacitance < originalMaxCap) {

            std::string bestReplacement;
            double bestCap = 0.0;
            std::pair<double, double> bestSize;

            for (const auto& variant : variants) {
                if (variant == originalCell) continue;

                liberty::Cell* variantCell = findLibertyCell(liberty, variant);
                if (!variantCell) continue;

                double variantCap = getMaxOutputCapacitance(variantCell);
                auto variantSize = lefData.getScaledMacroSize(variant);

                if (variantCap <= maxCapacitance && variantCap > bestCap) {
                    bestReplacement = variant;
                    bestCap = variantCap;
                    bestSize = variantSize;
                }
            }

            if (!bestReplacement.empty()) {
                auto currentRowIt = std::find_if(rows.begin(), rows.end(), [defComponent](const RowInfo& r) { return r.y == defComponent->POS.y; });
                if (currentRowIt != rows.end()) {
                    RowInfo rowCopy = *currentRowIt;
                    if (canPlaceInRow(rowCopy, defComponent, bestSize)) {
                        replacements.emplace_back(
                            defComponent->compName,
                            originalCell,
                            bestReplacement,
                            defComponent->POS.x,
                            defComponent->POS.y,
                            def::DEFWriter::Orientation_transform(defComponent->POS.orientation)
                        );

                        defComponent->modelName = bestReplacement;
                        updateDEFComponent(defFile, defComponent, originalCell, bestReplacement, maxCapacitance);
                    }
                }
            }
        }
    }

    if (!compactLayout(defFile, replacementMap)) {
        for (auto& [compName, cells] : replacementMap) {
            auto* comp = defFile.get_component(compName);
            if (comp) comp->modelName = cells.first;
        }
        replacements.clear();
    }
}

const std::vector<std::tuple<std::string, std::string, std::string, double, double, std::string>>&CellReplacer::getReplacements() const {
    return replacements;
}

liberty::Cell* CellReplacer::findLibertyCell(const liberty::Liberty& liberty, const std::string& cellName) {
    for (auto* lib : liberty.libraries) {
        for (auto* cell : lib->cells) {
            if (cell->name == cellName) {
                return cell;
            }
        }
    }
    return nullptr;
}

double CellReplacer::getMaxOutputCapacitance(const liberty::Cell* cell) {
    if (!cell) return 0.0;

    if (cell->outs.empty()) return 0.0;

    double maxCap = 0.0;
    for (auto* outPin : cell->outs) {
        if (outPin->capacitance > maxCap) {
            maxCap = outPin->capacitance;
        }
    }
    return maxCap;
}

std::tuple<std::string, std::string, std::string> CellReplacer::parseCellName(const std::string& cellName) {

    std::regex basePattern(R"((\w+?)x(\d+)_(ASAP7_\d+t_\w+))");
    std::regex xpPattern(R"((\w+?)xp(\d+)_(ASAP7_\d+t_\w+))");
    std::regex simplePattern(R"((\w+?)(x(\d+))?(_ASAP7_\d+t_\w+)?)");

    std::smatch matches;

    if (std::regex_match(cellName, matches, basePattern)) {
        return { matches[1].str(), matches[2].str(), matches[3].str() };
    }

    if (std::regex_match(cellName, matches, xpPattern)) {
        return { matches[1].str(), "p" + matches[2].str(), matches[3].str() };
    }

    if (std::regex_match(cellName, matches, simplePattern)) {
        std::string drive = matches[3].matched ? matches[3].str() : "1";
        std::string suffix = matches[4].matched ? matches[4].str().substr(1) : "";
        return { matches[1].str(), drive, suffix };
    }

    return { cellName, "1", "" };
}

std::vector<std::string> CellReplacer::findCellVariantsInLEF(const std::string& baseCell, const std::string& suffix) {
    std::vector<std::string> variants;
    std::string suffixPattern = suffix.empty() ? "" : "_" + suffix;

    for (const auto* macro : lefData.getMacroes()) {
        if (!macro || macro->name.empty()) continue;

        const std::string& cellName = macro->name;

        if (cellName.find(baseCell) == 0 && (suffix.empty() || cellName.find(suffixPattern) != std::string::npos)) {
            size_t baseLen = baseCell.length();
            if (cellName.length() > baseLen && (cellName[baseLen] == 'x' || (cellName[baseLen] == 'x' && cellName[baseLen + 1] == 'p'))) {
                variants.push_back(cellName);
            }
        }
    }

    std::sort(variants.begin(), variants.end(), [this](const auto& a, const auto& b) {return this->getDriveStrength(a) < this->getDriveStrength(b);});

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
    std::regex xpDrivePattern(R"(xp(\d+))");
    std::regex xpDigitDrivePattern(R"(x(p\d+))");
    std::smatch matches;

    if (std::regex_search(cellName, matches, xpDrivePattern)) {
        return std::stoi(matches[1].str());
    }
    if (std::regex_search(cellName, matches, xpDigitDrivePattern)) {
        return std::stoi(matches[1].str().substr(1));
    }
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

    for (auto& [y, row] : rowMap) {
        row.startX = defFile.DIEAREA.x1;
        row.endX = defFile.DIEAREA.x2;
        rows.push_back(row);
    }

    std::sort(rows.begin(), rows.end(), [](const RowInfo& a, const RowInfo& b) {return a.y < b.y;});

    return rows;
}

bool CellReplacer::canPlaceInRow(RowInfo& row, def::COMPONENTS_class* comp, const std::pair<double, double>& newSize) {
    std::sort(row.components.begin(), row.components.end(),
        [](const def::COMPONENTS_class* a, const def::COMPONENTS_class* b) {
            return a->POS.x < b->POS.x;
        });

    auto it = std::find(row.components.begin(), row.components.end(), comp);
    if (it == row.components.end()) return false;

    double availableSpaceLeft = (it == row.components.begin()) ?
        (comp->POS.x - row.startX) :
        (comp->POS.x - ((*(it - 1))->POS.x + lefData.getScaledMacroSize((*(it - 1))->modelName).first));

    double availableSpaceRight = (it + 1 == row.components.end()) ?
        (row.endX - (comp->POS.x + lefData.getScaledMacroSize(comp->modelName).first)) :
        ((*(it + 1))->POS.x - (comp->POS.x + lefData.getScaledMacroSize(comp->modelName).first));

    return (newSize.first <= (lefData.getScaledMacroSize(comp->modelName).first + availableSpaceLeft + availableSpaceRight));
}

bool CellReplacer::tryMoveToAdjacentRow(def::DEF_File& defFile, def::COMPONENTS_class* comp,
    const std::pair<double, double>& newSize, const std::vector<RowInfo>& rows) {
    return false;
}

void CellReplacer::adjustPlacementWithOrientation(def::DEF_File& defFile, def::COMPONENTS_class* component,
    const std::pair<double, double>& oldSize,
    const std::pair<double, double>& newSize) {
    double deltaX = newSize.first - oldSize.first;

    component->POS.orientation = def::Orientation::N; 

    auto& components = defFile.get_all_component();
    std::vector<def::COMPONENTS_class*> rowComponents;
    for (auto* comp : components) {
        if (comp->POS.y == component->POS.y) {
            rowComponents.push_back(comp);
        }
    }

    std::sort(rowComponents.begin(), rowComponents.end(),
        [](const def::COMPONENTS_class* a, const def::COMPONENTS_class* b) {
            return a->POS.x < b->POS.x;
        });

    auto it = std::find(rowComponents.begin(), rowComponents.end(), component);
    if (it == rowComponents.end()) return;

    for (auto next = it + 1; next != rowComponents.end(); ++next) {
        (*next)->POS.x += static_cast<int>(deltaX);
    }
}

void CellReplacer::updateDEFComponent(def::DEF_File& defFile, def::COMPONENTS_class* component, const std::string& OldCell, const std::string& NewCell, double maxÑapacitance) {

    const auto OldSize = lefData.getScaledMacroSize(OldCell);
    const auto NewSize = lefData.getScaledMacroSize(NewCell);

    component->modelName = NewCell;

    if (OldSize != NewSize) {
        adjustPlacementWithOrientation(defFile, component, OldSize, NewSize);

        if (component->POS.orientation == def::Orientation::FS) {
            component->POS.y -= static_cast<int>(NewSize.second);
        }
        
        std::cout << "Adjusted placement for " << component->compName
            << ": new position (" << component->POS.x << ", " << component->POS.y << ")"
            << ", orientation: " << static_cast<int>(component->POS.orientation)
            << ", size: " << NewSize.first << "x" << NewSize.second << " nm" << std::endl;

        std::cout << "Before adjustment - Neighboring cells:\n";
        for (auto* other : defFile.get_all_component()) {
            if (other->POS.y == component->POS.y && other != component) {
                std::cout << "  " << other->compName << ": (" << other->POS.x << ", " << other->POS.y << ")\n";
            }
        }
        
    }

    if (component->FIXED == def::FIXED_class::UNPLACED) {
        component->FIXED = def::FIXED_class::PLACED;
        component->SOURCE = def::SOURCE_class::TIMING;
    }
}

bool CellReplacer::compactLayout(def::DEF_File& defFile, const std::unordered_map<std::string, std::pair<std::string, std::string>>& replacementMap) {
    auto& components = const_cast<std::vector<def::COMPONENTS_class*>&>(defFile.get_all_component());
    std::sort(components.begin(), components.end(), [](const auto* a, const auto* b) {
        return std::tie(a->POS.y, a->POS.x) < std::tie(b->POS.y, b->POS.x);
        });

    double rowHeight = getRowHeight();
    double maxRowWidth = getMaxRowWidth();
    double currentY = components.empty() ? 0 : components[0]->POS.y;
    double currentX = defFile.DIEAREA.x1;


    std::unordered_map<std::string, def::Position> originalPositions;
    for (auto* comp : components) {
        originalPositions[comp->compName] = comp->POS;
    }

    for (auto* comp : components) {
        auto size = lefData.getScaledMacroSize(comp->modelName);

        if (currentX + size.first > maxRowWidth) {
            if (replacementMap.find(comp->compName) != replacementMap.end()) {
                comp->modelName = replacementMap.at(comp->compName).first;
                size = lefData.getScaledMacroSize(comp->modelName);
            }

            if (currentX + size.first > maxRowWidth) {
                currentY += rowHeight;
                currentX = defFile.DIEAREA.x1;

                if (currentX + size.first > maxRowWidth) {

                    for (auto* c : components) {
                        c->POS = originalPositions[c->compName];
                    }
                    return false;
                }
            }
        }

        comp->POS.x = static_cast<int>(currentX);
        comp->POS.y = static_cast<int>(currentY);
        currentX += size.first;
    }

    return true;
}



