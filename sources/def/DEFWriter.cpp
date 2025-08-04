#pragma once

#include "DEFWriter.hpp"

#include <fstream>

std::string def::DEFWriter::Orientation_transform(Orientation o) {
    if (o == Orientation::N)    return "N";
    if (o == Orientation::S)    return "S";
    if (o == Orientation::E)    return "E";
    if (o == Orientation::W)    return "W";
    if (o == Orientation::FN)   return "FN";
    if (o == Orientation::FS)   return "FS";
    if (o == Orientation::FE)   return "FE";
    if (o == Orientation::FW)   return "FW";
    return std::string("");
}
std::string def::DEFWriter::SOURCE_class_transform(SOURCE_class o) {
    if (o == SOURCE_class::NETLIST) return "NETLIST";
    if (o == SOURCE_class::DIST)    return "DIST";
    if (o == SOURCE_class::USER)    return "USER";
    if (o == SOURCE_class::TIMING)  return "TIMING";
    return std::string("");
}
std::string def::DEFWriter::FIXED_class_transform(FIXED_class o) {
    if (o == FIXED_class::FIXED)    return "FIXED";
    if (o == FIXED_class::COVER)    return "COVER";
    if (o == FIXED_class::PLACED)   return "PLACED";
    if (o == FIXED_class::UNPLACED) return "UNPLACED";
    return std::string("");
}
std::string def::DEFWriter::DIRECTION_class_transform(DIRECTION_class o) {
    if (o == DIRECTION_class::INPUT)    return "INPUT";
    if (o == DIRECTION_class::OUTPUT)   return "OUTPUT";
    if (o == DIRECTION_class::INOUT)    return "INOUT";
    if (o == DIRECTION_class::FEEDTHRU) return "FEEDTHRU";
    return std::string("");
}
std::string def::DEFWriter::USE_class_transform(USE_class o) {
    if (o == USE_class::SIGNAL) return "SIGNAL";
    if (o == USE_class::POWER)  return "POWER";
    if (o == USE_class::GROUND) return "GROUND";
    if (o == USE_class::CLOCK)  return "CLOCK";
    if (o == USE_class::TIEOFF) return "TIEOFF";
    if (o == USE_class::ANALOG) return "ANALOG";
    if (o == USE_class::SCAN)   return "SCAN";
    if (o == USE_class::RESET)  return "RESET";
    return std::string("");
}

