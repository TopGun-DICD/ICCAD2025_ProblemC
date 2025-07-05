#pragma once
#ifndef DEF_PARSER_CPP
#define DEF_PARSER_CPP
#include "DEFReader.hpp"
#include "NoBoostSplit.hpp"

void parsComponents(ifstream* inFile, DEF_File* def, int* i) {
    string buffer;
    vector<string> split_buffer;

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

            def->push_back_COMPONENTS(split_buffer[1], split_buffer[2]);

            for (const auto& word : split_buffer) {
                if (word == "PLACED") {
                    def->COMPONENTS[def->COMPONENTS.size() - 1].FIXED = PLACED;
                }
                if (word == "COVER") {
                    def->COMPONENTS[def->COMPONENTS.size() - 1].FIXED = COVER;
                }
                if (word == "UNPLACED") {
                    def->COMPONENTS[def->COMPONENTS.size() - 1].FIXED = UNPLACED;
                }
                if (word == "FIXED") {
                    def->COMPONENTS[def->COMPONENTS.size() - 1].FIXED = FIXED;
                }
                if (word == "NETLIST") {
                    def->COMPONENTS[def->COMPONENTS.size() - 1].SOURCE = NETLIST;
                }
                if (word == "USER") {
                    def->COMPONENTS[def->COMPONENTS.size() - 1].SOURCE = USER;
                }
                if (word == "DIST") {
                    def->COMPONENTS[def->COMPONENTS.size() - 1].SOURCE = DIST;
                }
                if (word == "TIMING") {
                    def->COMPONENTS[def->COMPONENTS.size() - 1].SOURCE = TIMING;
                }
                if (word == "(") {
                    def->COMPONENTS[def->COMPONENTS.size() - 1].POS.x = stoi(*(&word + 1));
                    def->COMPONENTS[def->COMPONENTS.size() - 1].POS.y = stoi(*(&word + 2));
                    //  ;
                }
                if (word == "N") {
                    def->COMPONENTS[def->COMPONENTS.size() - 1].POS.orientation = N;
                }
                if (word == "S") {
                    def->COMPONENTS[def->COMPONENTS.size() - 1].POS.orientation = S;
                }
                if (word == "E") {
                    def->COMPONENTS[def->COMPONENTS.size() - 1].POS.orientation = E;
                }
                if (word == "W") {
                    def->COMPONENTS[def->COMPONENTS.size() - 1].POS.orientation = W;
                }
                if (word == "FN") {
                    def->COMPONENTS[def->COMPONENTS.size() - 1].POS.orientation = FN;
                }
                if (word == "FS") {
                    def->COMPONENTS[def->COMPONENTS.size() - 1].POS.orientation = FS;
                }
                if (word == "FE") {
                    def->COMPONENTS[def->COMPONENTS.size() - 1].POS.orientation = FE;
                }
                if (word == "FW") {
                    def->COMPONENTS[def->COMPONENTS.size() - 1].POS.orientation = FW;
                }


            }
        }
        split_buffer.clear();
    }
}
void parsNets(ifstream* inFile, DEF_File* def, int* i) {
    string buffer;
    vector<string> split_buffer;
    while (getline(*inFile, buffer)) {
        *i = *i + 1;
        my_boost::trim(buffer);
        my_boost::split(split_buffer, buffer, my_boost::is_any_of<char>(" "), true);
        if (buffer == "END NETS") {
            //cout << "END Nets " << *i << endl;
            return;
        }
        if (split_buffer[0] == "-") {
            def->push_back_NETS(split_buffer[1]);
            for (const auto& word : split_buffer) {
                if (word == "(") {
                    def->push_back_NETS_rect_u((*(&word + 1)), (*(&word + 2)));
                }
                if (word == "USE") {
                    if (*(&word + 1) == "SIGNAL") {
                        def->NETS[def->NETS.size() - 1].USE = SIGNAL;
                    }
                    if (*(&word + 1) == "POWER") {
                        def->NETS[def->NETS.size() - 1].USE = POWER;
                    }
                    if (*(&word + 1) == "GROUND") {
                        def->NETS[def->NETS.size() - 1].USE = GROUND;
                    }
                    if (*(&word + 1) == "CLOCK") {
                        def->NETS[def->NETS.size() - 1].USE = CLOCK;
                    }
                    if (*(&word + 1) == "TIEOFF") {
                        def->NETS[def->NETS.size() - 1].USE = TIEOFF;
                    }
                    if (*(&word + 1) == "ANALOG") {
                        def->NETS[def->NETS.size() - 1].USE = ANALOG;
                    }
                    if (*(&word + 1) == "SCAN") {
                        def->NETS[def->NETS.size() - 1].USE = SCAN;
                    }
                    if (*(&word + 1) == "RESET") {
                        def->NETS[def->NETS.size() - 1].USE = RESET;
                    }
                }

            }
        }
        if (split_buffer[0] == "(") {

            for (const auto& word : split_buffer) {
                if (word == "(") {
                    def->push_back_NETS_rect_u((*(&word + 1)), (*(&word + 2)));
                }
                if (word == "USE") {
                    if (*(&word + 1) == "SIGNAL") {
                        def->NETS[def->NETS.size() - 1].USE = SIGNAL;
                    }
                    if (*(&word + 1) == "POWER") {
                        def->NETS[def->NETS.size() - 1].USE = POWER;
                    }
                    if (*(&word + 1) == "GROUND") {
                        def->NETS[def->NETS.size() - 1].USE = GROUND;
                    }
                    if (*(&word + 1) == "CLOCK") {
                        def->NETS[def->NETS.size() - 1].USE = CLOCK;
                    }
                    if (*(&word + 1) == "TIEOFF") {
                        def->NETS[def->NETS.size() - 1].USE = TIEOFF;
                    }
                    if (*(&word + 1) == "ANALOG") {
                        def->NETS[def->NETS.size() - 1].USE = ANALOG;
                    }
                    if (*(&word + 1) == "SCAN") {
                        def->NETS[def->NETS.size() - 1].USE = SCAN;
                    }
                    if (*(&word + 1) == "RESET") {
                        def->NETS[def->NETS.size() - 1].USE = RESET;
                    }
                }
            }
        }


        split_buffer.clear();
    }

}
void parsSpecialnets(ifstream* inFile, DEF_File* def, int* i) {
    string buffer;
    vector<string> split_buffer;
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
            def->push_back_SPECIALNETS(split_buffer[1]);
            for (const auto& word : split_buffer) {
                if (word == "(") {
                    def->push_back_SPECIALNETS_rect_u((*(&word + 1)), (*(&word + 2)));
                }
                if (word == "USE") {
                    if (*(&word + 1) == "SIGNAL") {
                        def->SPECIALNETS[def->SPECIALNETS.size() - 1].USE = SIGNAL;
                    }
                    if (*(&word + 1) == "POWER") {
                        def->SPECIALNETS[def->SPECIALNETS.size() - 1].USE = POWER;
                    }
                    if (*(&word + 1) == "GROUND") {
                        def->SPECIALNETS[def->SPECIALNETS.size() - 1].USE = GROUND;
                    }
                    if (*(&word + 1) == "CLOCK") {
                        def->SPECIALNETS[def->SPECIALNETS.size() - 1].USE = CLOCK;
                    }
                    if (*(&word + 1) == "TIEOFF") {
                        def->SPECIALNETS[def->SPECIALNETS.size() - 1].USE = TIEOFF;
                    }
                    if (*(&word + 1) == "ANALOG") {
                        def->SPECIALNETS[def->SPECIALNETS.size() - 1].USE = ANALOG;
                    }
                    if (*(&word + 1) == "SCAN") {
                        def->SPECIALNETS[def->SPECIALNETS.size() - 1].USE = SCAN;
                    }
                    if (*(&word + 1) == "RESET") {
                        def->SPECIALNETS[def->SPECIALNETS.size() - 1].USE = RESET;
                    }
                }

            }
        }
        if (split_buffer[0] == "(") {

            for (const auto& word : split_buffer) {
                if (word == "(") {
                    def->push_back_NETS_rect_u((*(&word + 1)), (*(&word + 2)));
                }
                if (word == "USE") {
                    if (*(&word + 1) == "SIGNAL") {
                        def->NETS[def->NETS.size() - 1].USE = SIGNAL;
                    }
                    if (*(&word + 1) == "POWER") {
                        def->NETS[def->NETS.size() - 1].USE = POWER;
                    }
                    if (*(&word + 1) == "GROUND") {
                        def->NETS[def->NETS.size() - 1].USE = GROUND;
                    }
                    if (*(&word + 1) == "CLOCK") {
                        def->NETS[def->NETS.size() - 1].USE = CLOCK;
                    }
                    if (*(&word + 1) == "TIEOFF") {
                        def->NETS[def->NETS.size() - 1].USE = TIEOFF;
                    }
                    if (*(&word + 1) == "ANALOG") {
                        def->NETS[def->NETS.size() - 1].USE = ANALOG;
                    }
                    if (*(&word + 1) == "SCAN") {
                        def->NETS[def->NETS.size() - 1].USE = SCAN;
                    }
                    if (*(&word + 1) == "RESET") {
                        def->NETS[def->NETS.size() - 1].USE = RESET;
                    }
                }
            }
        }

        split_buffer.clear();
    }

}
void parsPins(ifstream* inFile, DEF_File* def, int* i) {
    string buffer;
    string buffer_end;
    vector<string> split_buffer;
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
            def->push_back_PINS(split_buffer[1], split_buffer[4]);
        }

        for (const auto& word : split_buffer) {
            if (word == "SPECIAL") {
                def->PINS[def->PINS.size() - 1].SPECIAL = 1;
            }
            if (word == "DIRECTION") {
                if (*(&word + 1) == "INPUT") {
                    def->PINS[def->PINS.size() - 1].DIRECTION = INPUT;
                }
                if (*(&word + 1) == "OUTPUT") {
                    def->PINS[def->PINS.size() - 1].DIRECTION = OUTPUT;
                }
                if (*(&word + 1) == "INOUT") {
                    def->PINS[def->PINS.size() - 1].DIRECTION = INOUT;
                }
                if (*(&word + 1) == "FEEDTHRU") {
                    def->PINS[def->PINS.size() - 1].DIRECTION = FEEDTHRU;
                }
            }

            if (word == "USE") {
                if (*(&word + 1) == "SIGNAL") {
                    def->PINS[def->PINS.size() - 1].USE = SIGNAL;
                }
                if (*(&word + 1) == "POWER") {
                    def->PINS[def->PINS.size() - 1].USE = POWER;
                }
                if (*(&word + 1) == "GROUND") {
                    def->PINS[def->PINS.size() - 1].USE = GROUND;
                }
                if (*(&word + 1) == "CLOCK") {
                    def->PINS[def->PINS.size() - 1].USE = CLOCK;
                }
                if (*(&word + 1) == "TIEOFF") {
                    def->PINS[def->PINS.size() - 1].USE = TIEOFF;
                }
                if (*(&word + 1) == "ANALOG") {
                    def->PINS[def->PINS.size() - 1].USE = ANALOG;
                }
                if (*(&word + 1) == "SCAN") {
                    def->PINS[def->PINS.size() - 1].USE = SCAN;
                }
                if (*(&word + 1) == "RESET") {
                    def->PINS[def->PINS.size() - 1].USE = RESET;
                }
            }
            if (word == "LAYER") {
                LAYER_class o;
                def->PINS[def->PINS.size() - 1].LAYER.push_back(o);
                def->PINS[def->PINS.size() - 1].LAYER[def->PINS[def->PINS.size() - 1].LAYER.size() - 1].layerName = (*(&word + 1));
                def->PINS[def->PINS.size() - 1].LAYER[def->PINS[def->PINS.size() - 1].LAYER.size() - 1].rect.x1 = stoi(*(&word + 3));
                def->PINS[def->PINS.size() - 1].LAYER[def->PINS[def->PINS.size() - 1].LAYER.size() - 1].rect.x2 = stoi(*(&word + 4));
                def->PINS[def->PINS.size() - 1].LAYER[def->PINS[def->PINS.size() - 1].LAYER.size() - 1].rect.y1 = stoi(*(&word + 7));
                def->PINS[def->PINS.size() - 1].LAYER[def->PINS[def->PINS.size() - 1].LAYER.size() - 1].rect.y2 = stoi(*(&word + 8));

            }

            if (word == "PLACED") {
                def->PINS[def->PINS.size() - 1].PLACED_PIN = PLACED;
                def->PINS[def->PINS.size() - 1].POS.x = stoi(*(&word + 2));
                def->PINS[def->PINS.size() - 1].POS.y = stoi(*(&word + 3));
            }
            if (word == "COVER") {
                def->PINS[def->PINS.size() - 1].PLACED_PIN = COVER;
                def->PINS[def->PINS.size() - 1].POS.x = stoi(*(&word + 2));
                def->PINS[def->PINS.size() - 1].POS.y = stoi(*(&word + 3));
            }
            if (word == "UNPLACED") {
                def->PINS[def->PINS.size() - 1].PLACED_PIN = UNPLACED;
                def->PINS[def->PINS.size() - 1].POS.x = stoi(*(&word + 2));
                def->PINS[def->PINS.size() - 1].POS.y = stoi(*(&word + 3));
            }
            if (word == "FIXED") {
                def->PINS[def->PINS.size() - 1].PLACED_PIN = FIXED;
                def->PINS[def->PINS.size() - 1].POS.x = stoi(*(&word + 2));
                def->PINS[def->PINS.size() - 1].POS.y = stoi(*(&word + 3));
            }
            if (word == "N") {
                def->PINS[def->PINS.size() - 1].POS.orientation = N;
            }
            if (word == "S") {
                def->PINS[def->PINS.size() - 1].POS.orientation = S;
            }
            if (word == "E") {
                def->PINS[def->PINS.size() - 1].POS.orientation = E;
            }
            if (word == "W") {
                def->PINS[def->PINS.size() - 1].POS.orientation = W;
            }
            if (word == "FN") {
                def->PINS[def->PINS.size() - 1].POS.orientation = FN;
            }
            if (word == "FS") {
                def->PINS[def->PINS.size() - 1].POS.orientation = FS;
            }
            if (word == "FE") {
                def->PINS[def->PINS.size() - 1].POS.orientation = FE;
            }
            if (word == "FW") {
                def->PINS[def->PINS.size() - 1].POS.orientation = FW;
            }
        }


        split_buffer.clear();
    }

}
void parsPROPERTYDEFINITIONS(ifstream* inFile, DEF_File* def, int* i) {
    string buffer;
    vector<string> split_buffer;
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
                 //   def->PRO.push_back(buffer);
        
        }
        
       
        def->beginning.push_back(buffer);
        

        split_buffer.clear();
    }


}
void parsVias(ifstream* inFile, DEF_File* def, int* i) {
    string buffer;
    vector<string> split_buffer;
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
        def->VIAS_str.push_back(buffer);

        split_buffer.clear();
    }

}
Position return_component_pos(DEF_File* def, string name) {
    for (int o = 0; o < def->COMPONENTS.size(); o++) {
        if (def->COMPONENTS[o].compName == name) {
            return  def->COMPONENTS[o].POS;
        }
    }
}

