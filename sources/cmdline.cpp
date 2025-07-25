#include "cmdline.hpp"

#include <filesystem>
#include <iostream>

#define PROGRAM_VERSION "0.0.4"
#if defined _WIN32
#   define DELIMETER "\\"
#else
#   define DELIMETER "/"
#endif

#define ENABLE_VERILOG_READING
#define ENABLE_LEF_READING
#define ENABLE_DEF_READING
#define ENABLE_LIB_READING

bool CmdLine::parse(int argc, char *argv[]) {
    std::cout << "ICCAD 2025 TaskC solver [team cadc1079]\n\n";
    if (1 == argc) {
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
        if (!strcmp(argv[i], "--out"))
            if (i == argc - 1) {
                std::cerr << "__err__ : '--out' option should be followed by a valid path to the output file.          \nAbort.";
                break;
            }
            else {
                outFile = argv[++i];
                continue;
            }
    }

    // Perform elementary checks
    if (
#if defined ENABLE_VERILOG_READING        
        verilog.empty() ||
#endif
#if defined ENABLE_DEF_READING
        def.empty() ||
#endif
#if defined ENABLE_DEF_READING
        lefs.empty() ||
#endif
#if defined ENABLE_LIBS_READING
        libs.empty()
#endif
        false) {
        std::cerr << "__err__ : Some reqiured files are missing. Abort.\n\n";
        return false;
    }

    if (outFile.empty()) {
        std::filesystem::path path(def);
        outFile = path.parent_path().string() + DELIMETER + path.stem().string() + "_result.def";
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
    std::cout << "\n";
    std::cout << "  To specify output DEF file may should use this arg:\n";
    std::cout << "    --out <file.def>   - path to a file the results shoud be stored to\n";
    std::cout << "    Notes to this argument:\n";
    std::cout << "        - if it is not specified the result will be written in a file named as an input one but with the suffix '_result'\n";
    std::cout << "          (fore example, if input file is 'tests/aes_cipher_top/aes_cipher_top.def' then the result will be written to\n";
    std::cout << "          'tests/aes_cipher_top/aes_cipher_top_result.def'\n";
    std::cout << "        - if the given file already exist it will be overwritten without prompt\n\n";
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
#if defined ENABLE_VERILOG_READING
    if (!findVerilog(verilogFileName.c_str()))
        return false;
#endif
#if defined ENABLE_DEF_READING
    if (!findDEF(defFileName.c_str()))
        return false;
#endif
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
#if defined ENABLE_LEF_READING
    if (!findLEFs(lefPath.c_str()))
        return false;
#endif
#if defined ENABLE_LIB_READING
    if (!findLibs(libPath.c_str()))
        return false;
#endif
    return true;
}


