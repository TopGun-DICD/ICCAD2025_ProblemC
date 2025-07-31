#pragma once

#include "DEFReader.hpp"

#include "NoBoostSplit.hpp"

bool def::DEFReader::ReadDEF(const std::string &nameInFile, def::DEF_File &def, verilog::Netlist& netlist) {
    //
    std::string buffer;
    std::vector<std::string> split_buffer;
    std::string BUS_DIV;
    int flagPaternov = 0;
    std::ifstream inFile(nameInFile);
    if (!inFile.is_open()) {
        throw std::runtime_error("Could not open file: ");
    }
    int i = 0;

    while (getline(inFile, buffer)) {
        i++;
        if (!buffer.empty()) {
           
            my_boost::trim(buffer);
            my_boost::split(split_buffer, buffer, my_boost::is_any_of<char>(" "), true);



            if (buffer == "END DESIGN") {
                break;
            }

            if (split_buffer.front() == "VERSION") {
                def.version = stof(split_buffer[1]);
                flagPaternov = 1;
            }
            if (split_buffer.front() == "DIVIDERCHAR") {
                BUS_DIV = split_buffer[1];
                def.DIVIDERCHAR = BUS_DIV[1];
                flagPaternov = 1;
            }
            if (split_buffer.front() == "BUSBITCHARS") {
                BUS_DIV = split_buffer[1];
                def.BUSBITCHARS[0] = BUS_DIV[1];
                def.BUSBITCHARS[1] = BUS_DIV[2];

                flagPaternov = 1;
            }
            if (split_buffer.front() == "PROPERTYDEFINITIONS") {
                parsePROPERTYDEFINITIONS(&inFile, def, &i);
                flagPaternov = 1;
            }
            if (split_buffer.front() == "DESIGN") {
                def.DESIGN = split_buffer[1];
                flagPaternov = 1;
            }
            if (split_buffer.front() == "COMPONENTS") {
                def.COUNT_COMPONENTS = stoi(split_buffer[1]);
                parseComponents(&inFile, def, &i, netlist);
                //

                flagPaternov = 1;
            }
            if (split_buffer.front() == "VIAS") {
                def.COUNT_VIAS = stoi(split_buffer[1]);
                parseVias(&inFile, def, &i);
                flagPaternov = 1;
            }
            if (split_buffer.front() == "PINS") {
                def.COUNT_PINS = stoi(split_buffer[1]);
                parsePins(&inFile, def, &i);
                flagPaternov = 1;
            }
            if (split_buffer.front() == "NETS") {
                def.COUNT_NETS = stoi(split_buffer[1]);
                parseNets(&inFile, def, &i);
                flagPaternov = 1;
            }
            if (split_buffer.front() == "SPECIALNETS") {
                def.COUNT_SPECIALNETS = stoi(split_buffer[1]);
                parseSpecialnets(&inFile, def, &i);
                flagPaternov = 1;
            }
            if (split_buffer.front() == "UNITS") {
                def.UNITS_DISTANCE_MICRONS = stoi(split_buffer[3]);
                flagPaternov = 1;
            }
            if (split_buffer.front() == "DIEAREA") {
                def.DIEAREA.x1 = stoi(split_buffer[2]);
                def.DIEAREA.y1 = stoi(split_buffer[3]);
                def.DIEAREA.x2 = stoi(split_buffer[6]);
                def.DIEAREA.y2 = stoi(split_buffer[7]);
                flagPaternov = 1;
            }

            std::string ifter;
            if (!buffer.empty()) {
                ifter = split_buffer[0];
            }

            if ((flagPaternov == 0) && (split_buffer[0] != "#") && (buffer != "###############################################################") ) {

                if (!buffer.empty()) {
                    def.beginning.push_back(buffer);
                }

                //cout <<"error" ;
            }



            // if (i>1000) break;
            split_buffer.clear();
            flagPaternov = 0;

        }
           
        
    }
    inFile.close();
    return true;
}



