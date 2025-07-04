#pragma once
#include "DEFReader.hpp"
#include "DEFWriter.hpp"
string Orientation_transform(Orientation o) {
    if (o == 0) return "N";
    if (o == 1) return "S";
    if (o == 2) return "E";
    if (o == 3) return "W";
    if (o == 4) return "FN";
    if (o == 5) return "FS";
    if (o == 6) return "FE";
    if (o == 7) return "FW";
}
string SOURCE_class_transform(SOURCE_class o) {
    if (o == 0) return "NETLIST";
    if (o == 1) return "DIST";
    if (o == 2) return "USER";
    if (o == 3) return "TIMING";
   
}
string FIXED_class_transform(FIXED_class o) {
    if (o == 0) return "FIXED";
    if (o == 1) return "COVER";
    if (o == 2) return "PLACED";
    if (o == 3) return "UNPLACED";

}
string DIRECTION_class_transform(DIRECTION_class o){
    if (o == 0) return "INPUT";
    if (o == 1) return "OUTPUT";
    if (o == 2) return "INOUT";
    if (o == 3) return "FEEDTHRU";

}
string USE_class_transform(USE_class o)  {
    if (o == 0) return "SIGNAL";
    if (o == 1) return "POWER";
    if (o == 2) return "GROUND";
    if (o == 3) return "CLOCK";
    if (o == 4) return "TIEOFF";
    if (o == 5) return "ANALOG";
    if (o == 6) return "SCAN";
    if (o == 7) return "RESET";
}

void OutDEF(DEF_File* def, string nameOutFile) {

    ofstream outFile(nameOutFile);
    if (!outFile.is_open()) {
        throw std::runtime_error("Could not open file: ");
    }

    outFile << "VERSION " << def->version << " ;" << endl;
    outFile << "DIVIDERCHAR \"" << def->DIVIDERCHAR << "\" ;" << endl;
    ///outFile << "BUSBITCHARS \"" << def->BUSBITCHARS[0] << def->BUSBITCHARS[1] << "\" ;" << endl;
    outFile << "DESIGN " << def->DESIGN <<   " ;" << endl;
    outFile << "UNITS DISTANCE MICRONS " << def->UNITS_DISTANCE_MICRONS << " ;" << endl;
    
    outFile << "DIEAREA ( " << def->DIEAREA.x1 << " " << def->DIEAREA.y1 << " ) ( " << def->DIEAREA.x2 << " " << def->DIEAREA.y2 << " ) ;" << endl;



    for (int i = 0; i < (def->beginning.size()); i++) {
        outFile <<def->beginning[i] << endl;
    }

    outFile << "VIAS " << def->COUNT_VIAS << " ;"<< endl;
    for (int i = 0; i < def->VIAS_str.size();i++) {
        outFile << "\t" << def->VIAS_str[i] << endl;
    }
    outFile << "END VIAS" << endl;
    outFile << "COMPONENTS " << def->COMPONENTS.size() << " ;" << endl;
    for (int i = 0; i < def->COMPONENTS.size(); i++) {
        outFile << "\t" <<"- " << def->COMPONENTS[i].compName << " " << def->COMPONENTS[i].modelName << " + " ;
        if ((def->COMPONENTS[i].SOURCE == 0) || (def->COMPONENTS[i].SOURCE == 1) || (def->COMPONENTS[i].SOURCE == 2) || (def->COMPONENTS[i].SOURCE == 3)) {
            outFile << "SOURCE "<< SOURCE_class_transform(def->COMPONENTS[i].SOURCE) << " + ";
        }
        outFile << FIXED_class_transform(def->COMPONENTS[i].FIXED) << " ( " << def->COMPONENTS[i].POS.x << " " << def->COMPONENTS[i].POS.y << " ) "<< Orientation_transform(def->COMPONENTS[i].POS.orientation) << " ;" << endl;
    }
    outFile << "END COMPONENTS" << endl;
    outFile << "PINS " << def->PINS.size() << " ;" << endl;
    for (int i = 0; i < def->PINS.size(); i++) {
        outFile << "\t- " << def->PINS[i].pinName << " + NET " << def->PINS[i].netName << " + ";
        if (def->PINS[i].SPECIAL == 1) {
            outFile << "SPECIAL" << " + ";
        }
        outFile << "DIRECTION " << DIRECTION_class_transform(def->PINS[i].DIRECTION) << " + USE " << USE_class_transform(def->PINS[i].USE) << endl;
        outFile << "\t  + PORT" << endl;
        for (int o = 0; o < def->PINS[i].LAYER.size();o++) {
            outFile << "\t\t+ LAYER " << def->PINS[i].LAYER[o].layerName << " ( " << def->PINS[i].LAYER[o].rect.x1 << " " << def->PINS[i].LAYER[o].rect.x2 << " ) ( " << def->PINS[i].LAYER[o].rect.y1 << " " << def->PINS[i].LAYER[o].rect.y2 << " )" << endl;
        }
        outFile << "\t\t+ " << FIXED_class_transform(def->PINS[i].PLACED_PIN) << " ( " << def->PINS[i].POS.x << " " << def->PINS[i].POS.y << " ) " << Orientation_transform(def->PINS[i].POS.orientation) << " ;"<< endl;
    }
    outFile << "END PINS" << endl;

    outFile << "SPECIALNETS " << def->SPECIALNETS.size() << " ;"<< endl;
    for (int i = 0; i < def->SPECIALNETS.size(); i++) {
        outFile << "\t- " << def->SPECIALNETS[i].netName;
        for (int o = 0; o < def->SPECIALNETS[i].Net_unit.size(); o++) {
            outFile << " ( " << def->SPECIALNETS[i].Net_unit[o].first << " " << def->SPECIALNETS[i].Net_unit[o].second << " )";
        }
        outFile << " + USE " << USE_class_transform(def->SPECIALNETS[i].USE) << endl;
    }
    
    outFile << "END SPECIALNETS" << endl;

    outFile << "NETS " << def->NETS.size() << " ;" << endl;
    for (int i = 0; i < def->NETS.size(); i++) {
        outFile << "\t- " << def->NETS[i].netName;
        for (int o = 0; o < def->NETS[i].Net_unit.size(); o++) {
            outFile << " ( " << def->NETS[i].Net_unit[o].first << " " << def->NETS[i].Net_unit[o].second << " )";
            if ((o != 0)&&(o%5 == 4 )) outFile << endl;
        }
        outFile << endl;
    }
    outFile << "END NETS" << endl;
    outFile << "END DESIGN" << endl;

    outFile.close();
}