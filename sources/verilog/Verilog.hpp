#pragma once

#include <string>
#include <vector>

enum class NetType {
    undefined = 0,
    wire,
    reg
};

enum class PortDirection {
    undefined = 0,
    input,
    output,
    inout
};

struct Module;

struct Net {
    Module         *owner       = nullptr;
    bool            isPort      = false;
    std::string     name;
    NetType         type        = NetType::undefined;
};

struct Port : public Net {
    PortDirection   direction   = PortDirection::undefined;
public:
    Port();
};


struct Instance {
    Module                 *instanceOf  = nullptr;
    std::string             name;
    std::vector<Net *>      pins;
};

struct Module {
    std::string             name;
    std::vector<Net *>      nets;
    std::vector<Port *>     ports;
    std::vector<Instance *> instances;
    uint32_t                numberOfMyInstances = 0;
public:
    ~Module();
public:
    Net*        getNetByName(const std::string &name);
    Port*       getPortByName(const std::string &name);
    Instance*   getInstanceByName(const std::string &name);
    std::vector<Instance *>
                getInstancesByType(const std::string &typeName);
};

struct Netlist {
    std::string             fileName;
    Module                 *top = nullptr;
    std::vector<Module *>   library;
public:
    ~Netlist();
public:
    Module* getModuleByName(const std::string &);
};