void def::DEFReader::parseComponents(std::ifstream* inFile, DEF_File& def, int* i, verilog::Netlist& netlist) {
    //
    std::string buffer;
    std::vector<std::string> split_buffer;

    //cout << "START COMPONENTS " << *i << endl;

    while (getline(*inFile, buffer)) {
        *i = *i + 1;

        if ((!buffer.empty()) && (buffer.size() > 3)) {

            my_boost::trim(buffer);
            my_boost::split(split_buffer, buffer, my_boost::is_any_of<char>(" "), true);
            if (buffer == "END COMPONENTS") {
                // cout << "END COMPONENTS " << *i << endl;          
                return;
            }

            def.push_back_COMPONENTS(split_buffer[1], split_buffer[2]);
            //*
            verilog::Instance *instance = netlist.top->getInstanceByName(split_buffer[1]);
            if (!instance)
                instance = netlist.top->getInstanceByDEFName(split_buffer[1]);
            instance->placement.component = def.COMPONENTS.back();
           // */
            for (const auto& word : split_buffer) {
                if (word == "PLACED") {
                    def.COMPONENTS.back()->FIXED = def::FIXED_class::PLACED;
                }
                if (word == "COVER") {
                    def.COMPONENTS.back()->FIXED = FIXED_class::COVER;
                }
                if (word == "UNPLACED") {
                    def.COMPONENTS.back()->FIXED = FIXED_class::UNPLACED;
                }
                if (word == "FIXED") {
                    def.COMPONENTS.back()->FIXED = FIXED_class::FIXED;
                }
                if (word == "NETLIST") {
                    def.COMPONENTS.back()->SOURCE = SOURCE_class::NETLIST;
                }
                if (word == "USER") {
                    def.COMPONENTS.back()->SOURCE = SOURCE_class::USER;
                }
                if (word == "DIST") {
                    def.COMPONENTS.back()->SOURCE = SOURCE_class::DIST;
                }
                if (word == "TIMING") {
                    def.COMPONENTS.back()->SOURCE = SOURCE_class::TIMING;
                }
                if (word == "(") {
                    def.COMPONENTS.back()->POS.x = stoi(*(&word + 1));
                    def.COMPONENTS.back()->POS.y = stoi(*(&word + 2));
                    //  ;
                }
                if (word == "N") {
                    def.COMPONENTS.back()->POS.orientation = Orientation::N;
                }
                if (word == "S") {
                    def.COMPONENTS.back()->POS.orientation = Orientation::S;
                }
                if (word == "E") {
                    def.COMPONENTS.back()->POS.orientation = Orientation::E;
                }
                if (word == "W") {
                    def.COMPONENTS.back()->POS.orientation = Orientation::W;
                }
                if (word == "FN") {
                    def.COMPONENTS.back()->POS.orientation = Orientation::FN;
                }
                if (word == "FS") {
                    def.COMPONENTS.back()->POS.orientation = Orientation::FS;
                }
                if (word == "FE") {
                    def.COMPONENTS.back()->POS.orientation = Orientation::FE;
                }
                if (word == "FW") {
                    def.COMPONENTS.back()->POS.orientation = Orientation::FW;
                }


            }
        }
        split_buffer.clear();
    }
}
void def::DEFReader::parseNets(std::ifstream* inFile, DEF_File& def, int* i) {
    std::string buffer;
    std::vector<std::string> split_buffer;
    while (getline(*inFile, buffer)) {
        *i = *i + 1;
        my_boost::trim(buffer);
        my_boost::split(split_buffer, buffer, my_boost::is_any_of<char>(" "), true);
        if (buffer == "END NETS") {
            //cout << "END Nets " << *i << endl;
            return;
        }
        if (split_buffer[0] == "-") {
            def.push_back_NETS(split_buffer[1]);
            for (const auto& word : split_buffer) {
                if (word == "(") {
                    def.push_back_NETS_rect_u((*(&word + 1)), (*(&word + 2)));
                }
                if (word == "USE") {
                    if (*(&word + 1) == "SIGNAL") {
                        def.NETS.back()->USE = USE_class::SIGNAL;
                    }
                    if (*(&word + 1) == "POWER") {
                        def.NETS.back()->USE = USE_class::POWER;
                    }
                    if (*(&word + 1) == "GROUND") {
                        def.NETS.back()->USE = USE_class::GROUND;
                    }
                    if (*(&word + 1) == "CLOCK") {
                        def.NETS.back()->USE = USE_class::CLOCK;
                    }
                    if (*(&word + 1) == "TIEOFF") {
                        def.NETS.back()->USE = USE_class::TIEOFF;
                    }
                    if (*(&word + 1) == "ANALOG") {
                        def.NETS.back()->USE = USE_class::ANALOG;
                    }
                    if (*(&word + 1) == "SCAN") {
                        def.NETS.back()->USE = USE_class::SCAN;
                    }
                    if (*(&word + 1) == "RESET") {
                        def.NETS.back()->USE = USE_class::RESET;
                    }
                }

            }
        }
        if (split_buffer[0] == "(") {

            for (const auto& word : split_buffer) {
                if (word == "(") {
                    def.push_back_NETS_rect_u((*(&word + 1)), (*(&word + 2)));
                }
                if (word == "USE") {
                    if (*(&word + 1) == "SIGNAL") {
                        def.NETS.back()->USE = USE_class::SIGNAL;
                    }
                    if (*(&word + 1) == "POWER") {
                        def.NETS.back()->USE = USE_class::POWER;
                    }
                    if (*(&word + 1) == "GROUND") {
                        def.NETS.back()->USE = USE_class::GROUND;
                    }
                    if (*(&word + 1) == "CLOCK") {
                        def.NETS.back()->USE = USE_class::CLOCK;
                    }
                    if (*(&word + 1) == "TIEOFF") {
                        def.NETS.back()->USE = USE_class::TIEOFF;
                    }
                    if (*(&word + 1) == "ANALOG") {
                        def.NETS.back()->USE = USE_class::ANALOG;
                    }
                    if (*(&word + 1) == "SCAN") {
                        def.NETS.back()->USE = USE_class::SCAN;
                    }
                    if (*(&word + 1) == "RESET") {
                        def.NETS.back()->USE = USE_class::RESET;
                    }
                }
            }
        }


        split_buffer.clear();
    }

}
void def::DEFReader::parseSpecialnets(std::ifstream* inFile, DEF_File& def, int* i) {
    std::string buffer;
    std::vector<std::string> split_buffer;
    // cout << "START SPECIALNETS " << *i << endl;

    while (getline(*inFile, buffer)) {
        *i = *i + 1;
        my_boost::trim(buffer);
        my_boost::split(split_buffer, buffer, my_boost::is_any_of<char>(" "), true);
        if (buffer == "END SPECIALNETS") {
            // cout << "END SPECIALNETS " << *i << endl;
            return;
        }

        if (split_buffer[0] == "-") {
            def.push_back_SPECIALNETS(split_buffer[1]);
            for (const auto& word : split_buffer) {
                if (word == "(") {
                    def.push_back_SPECIALNETS_rect_u((*(&word + 1)), (*(&word + 2)));
                }
                if (word == "USE") {
                    if (*(&word + 1) == "SIGNAL") {
                        def.SPECIALNETS.back()->USE = USE_class::SIGNAL;
                    }
                    if (*(&word + 1) == "POWER") {
                        def.SPECIALNETS.back()->USE = USE_class::POWER;
                    }
                    if (*(&word + 1) == "GROUND") {
                        def.SPECIALNETS.back()->USE = USE_class::GROUND;
                    }
                    if (*(&word + 1) == "CLOCK") {
                        def.SPECIALNETS.back()->USE = USE_class::CLOCK;
                    }
                    if (*(&word + 1) == "TIEOFF") {
                        def.SPECIALNETS.back()->USE = USE_class::TIEOFF;
                    }
                    if (*(&word + 1) == "ANALOG") {
                        def.SPECIALNETS.back()->USE = USE_class::ANALOG;
                    }
                    if (*(&word + 1) == "SCAN") {
                        def.SPECIALNETS.back()->USE = USE_class::SCAN;
                    }
                    if (*(&word + 1) == "RESET") {
                        def.SPECIALNETS.back()->USE = USE_class::RESET;
                    }
                }

            }
        }
        if (split_buffer[0] == "(") {

            for (const auto& word : split_buffer) {
                if (word == "(") {
                    def.push_back_NETS_rect_u((*(&word + 1)), (*(&word + 2)));
                }
                if (word == "USE") {
                    if (*(&word + 1) == "SIGNAL") {
                        def.NETS.back()->USE = USE_class::SIGNAL;
                    }
                    if (*(&word + 1) == "POWER") {
                        def.NETS.back()->USE = USE_class::POWER;
                    }
                    if (*(&word + 1) == "GROUND") {
                        def.NETS.back()->USE = USE_class::GROUND;
                    }
                    if (*(&word + 1) == "CLOCK") {
                        def.NETS.back()->USE = USE_class::CLOCK;
                    }
                    if (*(&word + 1) == "TIEOFF") {
                        def.NETS.back()->USE = USE_class::TIEOFF;
                    }
                    if (*(&word + 1) == "ANALOG") {
                        def.NETS.back()->USE = USE_class::ANALOG;
                    }
                    if (*(&word + 1) == "SCAN") {
                        def.NETS.back()->USE = USE_class::SCAN;
                    }
                    if (*(&word + 1) == "RESET") {
                        def.NETS.back()->USE = USE_class::RESET;
                    }
                }
            }
        }

        split_buffer.clear();
    }

}
void def::DEFReader::parsePins(std::ifstream* inFile, DEF_File& def, int* i) {
    std::string buffer;
    std::string buffer_end;
    std::vector<std::string> split_buffer;
    // cout << "START PINS " << *i << endl;

    while (getline(*inFile, buffer)) {
        *i = *i + 1;
        my_boost::trim(buffer);
        my_boost::split(split_buffer, buffer, my_boost::is_any_of<char>(" "), true);
        if (buffer == "END PINS") {
            //cout << "END PINS " << *i << endl;
            return;
        }
        if (split_buffer[0] == "-") {
            def.push_back_PINS(split_buffer[1], split_buffer[4]);
        }

        for (const auto& word : split_buffer) {
            if (word == "SPECIAL") {
                def.PINS.back()->SPECIAL = 1;
            }
            if (word == "DIRECTION") {
                if (*(&word + 1) == "INPUT") {
                    def.PINS.back()->DIRECTION = DIRECTION_class::INPUT;
                }
                if (*(&word + 1) == "OUTPUT") {
                    def.PINS.back()->DIRECTION = DIRECTION_class::OUTPUT;
                }
                if (*(&word + 1) == "INOUT") {
                    def.PINS.back()->DIRECTION = DIRECTION_class::INOUT;
                }
                if (*(&word + 1) == "FEEDTHRU") {
                    def.PINS.back()->DIRECTION = DIRECTION_class::FEEDTHRU;
                }
            }

            if (word == "USE") {
                if (*(&word + 1) == "SIGNAL") {
                    def.PINS.back()->USE = USE_class::SIGNAL;
                }
                if (*(&word + 1) == "POWER") {
                    def.PINS.back()->USE = USE_class::POWER;
                }
                if (*(&word + 1) == "GROUND") {
                    def.PINS.back()->USE = USE_class::GROUND;
                }
                if (*(&word + 1) == "CLOCK") {
                    def.PINS.back()->USE = USE_class::CLOCK;
                }
                if (*(&word + 1) == "TIEOFF") {
                    def.PINS.back()->USE = USE_class::TIEOFF;
                }
                if (*(&word + 1) == "ANALOG") {
                    def.PINS.back()->USE = USE_class::ANALOG;
                }
                if (*(&word + 1) == "SCAN") {
                    def.PINS.back()->USE = USE_class::SCAN;
                }
                if (*(&word + 1) == "RESET") {
                    def.PINS.back()->USE = USE_class::RESET;
                }
            }
            if (word == "LAYER") {
                LAYER_class o;
                def.PINS.back()->LAYER.push_back(o);
                def.PINS.back()->LAYER[def.PINS.back()->LAYER.size() - 1].layerName = (*(&word + 1));
                def.PINS.back()->LAYER[def.PINS.back()->LAYER.size() - 1].rect.x1 = stoi(*(&word + 3));
                def.PINS.back()->LAYER[def.PINS.back()->LAYER.size() - 1].rect.x2 = stoi(*(&word + 4));
                def.PINS.back()->LAYER[def.PINS.back()->LAYER.size() - 1].rect.y1 = stoi(*(&word + 7));
                def.PINS.back()->LAYER[def.PINS.back()->LAYER.size() - 1].rect.y2 = stoi(*(&word + 8));

            }

            if (word == "PLACED") {
                def.PINS.back()->PLACED_PIN = FIXED_class::PLACED;
                def.PINS.back()->POS.x = stoi(*(&word + 2));
                def.PINS.back()->POS.y = stoi(*(&word + 3));
            }
            if (word == "COVER") {
                def.PINS.back()->PLACED_PIN = FIXED_class::COVER;
                def.PINS.back()->POS.x = stoi(*(&word + 2));
                def.PINS.back()->POS.y = stoi(*(&word + 3));
            }
            if (word == "UNPLACED") {
                def.PINS.back()->PLACED_PIN = FIXED_class::UNPLACED;
                def.PINS.back()->POS.x = stoi(*(&word + 2));
                def.PINS.back()->POS.y = stoi(*(&word + 3));
            }
            if (word == "FIXED") {
                def.PINS.back()->PLACED_PIN = FIXED_class::FIXED;
                def.PINS.back()->POS.x = stoi(*(&word + 2));
                def.PINS.back()->POS.y = stoi(*(&word + 3));
            }
            if (word == "N") {
                def.PINS.back()->POS.orientation = Orientation::N;
            }
            if (word == "S") {
                def.PINS.back()->POS.orientation = Orientation::S;
            }
            if (word == "E") {
                def.PINS.back()->POS.orientation = Orientation::E;
            }
            if (word == "W") {
                def.PINS.back()->POS.orientation = Orientation::W;
            }
            if (word == "FN") {
                def.PINS.back()->POS.orientation = Orientation::FN;
            }
            if (word == "FS") {
                def.PINS.back()->POS.orientation = Orientation::FS;
            }
            if (word == "FE") {
                def.PINS.back()->POS.orientation = Orientation::FE;
            }
            if (word == "FW") {
                def.PINS.back()->POS.orientation = Orientation::FW;
            }
        }


        split_buffer.clear();
    }

}
void def::DEFReader::parsePROPERTYDEFINITIONS(std::ifstream* inFile, DEF_File& def, int* i) {
    std::string buffer;
    std::vector<std::string> split_buffer;
    // cout << "START VIAS " << *i << endl;

    while (getline(*inFile, buffer)) {
        *i = *i + 1;
        
        if (!buffer.empty()) {
            my_boost::trim(buffer);
            my_boost::split(split_buffer, buffer, my_boost::is_any_of<char>(" "), true);
            if (buffer == "END PROPERTYDEFINITIONS") {
                //cout << "END VIAS " << *i << endl;         
                return;
            }
               def.PROPERTYDEFINITIONS.push_back(buffer);
        }

        split_buffer.clear();
    }


}
void def::DEFReader::parseVias(std::ifstream* inFile, DEF_File& def, int* i) {
    std::string buffer;
    std::vector<std::string> split_buffer;
    // cout << "START VIAS " << *i << endl;

    while (getline(*inFile, buffer)) {
        *i = *i + 1;
        if (!buffer.empty()) {
            my_boost::trim(buffer);
            my_boost::split(split_buffer, buffer, my_boost::is_any_of<char>(" "), true);
        }
        if (buffer == "END VIAS") {
            //cout << "END VIAS " << *i << endl;         
            return;
        }
        def.VIAS_str.push_back(buffer);

        split_buffer.clear();
    }

}

