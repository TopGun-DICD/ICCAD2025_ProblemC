#ifndef LEF_HPP
#define LEF_HPP

#include <string>
#include <vector>

struct Rect {
    double x1, y1, x2, y2;

    Rect(double _x1 = 0, double _y1 = 0, double _x2 = 0, double _y2 = 0)
        : x1(_x1), y1(_y1), x2(_x2), y2(_y2) {}
};

struct Port_lef {
    std::string          layer;
    std::vector<Rect>    rects;
};

struct Pin {
    Port_lef            port;
    std::string     name;
    std::string     direction;
    std::string     use;
    std::string     shape;
};

struct Obs {
    std::vector<std::pair<std::string, std::vector<Rect>>> geometries;
};

struct Site {
    std::string     name;
    std::string     symmetry;
    std::string     className;
    double          sizeX, sizeY;

public:
    ~Site();
};

struct Macro {
    std::string         name;
    std::string         className;
    std::string         foreignName;
    double              originX, originY;
    double              sizeX = 0.000, sizeY;
    std::string         symmetry;
    std::string         site;
    std::vector<Pin>    pins;
    Obs obs;

public:
    ~Macro();
public:
    Pin* getPinByName(const std::string&);
};
#endif