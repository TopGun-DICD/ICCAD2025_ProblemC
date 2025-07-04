#pragma once

//#include <boost/algorithm/string.hpp>
#include <iostream>
#include <vector>
#include <string>
#include <map>
#include <utility>
#include <fstream>
#include <map>
#include <sstream>
#include <algorithm>
#include <memory>
#include <type_traits>
#include <locale>
using namespace std;
// Ориентация компонента


enum Orientation {
    N,    // 0°
    S,    // 180°
    E,    // 90°
    W,    // 270°
    FN,   // Flipped N (mirror Y)
    FS,   // Flipped S (mirror Y)
    FE,   // Flipped E (mirror X)
    FW    // Flipped W (mirror X)
};
struct Rect_str {
    string first;
    string second;
    Rect_str(string _first, string _second)
        : first(_first), second(_second) {}
};

struct Rect {
    double x1, y1, x2, y2;

    Rect(double _x1 = 0, double _y1 = 0, double _x2 = 0, double _y2 = 0)
        : x1(_x1), y1(_y1), x2(_x2), y2(_y2) {}

};

// Позиция на плоскости
struct Position {
    int x;          // Координата X
    int y;          // Координата Y
    enum Orientation orientation; // Ориентация

    Position(int x = 0, int y = 0, Orientation orient = Orientation::N)
        : x(x), y(y), orientation(orient) {}
};

enum SOURCE_class {
    NETLIST = 0,
    DIST = 1,
    USER = 2,
    TIMING = 3
};
enum FIXED_class {
    FIXED = 0,
    COVER = 1,
    PLACED = 2,
    UNPLACED = 3
};
class COMPONENTS_clas {
public:
    string compName;
    string modelName;
    //string EQMASTER;
    Position POS;
    enum SOURCE_class SOURCE;
    enum FIXED_class FIXED;
    //int HALO;
    //string PROPERTY;
    // string REGION;
    //int ROUTEHALO;
    //string minLayer;
    //string maxLayer;
    //int WEIGHT;
    COMPONENTS_clas(string _compName, string _modelName) : compName(_compName), modelName(_modelName) {}
    COMPONENTS_clas() {}
};
class BLOCKAGES_class {
    string layerName;
    //Вернусь позже стр 242
    vector<Rect> RECT;
    int SPACING = 0;
    int DESIGNRULEWIDTH = 0;
    string COMPONENT_compName;
    string FILLS;
    string SLOTS;
    string PUSHDOWN;
    string EXCEPTPGNET;
    vector<Rect> POLYGON;
    string SOFT;///Вот здесь надо подумать над перечислениями с 243
    string PARTIAL;
    string COMPONENT;
};
class VIAS_class {


};
enum DIRECTION_class {
    INPUT,
    OUTPUT,
    INOUT,
    FEEDTHRU
};
enum USE_class {
    SIGNAL,
    POWER,
    GROUND,
    CLOCK,
    TIEOFF,
    ANALOG,
    SCAN,
    RESET
};
class VIA_class {
public:
    string viaName;
    int x;
    int y;
};
class LAYER_class {
public:
    string layerName;
    Rect rect;
};

class PINS_class {
public:
    string pinName;
    string netName;
    int SPECIAL = 0;
    enum DIRECTION_class DIRECTION;
    //NETEXPR
    //SUPPLYSENSITIVITY powerPinName
    //  GROUNDSENSITIVITY groundPinName
    enum USE_class USE;
    vector<LAYER_class>  LAYER;
    VIA_class VIA;
    enum FIXED_class PLACED_PIN;
    Position POS;
    PINS_class(string _compName, string _modelName) : pinName(_compName), netName(_modelName) {}
};
class NETS_class {
public:
    string netName;
    vector<Rect_str> Net_unit;
    USE_class USE;
    NETS_class(string netName) : netName(netName) {}
};
class SPECIALNETS_class {


};
  //O
class DEF_File {
public:
    int Flag_per_tz = 0;
    char BUSBITCHARS[2]; // Разделяющие символы для шинных битов(нужно уточнить их 2 или 3) gпо умолчанию []
    float version;
    char DIVIDERCHAR = '/';  //символ разделитель по умолчанию /
    string DESIGN;
   // vector<string> PRO; //раскоменчивание все ломает

    int UNITS_DISTANCE_MICRONS;
    Rect DIEAREA;
    vector<string> beginning;// мусор в начале, не парсим

    int COUNT_COMPONENTS = 0;
    vector<COMPONENTS_clas> COMPONENTS;
    int numBlockages;
    vector<BLOCKAGES_class> BLOCKAGES;
    int COUNT_VIAS = 0;
    vector<string> VIAS_str;// читается без нормального парса
    int COUNT_PINS = 0;
    vector<PINS_class> PINS;
    int COUNT_NETS = 0;
    vector<NETS_class> NETS;
    int COUNT_SPECIALNETS = 0;
    vector<NETS_class> SPECIALNETS;

    void push_back_COMPONENTS(string a, string b) {
        COMPONENTS_clas buffer_components(a, b);
        COMPONENTS.push_back(buffer_components);
    }
    void push_back_NETS_rect_u(string a, string b) {
        Rect_str buffer(a, b);
        NETS[NETS.size() - 1].Net_unit.push_back(buffer);
    }
    void push_back_SPECIALNETS_rect_u(string a, string b) {
        Rect_str buffer(a, b);
        SPECIALNETS[SPECIALNETS.size() - 1].Net_unit.push_back(buffer);
    }
    void push_back_PINS(string a, string b) {
        PINS_class buffer_pins(a, b);
        PINS.push_back(buffer_pins);
    }
    void push_back_NETS(string a) {
        NETS_class buffer_nets(a);
        NETS.push_back(buffer_nets);
    }
    void push_back_SPECIALNETS(string a) {
        NETS_class buffer_nets(a);
        SPECIALNETS.push_back(buffer_nets);
    }
};
void parsPROPERTYDEFINITIONS(ifstream* inFile, DEF_File* def, int* i);
void parsComponents(ifstream* inFile, DEF_File* def, int* i);
void parsNets(ifstream* inFile, DEF_File* def, int* i);
void parsSpecialnets(ifstream* inFile, DEF_File* def, int* i);
void parsPins(ifstream* inFile, DEF_File* def, int* i);
void parsVias(ifstream* inFile, DEF_File* def, int* i);
Position return_component_pos(DEF_File* def, string name);
void swup_comp(DEF_File* def, string name1, string name2);
void ReadDEF(DEF_File* def, string nameInFile);
