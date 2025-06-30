#include "cmdline.hpp"

#include <filesystem>
#include <iostream>

#define PROGRAM_VERSION "0.0.2"
#if defined _WIN32
#   define DELIMETER "\\"
#else
#   define DELIMETER "/"
#endif

bool CmdLine::parse(int argc, char *argv[]) {
    if (1 == argc) {
        std::cout << "ICCAD 2025 TaskC solver [cadc1079 team]\n\n";
        std::cout << "To learn about available options please run:\n  cadc1079 --help\n\n";
        return false;
    }

    // Look for '--version'
    for (int i = 1; i < argc; ++i) {
        if (!strcmp(argv[i], "--version") || !strcmp(argv[i], "-v")) {
            printVersionString();
            return false;
        }
    }

    // Look for '--help'
    for (int i = 1; i < argc; ++i) {
        if (!strcmp(argv[i], "--help") || !strcmp(argv[i], "-h") || !strcmp(argv[i], "/?")) {
            printHelpString();
            return false;
        }
    }

    for (int i = 1; i < argc; ++i) {
        if (!strcmp(argv[i], "--verilog"))
            if (i == argc - 1) {
                std::cerr << "__err__ : '--verilog' option should be followed by a valid path to a verilog file.        \nAbort.";
                break;
            } else {
                if(!findVerilog(argv[++i]))
                    return false;
                continue;
            }
        if (!strcmp(argv[i], "--def"))
            if (i == argc - 1) {
                std::cerr << "__err__ : '--def' option should be followed by a valid path to a DEF file.        \nAbort.";
                break;
            }
            else {
                if (!findDEF(argv[++i]))
                    return false;
                continue;
            }
        if (!strcmp(argv[i], "--design"))
            if (i == argc - 1) {
                std::cerr << "__err__ : '--design' option should be followed by a valid path to a design.        \nAbort.";
                break;
            }
            else {
                if (!findDesign(argv[++i]))
                    return false;
                continue;
            }
        if (!strcmp(argv[i], "--lefs"))
            if (i == argc - 1) {
                std::cerr << "__err__ : '--lefs' option should be followed by a valid path to LEF files.        \nAbort.";
                break;
            }
            else {
                if (!findLEFs(argv[++i]))
                    return false;
                continue;
            }
        if (!strcmp(argv[i], "--libs"))
            if (i == argc - 1) {
                std::cerr << "__err__ : '--libs' option should be followed by a valid path to Liberty files.        \nAbort.";
                break;
            }
            else {
                if (!findLEFs(argv[++i]))
                    return false;
                continue;
            }
        if (!strcmp(argv[i], "--asap7"))
            if (i == argc - 1) {
                std::cerr << "__err__ : '--asap7' option should be followed by a valid path to the ASAP7 PDK.        \nAbort.";
                break;
            }
            else {
                if (!findASAP7(argv[++i]))
                    return false;
                continue;
            }
    }

    // Perform elementary checks
    if (verilog.empty() || def.empty() || lefs.empty() || libs.empty()) {
        std::cerr << "__err__ : Some requred files are missing. Abort.\n\n";
        return false;
    }

    return true;
}

void CmdLine::printVersionString() {
    std::cout << "ICCAD 2025 TaskC solver [cadc1079 team] version " PROGRAM_VERSION << "\n\n";
}

void CmdLine::printHelpString() {
    std::cout << "ICCAD 2025 TaskC solver [cadc1079 team]\n\n";
    std::cout << "There are several types of args available.\n";
    std::cout << "  Informational args (each of the args is optional)\n";
    std::cout << "    --help     - show this help, this message (also -h)\n";
    std::cout << "    --version  - show this help (also -v)\n";
    std::cout << "\n";
    std::cout << "  To specify the design to be processed you may use one of theese sets of args:\n";
    std::cout << "    --verilog <file.v> - specify the path to a verilog file\n";
    std::cout << "    --def <file.def>   - specify the path to a DEF file\n";
    std::cout << "  or\n";
    std::cout << "    --design <path>    - path to the design with verilog and DEF files\n";
    std::cout << "                         The program will expect thet there are Verilog and DEF files in the folder\n";
    std::cout << "                         with the same name as the design: <path>.v and <path>.def\n";
    std::cout << "\n";
    std::cout << "  To specify required files from ASAP7 PDK you may use one of these sets of args:\n";
    std::cout << "    --lefs <path>      - path to a folder with LEF files in it\n";
    std::cout << "    --libs <path>      - path to a folder with Liberty files in it\n";
    std::cout << "  or\n";
    std::cout << "    --asap7 <path>     - path to the ASAP7 folder with the same structure as in the given example\n";
}

