#include "Verilog.hpp"

verilog::Port::Port() { 
    isPort = true; 
}

verilog::Module::~Module() {
    for (auto *net : nets)
        delete net;
    nets.clear();
    for (auto *port : ports)
        delete port;
    ports.clear();
    for (auto *instance : instances)
        delete instance;
    instances.clear();
}

verilog::Net* verilog::Module::getNetByName(const std::string &name) {
    for (Net *net : nets)
        if (net->name == name)
            return net;
    return nullptr;
}

verilog::Port* verilog::Module::getPortByName(const std::string &name) {
    for (Port *port : ports)
        if (port->name == name)
            return port;
    return nullptr;
}

verilog::Instance* verilog::Module::getInstanceByName(const std::string &name) {
    for (Instance *instance : instances)
        if (instance->name == name)
            return instance;
    return nullptr;
}

std::vector<verilog::Instance*> verilog::Module::getInstancesByType(const std::string &typeName) {
    std::vector<Instance *> lst;
    for (Instance* instance : instances)
        if (instance->instanceOf->name == name)
            lst.push_back(instance);
    return lst;
}

verilog::Netlist::~Netlist() {
    for (auto *module : library) {
        delete module;
    }
    library.clear();
}

verilog::Module * verilog::Netlist::getModuleByName(const std::string &name) {
    for (Module* module : library)
        if (module->name == name)
            return module;
    return nullptr;
}
