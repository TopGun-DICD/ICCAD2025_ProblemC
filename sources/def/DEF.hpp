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
    UNDEFINED = 0,
    NETLIST,
    DIST,
    USER,
    TIMING
};
enum class FIXED_class {
    UNDEFINED = 0,
    FIXED,
    COVER,
    PLACED,
    UNPLACED
};

class COMPONENTS_class {
public:
    std::string compName;
    std::string modelName;
    //std::string EQMASTER;
    Position        POS     = Position();
    SOURCE_class    SOURCE  = SOURCE_class::UNDEFINED;
    FIXED_class     FIXED   = FIXED_class::UNDEFINED;
    //int HALO;
    //std::string PROPERTY;
    // std::string REGION;
    //int ROUTEHALO;
    //std::string minLayer;
    //std::string maxLayer;
    //int WEIGHT;
    COMPONENTS_class(std::string _compName, std::string _modelName) : compName(_compName), modelName(_modelName) {}
    COMPONENTS_class() {}
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
    UNDEFINED, 
    INPUT,
    OUTPUT,
    INOUT,
    FEEDTHRU
};

enum class USE_class {
    UNDEFINED,
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
    int x = 0;
    int y = 0;
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
    DIRECTION_class DIRECTION  = DIRECTION_class::UNDEFINED;
    //NETEXPR
    //SUPPLYSENSITIVITY powerPinName
    //  GROUNDSENSITIVITY groundPinName
    USE_class USE = USE_class::UNDEFINED;
    std::vector<LAYER_class>  LAYER;
    VIA_class VIA;
    FIXED_class PLACED_PIN     = FIXED_class::UNDEFINED;
    Position POS;
    PINS_class(std::string _compName, std::string _modelName) : pinName(_compName), netName(_modelName) {}
};

class NETS_class {
public:
    std::string netName;
    std::vector<Rect_str> Net_unit;
    USE_class USE = USE_class::UNDEFINED;
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
    std::vector<std::string> PROPERTYDEFINITIONS; //раскоменчивание все ломает

    int UNITS_DISTANCE_MICRONS;
    Rect DIEAREA;
    std::vector<std::string> hair;// Шапка
    std::vector<std::string> beginning;// мусор в начале, не парсим

    int COUNT_COMPONENTS = 0;
    std::vector<COMPONENTS_class *> COMPONENTS;
    int numBlockages;
    std::vector<BLOCKAGES_class *>  BLOCKAGES;
    int COUNT_VIAS = 0;
    std::vector<std::string> VIAS_str;// читается без нормального парса
    int COUNT_PINS = 0;
    std::vector<PINS_class *>       PINS;
    int COUNT_NETS = 0;
    std::vector<NETS_class *>       NETS;
    int COUNT_SPECIALNETS = 0;
    std::vector<NETS_class *>       SPECIALNETS;

   ~DEF_File() {
       for (int i = 0; i < COMPONENTS.size(); ++i)
           delete COMPONENTS[i];
       COMPONENTS.clear();
       for (int i = 0; i < NETS.size(); ++i)
           delete NETS[i];
       NETS.clear();
       for (int i = 0; i < SPECIALNETS.size(); ++i)
           delete SPECIALNETS[i];
       SPECIALNETS.clear();
       for (int i = 0; i < PINS.size(); ++i)
           delete PINS[i];
       PINS.clear();
   }

    void push_back_COMPONENTS(std::string a, std::string b) {
        COMPONENTS_class *component = new COMPONENTS_class(a, b);
        COMPONENTS.push_back(component);
    }
    void push_back_NETS_rect_u(std::string a, std::string b) {
        Rect_str buffer(a, b);
        NETS.back()->Net_unit.push_back(buffer);
    }
    void push_back_SPECIALNETS_rect_u(std::string a, std::string b) {
        Rect_str buffer(a, b);
        SPECIALNETS.back()->Net_unit.push_back(buffer);
    }
    void push_back_PINS(std::string a, std::string b) {
        PINS_class *pin = new PINS_class(a, b);
        PINS.push_back(pin);
    }
    void push_back_NETS(std::string a) {
        NETS_class *net = new NETS_class(a);
        NETS.push_back(net);
    }
    void push_back_SPECIALNETS(std::string a) {
        NETS_class *specialNet = new NETS_class(a);
        SPECIALNETS.push_back(specialNet);
    }
    Position get_component_pos(std::string name) {
        for (int o = 0; o < COMPONENTS.size(); o++) {
            if (COMPONENTS[o]->compName == name) {
                return  COMPONENTS[o]->POS;
            }
        }
        return Position();
    }
    COMPONENTS_class* get_component(std::string name) {
        for (int o = 0; o < COMPONENTS.size(); o++) {
            if (COMPONENTS[o]->compName == name) {
                return  COMPONENTS[o];
            }
        }
        return nullptr;
    }

};

}