void swup_comp(DEF_File* def, string name1, string name2) {
    COMPONENTS_clas buf;
    int adr = 0;
    for (int o = 0; o < def->COMPONENTS.size(); o++) {
        if (def->COMPONENTS[o].compName == name1) {
            buf.compName = def->COMPONENTS[o].compName;
            buf.FIXED = def->COMPONENTS[o].FIXED;
            buf.modelName = def->COMPONENTS[o].modelName;
            buf.POS.x = def->COMPONENTS[o].POS.x;
            buf.POS.y = def->COMPONENTS[o].POS.y;
            buf.POS.orientation = def->COMPONENTS[o].POS.orientation;
            buf.SOURCE = def->COMPONENTS[o].SOURCE;
            adr = o;
        }
    }
    for (int o = 0; o < def->COMPONENTS.size(); o++) {
        if ((def->COMPONENTS[o].compName == name2) && (def->COMPONENTS[o].modelName == def->COMPONENTS[adr].modelName)) {
            def->COMPONENTS[adr].compName = def->COMPONENTS[o].compName;
            def->COMPONENTS[adr].FIXED = def->COMPONENTS[o].FIXED;
            def->COMPONENTS[adr].POS.x = def->COMPONENTS[o].POS.x;
            def->COMPONENTS[adr].POS.y = def->COMPONENTS[o].POS.y;
            def->COMPONENTS[adr].POS.orientation = def->COMPONENTS[o].POS.orientation;
            def->COMPONENTS[adr].SOURCE = def->COMPONENTS[o].SOURCE;

            def->COMPONENTS[o].compName = buf.compName;
            def->COMPONENTS[o].FIXED = buf.FIXED;
            def->COMPONENTS[o].modelName = buf.modelName;
            def->COMPONENTS[o].POS.x = buf.POS.x;
            def->COMPONENTS[o].POS.y = buf.POS.y;
            def->COMPONENTS[o].POS.orientation = buf.POS.orientation;
            def->COMPONENTS[o].SOURCE = buf.SOURCE;
            //cout << "Ver tipe" << endl;
        }
    }

}
void ReadDEF(DEF_File* def, string nameInFile) {
    string buffer;
    vector<string> split_buffer;
    string BUS_DIV;
    int flagPaternov = 0;
    ifstream inFile(nameInFile);
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
                def->version = stof(split_buffer[1]);
                flagPaternov = 1;
            }
            if (split_buffer.front() == "DIVIDERCHAR") {
                BUS_DIV = split_buffer[1];
                def->DIVIDERCHAR = BUS_DIV[1];
                flagPaternov = 1;
            }
            if (split_buffer.front() == "BUSBITCHARS") {
                BUS_DIV = split_buffer[1];
                def->BUSBITCHARS[0] = BUS_DIV[1];
                def->BUSBITCHARS[1] = BUS_DIV[2];

                flagPaternov = 1;
            }
            if (split_buffer.front() == "PROPERTYDEFINITIONS") {
                parsPROPERTYDEFINITIONS(&inFile, def, &i);
                flagPaternov = 1;
            }
            if (split_buffer.front() == "DESIGN") {
                def->DESIGN = split_buffer[1];
                flagPaternov = 1;
            }
            if (split_buffer.front() == "COMPONENTS") {
                def->COUNT_COMPONENTS = stoi(split_buffer[1]);
                parsComponents(&inFile, def, &i);

                flagPaternov = 1;
            }
            if (split_buffer.front() == "VIAS") {
                def->COUNT_VIAS = stoi(split_buffer[1]);
                parsVias(&inFile, def, &i);
                flagPaternov = 1;
            }
            if (split_buffer.front() == "PINS") {
                def->COUNT_PINS = stoi(split_buffer[1]);
                parsPins(&inFile, def, &i);
                flagPaternov = 1;
            }
            if (split_buffer.front() == "NETS") {
                def->COUNT_NETS = stoi(split_buffer[1]);
                parsNets(&inFile, def, &i);
                flagPaternov = 1;
            }
            if (split_buffer.front() == "SPECIALNETS") {
                def->COUNT_SPECIALNETS = stoi(split_buffer[1]);
                parsSpecialnets(&inFile, def, &i);
                flagPaternov = 1;
            }
            if (split_buffer.front() == "UNITS") {
                def->UNITS_DISTANCE_MICRONS = stoi(split_buffer[3]);
                flagPaternov = 1;
            }
            if (split_buffer.front() == "DIEAREA") {
                def->DIEAREA.x1 = stoi(split_buffer[2]);
                def->DIEAREA.y1 = stoi(split_buffer[3]);
                def->DIEAREA.x2 = stoi(split_buffer[6]);
                def->DIEAREA.y2 = stoi(split_buffer[7]);
                flagPaternov = 1;
            }

            string ifter;
            if (!buffer.empty()) {
                ifter = split_buffer[0];
            }

            if ((flagPaternov == 0) && (split_buffer[0] != "#")) {

                if (!buffer.empty()) {
                    def->beginning.push_back(buffer);
                }

                //cout <<"error" ;
            }



            // if (i>1000) break;
            split_buffer.clear();
            flagPaternov = 0;

        }
    }
    inFile.close();

}


#endif