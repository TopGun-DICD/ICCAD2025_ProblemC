#pragma once

#include <string>
#include <vector>

#include "../def/DEF.hpp"
#include "../liberty/Liberty.hpp"

namespace verilog {

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
    struct Instance;

    struct Net {
        Instance                   *driver = nullptr;
        std::vector<Instance *>     sourceFor;
        bool                        isPort = false;
        std::string                 name;
        NetType                     type = NetType::undefined;
    };

    struct Port : public Net {
        PortDirection   direction = PortDirection::undefined;
    public:
        Port();
    };

    struct Instance {
        Module                     *instanceOf = nullptr;
        std::string                 name;
        std::vector<Net *>          ins,
                                    outs,
                                    unknown;
        std::vector<liberty::Pin *> libertyIns,
                                    libertyOuts;
        struct {
            def::COMPONENTS_class  *component  = nullptr;
            std::vector<int>        dx,                 // �� ������ - �� ����� ������ - ���������� �� ����������� ���������� �� X
                                    dy;                 // �� ������ - �� ����� ������ - ���������� �� ����������� ���������� �� Y
            int                     sum = 0,            // �� ������ - ��������� ���������� dx + dy �� ���� ������
                                    sumNormalized = 0,  // �� ������ - ��������������� �� ����� ������ ��������� ���������� �� ���� ������
                                    radius = 0;         // �� ������� - ������ - ���������� �������� �������� ���������� �� ������
            def::Position           massCenter;         // �� ������ - "����� ����" ������� ��������� ������� ������
        } placement;
        liberty::Cell              *libertyCell = nullptr;


    public:
        void recalcPlacementParameters();
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
        Net* getNetByName(const std::string &name);
        Port* getPortByName(const std::string &name);
        Instance* getInstanceByName(const std::string &name);
        Instance* getInstanceByDEFName(const std::string &name);
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
        Module* getModuleByName(const std::string&);
    };

}