void def::DEFWriter::OutDEF(std::string nameOutFile, DEF_File& def, const std::vector<std::tuple<std::string, std::string, std::string, double, double, std::string>>& replacements) {

    std::ofstream outFile(nameOutFile);
    if (!outFile.is_open()) {
        throw std::runtime_error("Could not open file: ");
    }

    outFile << "VERSION " << def.version << " ;" << std::endl;
    outFile << "DIVIDERCHAR \"" << def.DIVIDERCHAR << "\" ;" << std::endl;
    outFile << "BUSBITCHARS \"" << def.BUSBITCHARS[0] << def.BUSBITCHARS[1] << "\" ;" << std::endl;
    outFile << "DESIGN " << def.DESIGN << " ;" << std::endl;
    outFile << "UNITS DISTANCE MICRONS " << def.UNITS_DISTANCE_MICRONS << " ;" << std::endl;

    outFile << "DIEAREA ( " << def.DIEAREA.x1 << " " << def.DIEAREA.y1 << " ) ( "
        << def.DIEAREA.x2 << " " << def.DIEAREA.y2 << " ) ;" << std::endl;



    for (int i = 0; i < (def.beginning.size()); i++) {
        //  def.PRO[i] = " o";
        outFile << def.beginning[i] << std::endl;
    }

    outFile << "VIAS " << def.COUNT_VIAS << " ;" << std::endl;
    for (int i = 0; i < def.VIAS_str.size();i++) {
        outFile << "\t" << def.VIAS_str[i] << std::endl;
    }
    outFile << "END VIAS" << std::endl;
    outFile << "COMPONENTS " << def.COMPONENTS.size() << " ;" << std::endl;
    for (int i = 0; i < def.COMPONENTS.size(); i++) {
        outFile << "\t" << "- " << def.COMPONENTS[i]->compName << " " << def.COMPONENTS[i]->modelName << " + ";
        if ((def.COMPONENTS[i]->SOURCE == SOURCE_class::NETLIST) || (def.COMPONENTS[i]->SOURCE == SOURCE_class::DIST) ||
            (def.COMPONENTS[i]->SOURCE == SOURCE_class::USER) || (def.COMPONENTS[i]->SOURCE == SOURCE_class::TIMING)) {
            outFile << "SOURCE " << SOURCE_class_transform(def.COMPONENTS[i]->SOURCE) << " + ";
        }
        outFile << FIXED_class_transform(def.COMPONENTS[i]->FIXED) << " ( " << def.COMPONENTS[i]->POS.x << " " << def.COMPONENTS[i]->POS.y << " ) " << Orientation_transform(def.COMPONENTS[i]->POS.orientation) << " ;" << std::endl;
    }
    outFile << "END COMPONENTS" << std::endl;
    outFile << "PINS " << def.PINS.size() << " ;" << std::endl;
    for (int i = 0; i < def.PINS.size(); i++) {
        outFile << "\t- " << def.PINS[i]->pinName << " + NET " << def.PINS[i]->netName << " + ";
        if (def.PINS[i]->SPECIAL == 1) {
            outFile << "SPECIAL" << " + ";
        }
        outFile << "DIRECTION " << DIRECTION_class_transform(def.PINS[i]->DIRECTION) << " + USE " << USE_class_transform(def.PINS[i]->USE) << std::endl;
        outFile << "\t  + PORT" << std::endl;
        for (int o = 0; o < def.PINS[i]->LAYER.size();o++) {
            outFile << "\t\t+ LAYER " << def.PINS[i]->LAYER[o].layerName << " ( " << def.PINS[i]->LAYER[o].rect.x1 << " " << def.PINS[i]->LAYER[o].rect.x2 << " ) ( " << def.PINS[i]->LAYER[o].rect.y1 << " " << def.PINS[i]->LAYER[o].rect.y2 << " )" << std::endl;
        }
        outFile << "\t\t+ " << FIXED_class_transform(def.PINS[i]->PLACED_PIN) << " ( " << def.PINS[i]->POS.x << " " << def.PINS[i]->POS.y << " ) " << Orientation_transform(def.PINS[i]->POS.orientation) << " ;" << std::endl;
    }
    outFile << "END PINS" << std::endl;

    outFile << "SPECIALNETS " << def.SPECIALNETS.size() << " ;" << std::endl;
    for (int i = 0; i < def.SPECIALNETS.size(); i++) {
        outFile << "\t- " << def.SPECIALNETS[i]->netName;
        for (int o = 0; o < def.SPECIALNETS[i]->Net_unit.size(); o++) {
            outFile << " ( " << def.SPECIALNETS[i]->Net_unit[o].first << " " << def.SPECIALNETS[i]->Net_unit[o].second << " )";
        }
        outFile << " + USE " << USE_class_transform(def.SPECIALNETS[i]->USE) << std::endl;
    }

    outFile << "END SPECIALNETS" << std::endl;

    outFile << "NETS " << def.NETS.size() << " ;" << std::endl;
    for (int i = 0; i < def.NETS.size(); i++) {
        outFile << "\t- " << def.NETS[i]->netName;
        for (int o = 0; o < def.NETS[i]->Net_unit.size(); o++) {
            outFile << " ( " << def.NETS[i]->Net_unit[o].first << " " << def.NETS[i]->Net_unit[o].second << " )";
            if ((o != 0) && (o % 5 == 4)) outFile << std::endl;
        }
        outFile << std::endl;
    }
    outFile << "END NETS" << std::endl;
    outFile << "END DESIGN" << std::endl;

    outFile.close();
}