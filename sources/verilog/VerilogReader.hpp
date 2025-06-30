#pragma once

#include <string>
#include <fstream>

#include "Verilog.hpp"

class VerilogReader {
    char       *hdlCode     = nullptr;
    uint64_t    codeLength  = 0;
    uint64_t    posInCode   = 0;
    uint64_t    line        = 0;
public:
   ~VerilogReader();
public:
    bool read(const std::string &fname, Netlist &netlist);
private:
    bool readHDLCode(const std::string &fname);
    void readToken(char *token);
    void readIdentifier(char *token);
    void readToTheEOL();
    bool readModule(Netlist &netlist);
    bool readModulePorts(Module *module);
    bool readModulePortsOfDirection(Module *module, PortDirection dir);
    bool readModuleNetsOfType(Module *module, NetType type);
    bool readModuleInstance(Netlist &netlist, Module *module, const char *moduleType);
    bool performBasicChecks(Netlist &netlist);
    bool findTopModule(Netlist &netlist);
};
