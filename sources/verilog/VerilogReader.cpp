#include "VerilogReader.hpp"

#include <iostream>
#include <unordered_set>
#include <ctime>

#define MAX_TOKEN_LENGTH    256

std::unordered_set<char> oneByteTokens_Verilog { '(', ')', '[', ']', '.', ',', '*', '/', '+', '-', '@', '`', '$', '\\', ';', ':', '#', '<' };

verilog::VerilogReader::~VerilogReader() {
    if (hdlCode) {
        delete [] hdlCode;
        hdlCode = nullptr;
    }
}

bool verilog::VerilogReader::read(const std::string &_fname, Netlist &_netlist, lef::LEFData &_lef) {
    if (!readHDLCode(_fname))
        return false;

    netlist = &_netlist;
    lef = &_lef;

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
                    if (!readModule())
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

    netlist->fileName = _fname;

    postProcess();

    return true;
}

void verilog::VerilogReader::postProcessAfterDEF() {
    for (Module *module : netlist->library)
        for (Instance *instance : module->instances)
            instance->recalcPlacementParameters();

}

bool verilog::VerilogReader::readHDLCode(const std::string &fname) {
    std::ifstream in(fname, std::ios::in | std::ios::binary);
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
    if (hdlCode[posInCode] == '\r') {
        ++posInCode;
        goto token_start;
    }
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
        if (oneByteTokens_Verilog.find(hdlCode[posInCode]) != oneByteTokens_Verilog.end()) {
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
            if (token[i - 1] == '\r')
                token[i - 1] = '\0';
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

bool verilog::VerilogReader::readModule() {
    char token[MAX_TOKEN_LENGTH];
    token[0] = '\0';

    readIdentifier(token);

    Module *module = new Module;
    netlist->library.push_back(module);
    module->name = token;

    readToken(token);
    if (token[0] != '(') {
        if (token[0] == ';')
            std::cerr << "  __wrn__ (" << line << ") : module with no ports was met: '" << module->name << "'" << std::endl;
        else {
            std::cerr << "  __err__ (" << line << "): unexpected token in module '" << module->name << "' definition. Abort." << std::endl;
            netlist->library.pop_back();
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
        if (!readModuleInstance(module, token))
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
        if (token[0] == ')')
            break;

        Port *port = new Port;
        //port->owner     = module;
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
    readToken(token);   // ;
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

bool verilog::VerilogReader::fillModulePortsInfoFromLEF(Module* _module) {
    lef::Macro *cell = lef->getMacroByName(_module->name);
    if (!cell) {
        //    if (_module->name != netlist->top->name) {
        std::cerr << "  __err__ : Couldn't find LEF MACRO '" << _module->name << "' (module found in netlist)! Abort!\n";
        return false;
        //    }
        //    continue;
    }
    for (verilog::Port* port : _module->ports) {
        if (port->direction != verilog::PortDirection::undefined)
            continue;
        lef::Pin* pin = cell->getPinByName(port->name);
        if (!pin) {
            std::cerr << "  __err__ : Couldn't find pin '" << port->name << "' int LEF MACRO '" << cell->name << "' Abort!\n";
            return false;
        }
        if (pin->direction == "INPUT") {
            port->direction = verilog::PortDirection::input;
            continue;
        }
        if (pin->direction == "OUTPUT") {
            port->direction = verilog::PortDirection::output;
            continue;
        }
        if (pin->direction == "INOUT") {
            port->direction = verilog::PortDirection::inout;
            continue;
        }
        std::cerr << "  __wrn__ : Unsupported MACRO PIN direction for cell '" << _module->name << "'. Ignore direction.\n";
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
        //net->owner  = module;
        net->name   = token;
        net->type   = type;
    }
    return true;
}

bool verilog::VerilogReader::readModuleInstance(Module *_module, const char *_moduleType) {
    char token[MAX_TOKEN_LENGTH] = { 0 };

    std::string type = _moduleType;
    readIdentifier(token);
    std::string name = token;

    Instance* instance = new Instance;
    _module->instances.push_back(instance);
    instance->name = name;

    std::vector<std::string>    pins,
                                wires;

    readToken(token);
    if (token[0] != '(') {
        std::cerr << "  __err__ (" << line << ") : syntax error in module instantiation, expected '(' but got '" 
                  << token <<"'. Abort." << std::endl;
        return false;
    }

    // Читаеем порты экземпляра модуля
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

    // Ищем модуль, типа которого наш компонент
    for (Module *module : netlist->library)
        if (module->name == _moduleType)
            instance->instanceOf = module;

    // Если родительский модуль не найден - создадим новый и подгрузим данные о тего пинах из LEF
    if(!instance->instanceOf) {
        Module *dummy = new Module;
        netlist->library.push_back(dummy);
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
        if (!fillModulePortsInfoFromLEF(dummy))
            return false;
    }
    ++instance->instanceOf->numberOfMyInstances;

    // Выясним, какие из пинов экземпляра являются входными, а какие - выходными. Сделаем это на основе направления портов 
    // родительского модуля
    for (size_t i = 0; i < wires.size(); ++i) {
        // Сначала найдём цепь по имени подключаемого пина
        size_t netIndex = 0;
        for (netIndex = 0; netIndex < _module->nets.size(); ++netIndex)
            if (_module->nets[netIndex]->name == wires[i])
                break;

        // Цепь нашлась
        if (netIndex < _module->nets.size())
            // Задано ли ассоциативное назначение портов?
            // Если да
            if (!pins.empty()) {
                //TODO: Необходимо проверить, что порт вообще нашёлся
                for (Port *port : instance->instanceOf->ports) {
                    if (port->name != pins[i])
                        continue;
                    switch (port->direction) {
                        case PortDirection::input:
                            instance->ins.push_back(_module->nets[netIndex]);
                            break;
                        case PortDirection::output:
                        case PortDirection::inout:
                            instance->outs.push_back(_module->nets[netIndex]);
                            break;
                        default:
                            ; //TODO: std::cerr
                    }
                }
            }
            // Если нет
            else {
                switch (instance->instanceOf->ports[i]->direction) {
                    case PortDirection::input:
                        instance->ins.push_back(_module->nets[netIndex]);
                        break;
                    case PortDirection::output:
                    case PortDirection::inout:
                        instance->outs.push_back(_module->nets[netIndex]);
                        break;
                    default:
                        ; //TODO: std::cerr
                }
            }

        // Цепь не нашлась. Может это порт?
        else {
            for (netIndex = 0; netIndex < _module->ports.size(); ++netIndex)
                if (_module->ports[netIndex]->name == wires[i])
                    break;
            if (netIndex == _module->ports.size()) {
                std::cerr << "  __err__ (" << line << ") : used net '" << wires[i] << "'wasn't found neither in nets nor ports of a module '" 
                          << _module->name << "'. Abort." << std::endl;
                return false;
            }
            if (!pins.empty()) {
                //TODO: Необходимо проверить, что порт вообще нашёлся
                for (Port *port : instance->instanceOf->ports) {
                    if (port->name != pins[i])
                        continue;
                    switch (port->direction) {
                        case PortDirection::input:
                            instance->ins.push_back(_module->ports[netIndex]);
                            break;
                        case PortDirection::output:
                        case PortDirection::inout:
                            instance->outs.push_back(_module->ports[netIndex]);
                            break;
                        default:
                            ; //TODO: std::cerr
                    }
                }
            }
            // Если нет
            else {
                switch (instance->instanceOf->ports[i]->direction) {
                case PortDirection::input:
                    instance->ins.push_back(_module->ports[netIndex]);
                    break;
                case PortDirection::output:
                case PortDirection::inout:
                    instance->outs.push_back(_module->ports[netIndex]);
                    break;
                default:
                    ; //TODO: std::cerr
                }
            }
        }
    }
    
    instance->placement.dx.resize(instance->ins.size(), 0);
    instance->placement.dy.resize(instance->ins.size(), 0);

    return true;
}

bool verilog::VerilogReader::findTopModule() {
    std::vector<Module *> unusedCells;

    for (Module *module : netlist->library)
        if (!module->numberOfMyInstances)
            unusedCells.push_back(module);

    if (unusedCells.size() == 1) {
        netlist->top = unusedCells[0];
        return true;
    }

    std::cerr << "  __wrn__ : There were found " << unusedCells.size() << " challenger cells to be TOP modules:" << std::endl;
    for (auto *module : unusedCells)
        std::cerr << "          * " << module->name << std::endl;
    std::cerr << "            I can't make a decision. Abort." << std::endl;
    return false;
}

bool verilog::VerilogReader::postProcess() {
    std::cout << "  Performing basic ckecks..." << std::endl;

    if (!findTopModule())
        return false;

    std::cout << "    Checking if any instance has no parent module...\n";
    for (Module *module : netlist->library) {
        
        for (Instance *instance : module->instances) {
            if (!instance->instanceOf) {
                std::cerr << "    __err__ : no base module found for instance " << instance->name
                          << " in module " << module->name;
                return false;
            }
            for (Net *net : instance->ins) {
                net->sourceFor.push_back(instance);
            }
            for (Net *net : instance->outs) {
                net->sourceFor.push_back(instance);
                if (net->driver) {
                    std::cerr << "  __wrn__ : net " << module->name << "." << net->name << "already has driver!\n"
                              << "            It's driver is '" << net->driver->name << ". Will be reassigned!\n";
                }
                net->driver = instance;
            }
        }
        if (module == netlist->top)
            continue;
        for (Port *port : module->ports) {
            if (port->direction == verilog::PortDirection::undefined) {
                std::cerr << "    __err__ : found port (" << module->name << "." << port->name << ") with undefined direction. Abort.\n";
                return false;
            }
        }
    }

    std::cout << "    Checking Port directions for modules...\n";
    for (Module *module : netlist->library)
        if (!checkModulePortDirections(module))
            return false;

    std::cout << "  Basic ckecks completed." << std::endl;
    return true;
}

bool verilog::VerilogReader::checkModulePortDirections(Module *_module) {
    std::vector<Port *> undirectedPorts;
    for (Port *port : _module->ports) {
        if (port->direction != PortDirection::undefined)
            continue;
        undirectedPorts.push_back(port);
    }

    if (undirectedPorts.empty())
        return true;

    for (Port *port : undirectedPorts) {
        bool portIsDirected = false;
        for (Instance *instance : _module->instances) {
            for (Net *pin : instance->ins)
                if (pin->name == port->name) {
                    port->direction = PortDirection::input;
                    portIsDirected = true;
                    break;
                }
            if (portIsDirected)
                break;
            for (Net *pin : instance->outs)
                if (pin->name == port->name) {
                    port->direction = PortDirection::output;
                    portIsDirected = true;
                    break;
                }
            if (portIsDirected)
                break;
        }
        if (!portIsDirected) {
            std::cerr << "    __err__ : Can't determine the direction if the port '" << port->name << "' in module '" 
                      << _module->name << "'\n";
            return false;
        }
    }
    return true;
}
