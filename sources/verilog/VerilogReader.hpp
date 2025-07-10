#pragma once

#include <string>
#include <fstream>

#include "Verilog.hpp"
#include "../lef/LEF.hpp"

namespace verilog {

    class VerilogReader {
        char           *hdlCode     = nullptr;
        uint64_t        codeLength  = 0;
        uint64_t        posInCode   = 0;
        uint64_t        line        = 0;

        Netlist        *netlist     = nullptr;
        lef::LEFData   *lef         = nullptr;
    public:
        ~VerilogReader();
    public:
        bool read(const std::string &_fname, Netlist &_netlist, lef::LEFData &_lef);
    private:
        bool readHDLCode(const std::string &_fname);
        void readToken(char *_token);
        void readIdentifier(char *_token);
        void readToTheEOL();
        bool readModule();
        bool readModulePorts(Module *_module);
        bool readModulePortsOfDirection(Module *_module, PortDirection _dir);
        bool fillModulePortsInfoFromLEF(Module *_module);
        bool readModuleNetsOfType(Module *_module, NetType _type);
        bool readModuleInstance(Module *_module, const char *_moduleType);
        bool PostProcess();
        bool findTopModule();
        bool checkPortDirections(Module *_module);
    };

}