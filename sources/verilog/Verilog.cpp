#include "Verilog.hpp"

verilog::Port::Port() { 
    isPort = true; 
}

void verilog::Instance::recalcPlacementParameters() {
    placement.sum = 0;
    int sumX = 0, 
        sumY = 0;
    for (size_t i = 0; i < ins.size(); ++i) {
        if (!ins[i]->driver)
            continue;
        placement.dx[i] = placement.component->POS.x - ins[i]->driver->placement.component->POS.x;
        placement.dy[i] = placement.component->POS.y - ins[i]->driver->placement.component->POS.y;
        placement.sum += placement.dx[i] + placement.dy[i];
        sumX += ins[i]->driver->placement.component->POS.x;
        sumY += ins[i]->driver->placement.component->POS.y;
    }
    placement.sumNormalized = placement.sum / static_cast<int>(ins.size());
    placement.massCenter.x = sumX / static_cast<int>(ins.size());
    placement.massCenter.y = sumY / static_cast<int>(ins.size());

    double  max_radius = 0.0,
            radius = 0.0;
    for (size_t i = 0; i < outs.size(); ++i)
        for (size_t j = 0; j < outs[i]->sourceFor.size(); ++j) {
            double dx = static_cast<double>(outs[i]->sourceFor[j]->placement.component->POS.x - placement.component->POS.x);
            double dy = static_cast<double>(outs[i]->sourceFor[j]->placement.component->POS.y - placement.component->POS.y);
            radius = sqrt(dx*dx + dy*dy);
            if (radius > max_radius)
                max_radius = radius;
        }
    placement.radius = static_cast<uint32_t>(max_radius);
}

verilog::Module::~Module() {
    //for (auto *net : nets)
    //    delete net;
    //nets.clear();

    for (auto it = nets.begin(); it != nets.end(); it++)
        delete it->second;
    nets.clear();

    for (auto *port : ports)
        delete port;
    ports.clear();
    for (auto *instance : instances)
        delete instance;
    instances.clear();
}

verilog::Net* verilog::Module::getNetByName(const std::string &name) {
    //for (Net *net : nets)
    //    if (net->name == name)
    //        return net;
    auto it = nets.find(name);

    if (it != nets.end())
        return it->second;

    return nullptr;
}

verilog::Port* verilog::Module::getPortByName(const std::string &name) {
    for (Port *port : ports)
        if (port->name == name)
            return port;
    return nullptr;
}

verilog::Port* verilog::Module::getPortByDEFName(const std::string& name) {

    for (Port *port : ports) {
        std::string tempName = port->name; 
        
        size_t pos = tempName.find_first_of('\\');
        if (pos == std::string::npos)
            if (port->name == name)
                return port;

        while (pos != std::string::npos) {
            tempName.erase(pos, 1);
            pos = tempName.find_first_of('\\');
        }
        if (name == tempName)
            return port;
    }
    return nullptr;
}


verilog::Instance* verilog::Module::getInstanceByName(const std::string &name) {
    for (Instance *instance : instances)
        if (instance->name == name)
            return instance;
    return nullptr;
}

verilog::Instance *verilog::Module::getInstanceByDEFName(const std::string &name) {
    size_t pos = name.find_first_of('\\');
    if (pos == std::string::npos)
        return getInstanceByName(name);

    std::string tempName = name;

    while (pos != std::string::npos) {
        tempName.erase(pos, 1);
        pos = tempName.find_first_of('\\');
    }
    tempName = '\\' + tempName;

    return getInstanceByName(tempName);
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
