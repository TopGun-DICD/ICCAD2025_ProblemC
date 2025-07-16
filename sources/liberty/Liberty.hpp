#pragma once

#include <string>
#include <vector>

namespace liberty {

    enum class PinDirection {
        undefined = 0,
        input,
        output,
    };

    struct Pin {
        std::string         name;
        PinDirection        direction = PinDirection::undefined;
        double              capacitance;
    };

    struct Cell {
        std::string         name;
        double              area;
        std::vector<Pin *>  ins,
                            outs;
    public:
       ~Cell();
    };

    struct Library {
        std::string         name;
        std::vector<Cell *> cells;
    public:
       ~Library();
    };

    struct Liberty {
        std::string             fileName;
        std::vector<Library *>  libraries;
    public:
       ~Liberty();
    };

}
