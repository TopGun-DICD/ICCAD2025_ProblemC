#include "VerilogReader.hpp"

#include <iostream>
#include <unordered_set>
#include <ctime>

#define MAX_TOKEN_LENGTH    256

std::unordered_set<char> oneByteTokens{ '(', ')', '[', ']', '.', ',', '*', '/', '+', '-', '@', '`', '$', '\\', ';', ':', '#', '<' };

verilog::VerilogReader::~VerilogReader() {
    if (hdlCode) {
        delete [] hdlCode;
        hdlCode = nullptr;
    }
}

bool verilog::VerilogReader::read(const std::string &fname, Netlist &netlist) {
    if (!readHDLCode(fname))
        return false;

    char token[MAX_TOKEN_LENGTH];
    while (posInCode < codeLength) {
        readToken(token);
        if (token[0] == '\0')
            continue;
        switch (token[0]) {
            case '`':
                std::cerr << "  __err__ : unsupported statement '" << token << "'. Abort." << std::endl;
                return false;
            case 'm':
                if (!strcmp(token, "module")) {
                    if (!readModule(netlist))
                        return false;
                    break;
                }
                std::cerr << "  __err__ : unsupported token '" << token << "'. Abort." << std::endl;
                return false;
            default:
                std::cerr << "  __err__ : unsupported token '" << token << "'. Abort." << std::endl;
                return false;
        }
    }

    netlist.fileName = fname;

    performBasicChecks(netlist);
    findTopModule(netlist);

    return true;
}

bool verilog::VerilogReader::readHDLCode(const std::string &fname) {
    std::ifstream in(fname, std::ios::in);
    if (!in.is_open())
        return false;

    in.seekg(0, std::ios::end);
    codeLength = in.tellg();
    in.seekg(0, std::ios::beg);
    hdlCode = new char[codeLength + 1];
    in.read(hdlCode, codeLength);
    in.close();
    hdlCode[codeLength] = '\0';

    posInCode   = 0;
    line        = 1;
    return true;
}

void verilog::VerilogReader::readToken(char *token) {
    token[0] = '\0';
    int i = 0;
token_start:
    if (posInCode >= codeLength)
        return;
    while (hdlCode[posInCode] == ' ' || hdlCode[posInCode] == '\t')
        ++posInCode;
    if (hdlCode[posInCode] == '\n') {
        ++posInCode;
        ++line;
        goto token_start;
    }

    while (posInCode < codeLength) {
        if (hdlCode[posInCode] == '/' && hdlCode[posInCode + 1] == '/') {
            readToTheEOL();
            goto token_start;
        }
        if (hdlCode[posInCode] == '/' && hdlCode[posInCode + 1] == '*') {
            while (posInCode < codeLength) {
                if (hdlCode[posInCode] == '\n')
                    ++line;
                if (hdlCode[posInCode] == '*' && hdlCode[posInCode + 1] == '/') {
                    posInCode += 2;
                    break;
                }
                ++posInCode;
            }
            goto token_start;
        }
        if (hdlCode[posInCode] == '(' && hdlCode[posInCode + 1] == '*') {
            while (posInCode < codeLength) {
                if (hdlCode[posInCode] == '*' && hdlCode[posInCode + 1] == ')') {
                    posInCode += 2;
                    break;
                }
                ++posInCode; 
            }
            goto token_start;
        }
        if (oneByteTokens.find(hdlCode[posInCode]) != oneByteTokens.end()) {
            if (!i) {
                token[i] = hdlCode[posInCode];
                token[i + 1] = '\0';
                ++posInCode;
                return;
            }
            token[i] = '\0';
            return;
        }
        if (hdlCode[posInCode] == ' ' || hdlCode[posInCode] == '\t') {
            ++posInCode;
            token[i] = '\0';
            return;
        }
        if (hdlCode[posInCode] == '\n') {
            ++posInCode;
            ++line;
            token[i] = '\0';
            return;
        }
        token[i++] = hdlCode[posInCode++];
    }
}

void verilog::VerilogReader::readIdentifier(char *token) {
    readToken(token);
    if (token[0] != '\\')
        return;
    int j = 1;
    while (hdlCode[posInCode] != ' ') {
        token[j] = hdlCode[posInCode];
        ++j;
        ++posInCode;
    }
    token[j] = '\0';
}

void verilog::VerilogReader::readToTheEOL() {
    while (hdlCode[posInCode] != '\n' && posInCode < codeLength)
        ++posInCode;
    ++posInCode;
    ++line;
}

