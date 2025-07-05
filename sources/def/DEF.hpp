#pragma once

#include <vector>
#include <string>
// Ориентация компонента

namespace def {

enum class Orientation {
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
    std::string first;
    std::string second;
    Rect_str(std::string _first, std::string _second)
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
    enum Orientation orientation = Orientation::N; // Ориентация

    Position(int x = 0, int y = 0, Orientation orient = Orientation::N)
        : x(x), y(y), orientation(orient) {}
};

enum class SOURCE_class {
    NETLIST = 0,
    DIST = 1,
    USER = 2,
    TIMING = 3
};
enum class FIXED_class {
    FIXED = 0,
    COVER = 1,
    PLACED = 2,
    UNPLACED = 3
};

class COMPONENTS_clas {
public:
    std::string compName;
    std::string modelName;
    //std::string EQMASTER;
    Position POS;
    SOURCE_class SOURCE;
    FIXED_class FIXED;
    //int HALO;
    //std::string PROPERTY;
    // std::string REGION;
    //int ROUTEHALO;
    //std::string minLayer;
    //std::string maxLayer;
    //int WEIGHT;
    COMPONENTS_clas(std::string _compName, std::string _modelName) : compName(_compName), modelName(_modelName) {}
    COMPONENTS_clas() {}
};

class BLOCKAGES_class {
    std::string layerName;
    //Вернусь позже стр 242
    std::vector<Rect> RECT;
    int SPACING = 0;
    int DESIGNRULEWIDTH = 0;
    std::string COMPONENT_compName;
    std::string FILLS;
    std::string SLOTS;
    std::string PUSHDOWN;
    std::string EXCEPTPGNET;
    std::vector<Rect> POLYGON;
    std::string SOFT;///Вот здесь надо подумать над перечислениями с 243
    std::string PARTIAL;
    std::string COMPONENT;
};

class VIAS_class {
};

enum class DIRECTION_class {
    INPUT,
    OUTPUT,
    INOUT,
    FEEDTHRU
};

enum class USE_class {
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
    std::string viaName;
    int x;
    int y;
};

class LAYER_class {
public:
    std::string layerName;
    Rect rect;
};

class PINS_class {
public:
    std::string pinName;
    std::string netName;
    int SPECIAL = 0;
    enum DIRECTION_class DIRECTION;
    //NETEXPR
    //SUPPLYSENSITIVITY powerPinName
    //  GROUNDSENSITIVITY groundPinName
    enum USE_class USE;
    std::vector<LAYER_class>  LAYER;
    VIA_class VIA;
    enum FIXED_class PLACED_PIN;
    Position POS;
    PINS_class(std::string _compName, std::string _modelName) : pinName(_compName), netName(_modelName) {}
};

class NETS_class {
public:
    std::string netName;
    std::vector<Rect_str> Net_unit;
    USE_class USE;
    NETS_class(std::string netName) : netName(netName) {}
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
    std::string DESIGN;
   // std::vector<std::string> PRO; //раскоменчивание все ломает

    int UNITS_DISTANCE_MICRONS;
    Rect DIEAREA;
    std::vector<std::string> beginning;// мусор в начале, не парсим

    int COUNT_COMPONENTS = 0;
    std::vector<COMPONENTS_clas> COMPONENTS;
    int numBlockages;
    std::vector<BLOCKAGES_class> BLOCKAGES;
    int COUNT_VIAS = 0;
    std::vector<std::string> VIAS_str;// читается без нормального парса
    int COUNT_PINS = 0;
    std::vector<PINS_class> PINS;
    int COUNT_NETS = 0;
    std::vector<NETS_class> NETS;
    int COUNT_SPECIALNETS = 0;
    std::vector<NETS_class> SPECIALNETS;

    void push_back_COMPONENTS(std::string a, std::string b) {
        COMPONENTS_clas buffer_components(a, b);
        COMPONENTS.push_back(buffer_components);
    }
    void push_back_NETS_rect_u(std::string a, std::string b) {
        Rect_str buffer(a, b);
        NETS[NETS.size() - 1].Net_unit.push_back(buffer);
    }
    void push_back_SPECIALNETS_rect_u(std::string a, std::string b) {
        Rect_str buffer(a, b);
        SPECIALNETS[SPECIALNETS.size() - 1].Net_unit.push_back(buffer);
    }
    void push_back_PINS(std::string a, std::string b) {
        PINS_class buffer_pins(a, b);
        PINS.push_back(buffer_pins);
    }
    void push_back_NETS(std::string a) {
        NETS_class buffer_nets(a);
        NETS.push_back(buffer_nets);
    }
    void push_back_SPECIALNETS(std::string a) {
        NETS_class buffer_nets(a);
        SPECIALNETS.push_back(buffer_nets);
    }
    Position return_component_pos(std::string name) {
        for (int o = 0; o < COMPONENTS.size(); o++) {
            if (COMPONENTS[o].compName == name) {
                return  COMPONENTS[o].POS;
            }
        }
    }

};

}

