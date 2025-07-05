#ifndef LEF_HPP
#define LEF_HPP

#include <string>
#include <vector>

namespace lef {

struct Rect {
    double x1, y1, x2, y2;

    Rect(double _x1 = 0, double _y1 = 0, double _x2 = 0, double _y2 = 0)
        : x1(_x1), y1(_y1), x2(_x2), y2(_y2) {}
};

struct Port {
    std::string          layer;
    std::vector<Rect>    rects;
};

struct Pin {
    Port            port;
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
    double          sizeX,
                    sizeY;

public:
    ~Site();
};

struct Macro {
    std::string         name;
    std::string         className;
    std::string         foreignName;
    double              originX = 0.0, 
                        originY = 0.0;
    double              sizeX = 0.0, 
                        sizeY = 0.0;
    std::string         symmetry;
    std::string         site;
    std::vector<Pin *>  pins;
    Obs                 obs;

public:
    ~Macro();
public:
    Pin* getPinByName(const std::string&);
};

class LEFData {
    std::vector<Macro*> macroes;
    std::vector<Site*>  sites;
public:
   ~LEFData();
public:
    void addMacro(Macro *_macro);
    void addSite(Site *_site);
    Macro* getMacroByName(const std::string &_name);
    Site* getSiteByName(const std::string &_name);
};

}
#endif