bool CmdLine::findVerilog(const char *_path) {
    if (!_path)
        return false;
    std::filesystem::path path(_path);
    if (!std::filesystem::exists(path)) {
        std::cerr << "__err__ : Specified verilog file '" << _path << "' doesn't exist!        \nAbort.";
        return false;
    }
    if (!std::filesystem::is_regular_file(path)) {
        std::cerr << "__err__ : Specified verilog file '" << _path << "' is not a regular file!        \nAbort.";
        return false;
    }
    verilog = path.string();
    return true;
}

bool CmdLine::findDEF(const char *_path) {
    if (!_path)
        return false;
    std::filesystem::path path(_path);
    if (!std::filesystem::exists(path)) {
        std::cerr << "__err__ : Specified DEF file '" << _path << "' doesn't exist!        \nAbort.";
        return false;
    }
    if (!std::filesystem::is_regular_file(path)) {
        std::cerr << "__err__ : Specified DEF file '" << _path << "' is not a regular file!        \nAbort.";
        return false;
    }
    def = path.string();
    return true;
}

bool CmdLine::findLEFs(const char *_path) {
    for (const auto &file : std::filesystem::directory_iterator(_path)) {
        if (!std::filesystem::is_regular_file(file.status()))
            continue;
        if (file.path().extension().string() != ".lef")
            continue;
        lefs.push_back(file.path().string());
    }
    if (lefs.empty()) {
        std::cerr << "__err__ : There were no .lef files in the given directory!\n          Abort." << std::endl;
        return false;
    }
    return true;
}

bool CmdLine::findLibs(const char *_path) {
    for (const auto &file : std::filesystem::directory_iterator(_path)) {
        if (!std::filesystem::is_regular_file(file.status()))
            continue;
        if (file.path().extension().string() != ".lib")
            continue;
        libs.push_back(file.path().string());
    }
    if (libs.empty()) {
        std::cerr << "__err__ : There were no .lib files in the given directory!\n          Abort." << std::endl;
        return false;
    }
    return true;
}

bool CmdLine::findDesign(const char *_path) {
    if (!_path)
        return false;
    std::filesystem::path path(_path);
    if (!std::filesystem::exists(path)) {
        std::cerr << "__err__ : Specified design path '" << _path << "' doesn't exist!        \nAbort.";
        return false;
    }
    std::string str(_path);
    std::string verilogFileName;
    std::string defFileName;
    size_t pos = str.find_last_of(DELIMETER);
    if (std::string::npos == pos) {
        verilogFileName = str + DELIMETER + str + ".v";
        defFileName = str + DELIMETER + str + ".def";
    }
    else {
        verilogFileName = str + DELIMETER + str.substr(pos + 1) + ".v";
        defFileName = str + DELIMETER + str.substr(pos + 1) + ".def";
    }
    if (!findVerilog(verilogFileName.c_str()))
        return false;
    if (!findDEF(defFileName.c_str()))
        return false;
    return true;
}

bool CmdLine::findASAP7(const char *_path) {
    if (!_path)
        return false;
    std::filesystem::path path(_path);
    if (!std::filesystem::exists(path)) {
        std::cerr << "__err__ : Specified ASAP7 folder '" << _path << "' doesn't exist!        \nAbort.";
        return false;
    }
    std::string lefPath = std::string(_path) + DELIMETER + std::string("LEF");
    std::string libPath = std::string(_path) + DELIMETER + std::string("LIB");
    if (!findLEFs(lefPath.c_str()))
        return false;
    if (!findLibs(libPath.c_str()))
        return false;
    return true;
}


