#pragma once

#include "Liberty.hpp"

#include "../verilog/Verilog.hpp"

namespace liberty {

    class LibertyReader {
        char               *code        = nullptr;
        uint64_t            codeLength  = 0;
        uint64_t            posInCode   = 0;
        uint64_t            line        = 0;

        Liberty            *liberty     = nullptr;
        verilog::Netlist   *netlist     = nullptr;
    public:
       ~LibertyReader();
    public:
        bool read(const std::string &_fname, Liberty &_liberty, verilog::Netlist &_netlist);
    private:
        bool readCode(const std::string &_fname);
        void readToken(char *_token);
        void readIdentifier(char *_token);
        void readToTheEOL();
        void readUntil(char symbol);
        void readUntil2(char symbol1, char symbol2);

        bool readLibrary();
        bool readCell(Library *_library);
        bool readPin(Cell *_cell);

        bool postProcess();
    };

}