bool verilog::VerilogReader::readModule(Netlist &netlist) {
    char token[MAX_TOKEN_LENGTH];
    token[0] = '\0';

    readIdentifier(token);

    Module *module = new Module;
    netlist.library.push_back(module);
    module->name = token;

    readToken(token);
    if (token[0] != '(') {
        if (token[0] == ';')
            std::cerr << "  __wrn__ (" << line << ") : module with no ports was met: '" << module->name << "'" << std::endl;
        else {
            std::cerr << "  __err__ (" << line << "): unexpected token in module '" << module->name << "' definition. Abort." << std::endl;
            netlist.library.pop_back();
            delete module;
            return false;
        }
    }
    readModulePorts(module);
    while (posInCode < codeLength) {
        readIdentifier(token);
        if (!strcmp(token, "endmodule"))
            break;
        if (!strcmp(token, "input"))    { if (!readModulePortsOfDirection(module, PortDirection::input))    return false; continue; }
        if (!strcmp(token, "output"))   { if (!readModulePortsOfDirection(module, PortDirection::output))   return false; continue; }
        if (!strcmp(token, "inout"))    { if (!readModulePortsOfDirection(module, PortDirection::inout))    return false; continue; }
        if (!strcmp(token, "wire"))     { if (!readModuleNetsOfType(module, NetType::wire)) return false;   continue; }
        if (!strcmp(token, "reg"))      { if (!readModuleNetsOfType(module, NetType::reg)) return false;    continue; }
        if (!readModuleInstance(netlist, module, token))
            return false;
    }
    return true;
}

bool verilog::VerilogReader::readModulePorts(Module *module) {
    char token[MAX_TOKEN_LENGTH] = { 0 };

    
    while (strcmp(token, ")") && posInCode < codeLength) {
        readIdentifier(token);
        if (token[0] == ',')
            continue;

        Port *port = new Port;
        port->owner     = module;
        module->ports.push_back(port);

        if (!strcmp(token, "input"))    { port->direction = PortDirection::input;   readIdentifier(token); }
        if (!strcmp(token, "output"))   { port->direction = PortDirection::output;  readIdentifier(token); }
        if (!strcmp(token, "inout"))    { port->direction = PortDirection::inout;   readIdentifier(token); }
        if (!strcmp(token, "wire"))     { port->type = NetType::wire;   readIdentifier(token); }
        if (!strcmp(token, "reg"))      { port->type = NetType::reg;    readIdentifier(token); }
        port->name = token;
    }
    if (posInCode >= codeLength)
        return false;
    readToken(token);
    return true;
}

bool verilog::VerilogReader::readModulePortsOfDirection(Module *module, PortDirection dir) {
    char token[MAX_TOKEN_LENGTH] = { 0 };

    while (true) {
        readIdentifier(token);
        if (token[0] == ',')
            continue;
        if (token[0] == ';')
            break;
        size_t index = 0;
        for (; index < module->ports.size(); ++index) {
            if (module->ports[index]->name == token)
                break;
        }
        if (index == module->ports.size()) {
            std::cerr << "  __err__ (" << line << "): in module '" << module->name << "' direction given for an undeclared port '" 
                      << token << "'. Abort." << std::endl;
            return false;
        }
        if (module->ports[index]->direction != PortDirection::undefined)
            std::cerr << "  __wrn__ (" << line << "): in module '" << module->name << "' port '" << token << "' already has got its direction" 
                      << std::endl;
        module->ports[index]->direction = dir;
    }
    return true;
}

bool verilog::VerilogReader::readModuleNetsOfType(Module *module, NetType type) {
    char token[MAX_TOKEN_LENGTH] = { 0 };

    while (true) {
        readIdentifier(token);
        if (token[0] == ',')
            continue;
        if (token[0] == ';')
            break;
        size_t index = 0;
        for (index = 0; index < module->ports.size(); ++index) {
            if (module->ports[index]->name == token)
                break;
        }
        if (index != module->ports.size()) {
            if (module->ports[index]->type != NetType::undefined)
                std::cerr << "  __wrn__ (" << line << "): in module '" << module->name << "' port '" << token << "' already has git its type"
                          << std::endl;
            module->ports[index]->type = type;
            continue;
        }

        if (!module->nets.empty()) {
            for (index = 0; index < module->nets.size(); ++index) {
                if (module->nets[index]->name == token)
                    break;
            }
            if (index != module->nets.size()) {
                if (module->nets[index]->type != NetType::undefined)
                    std::cerr << "  __wrn__ (" << line << "): in module '" << module->name << "' net '" << token << "' already has git its type"
                    << std::endl;
                module->nets[index]->type = type;
                continue;
            }
        }
        Net *net    = new Net;
        module->nets.push_back(net);
        net->owner  = module;
        net->name   = token;
        net->type   = type;
    }
    return true;
}