/*
void swup_comp(DEF_File* def, std::string name1, std::string name2) {
    COMPONENTS_clas buf;
    int adr = 0;
    for (int o = 0; o < def.COMPONENTS.size(); o++) {
        if (def.COMPONENTS[o].compName == name1) {
            buf.compName = def.C OMPONENTS[o].compName;
            buf.FIXED = def.COMPONENTS[o].FIXED;
            buf.modelName = def.COMPONENTS[o].modelName;
            buf.POS.x = def.COMPONENTS[o].POS.x;
            buf.POS.y = def.COMPONENTS[o].POS.y;
            buf.POS.orientation = def.COMPONENTS[o].POS.orientation;
            buf.SOURCE = def.COMPONENTS[o].SOURCE;
            adr = o;
        }
    }
    for (int o = 0; o < def.COMPONENTS.size(); o++) {
        if ((def.COMPONENTS[o].compName == name2) && (def.COMPONENTS[o].modelName == def.COMPONENTS[adr].modelName)) {
            def.COMPONENTS[adr].compName = def.COMPONENTS[o].compName;
            def.COMPONENTS[adr].FIXED = def.COMPONENTS[o].FIXED;
            def.COMPONENTS[adr].POS.x = def.COMPONENTS[o].POS.x;
            def.COMPONENTS[adr].POS.y = def.COMPONENTS[o].POS.y;
            def.COMPONENTS[adr].POS.orientation = def.COMPONENTS[o].POS.orientation;
            def.COMPONENTS[adr].SOURCE = def.COMPONENTS[o].SOURCE;

            def.COMPONENTS[o].compName = buf.compName;
            def.COMPONENTS[o].FIXED = buf.FIXED;
            def.COMPONENTS[o].modelName = buf.modelName;
            def.COMPONENTS[o].POS.x = buf.POS.x;
            def.COMPONENTS[o].POS.y = buf.POS.y;
            def.COMPONENTS[o].POS.orientation = buf.POS.orientation;
            def.COMPONENTS[o].SOURCE = buf.SOURCE;
            //cout << "Ver tipe" << endl;
        }
    }

}
//*/

