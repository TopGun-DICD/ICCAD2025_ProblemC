#pragma once

#include <string>
#include <vector>

class CmdLine {
public:
    std::string                 verilog,
                                def;
    std::vector<std::string>    lefs,
                                libs;
    std::string                 outFile;
public:
    bool parse(int argc, char *argv[]);
private:
    void printVersionString();
    void printHelpString();
    bool findVerilog(const char *path);
    bool findDEF(const char *path);
    bool findLEFs(const char *path);
    bool findLibs(const char *path);
    bool findDesign(const char *path);
    bool findASAP7(const char *path);
};