bool verilog::VerilogReader::readModuleInstance(Netlist &netlist, Module *module, const char *moduleType) {
    char token[MAX_TOKEN_LENGTH] = { 0 };

    std::string type = moduleType;
    readIdentifier(token);
    std::string name = token;

    Instance* instance = new Instance;
    module->instances.push_back(instance);
    instance->name = name;

    std::vector<std::string>    pins,
                                wires;

    readToken(token);
    if (token[0] != '(') {
        std::cerr << "  __err__ (" << line << ") : syntax error in module instantiation, expected '(' but got '" 
                  << token <<"'. Abort." << std::endl;
        return false;
    }

    while (true) {
        readIdentifier(token);
        if (token[0] == ',')
            continue;
        if (token[0] == ')')
            break;

        if (token[0] == '.') {              // (.Y(n12), .A(n18))
            readIdentifier(token);
            pins.push_back(token);
            readToken(token);
            readIdentifier(token);
            wires.push_back(token);
            readToken(token);
        }
        else {                              // (n12, n18)
            wires.push_back(token);
        }
    }
    readToken(token);

    for (size_t i = 0; i < netlist.library.size(); ++i)
        if (netlist.library[i]->name == moduleType)
            instance->instanceOf = netlist.library[i];

    if(!instance->instanceOf) {
        Module *dummy = new Module;
        netlist.library.push_back(dummy);
        dummy->name = type;

        instance->instanceOf = dummy;
        if (pins.empty()) 
            for (size_t i = 0; i < wires.size(); ++i) {
                Port *port = new Port;
                port->name = "$_" + std::to_string(i);
                dummy->ports.push_back(port);
            }
        else
            for (size_t i = 0; i < wires.size(); ++i) {
                Port *port = new Port;
                port->name = pins[i];
                dummy->ports.push_back(port);
            }
    }
    ++instance->instanceOf->numberOfMyInstances;

    for (size_t i = 0; i < wires.size(); ++i) {
        size_t netIndex = 0;
        for (netIndex = 0; netIndex < module->nets.size(); ++netIndex)
            if (module->nets[netIndex]->name == wires[i])
                break;

        if (netIndex < module->nets.size())
            instance->pins.push_back(module->nets[netIndex]);
        else {
            for (netIndex = 0; netIndex < module->ports.size(); ++netIndex)
                if (module->ports[netIndex]->name == wires[i])
                    break;
            if (netIndex == module->ports.size()) {
                std::cerr << "  __err__ (" << line << ") : used net '" << wires[i] << "'wasn't found neither in nets nor ports of a module '" << module->name
                    << "'. Abort." << std::endl;
                return false;
            }
            instance->pins.push_back(module->ports[netIndex]);
        }
        
    }

    return true;
}

bool verilog::VerilogReader::performBasicChecks(Netlist &netlist) {
    std::cout << "  Performing basic ckecks..." << std::endl;
    for (int i = 0; i < netlist.library.size(); ++i) {
        for(int j = 0; j < netlist.library[i]->instances.size(); ++j)
            if (!netlist.library[i]->instances[j]->instanceOf)
                std::cerr << "    __err__ : no base module found for instance " << netlist.library[i]->instances[j]->name 
                          << " in module " << netlist.library[i]->name;
    }
    std::cout << "  Basic ckecks completed." << std::endl;
    return true;
}

bool verilog::VerilogReader::findTopModule(Netlist &netlist) {
    std::vector<Module *> unusedCells;
    for (size_t i = 0; i < netlist.library.size(); ++i)
        if (!netlist.library[i]->numberOfMyInstances)
            unusedCells.push_back(netlist.library[i]);

    if (unusedCells.size() == 1) {
        netlist.top = unusedCells[0];
        return true;
    }

    std::cerr << "  __wrn__ : There were found " << unusedCells.size() << " challenger cells to be TOP modules:" << std::endl;
    for (auto *module : unusedCells)
        std::cerr << "          * " << module->name << std::endl;
    std::cerr << "            I can't make a decision. Abort." << std::endl;
    return false;
}
