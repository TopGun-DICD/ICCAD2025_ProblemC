#include "Liberty.hpp"

liberty::Cell::~Cell() {
    for (size_t i = 0; i < ins.size(); ++i)
        delete ins[i];
    ins.clear();
    for (size_t i = 0; i < outs.size(); ++i)
        delete outs[i];
    outs.clear();
}

liberty::Library::~Library() {
    for (size_t i = 0; i < cells.size(); ++i)
        delete cells[i];
    cells.clear();
}

liberty::Liberty::~Liberty() {
    for (size_t i = 0; i < libraries.size(); ++i)
        delete libraries[i];
    libraries.clear();
}

