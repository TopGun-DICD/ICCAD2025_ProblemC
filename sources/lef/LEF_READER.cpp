#include <iomanip>
#include <stdexcept>
#include <sstream> 
#include <ctime>

#include "LEF_READER.hpp"

LEF_READER::~LEF_READER() {
    clear();
}

bool LEF_READER::read(const std::string& filename) {

    std::cout << "Reading input lef file '" << filename << "'...\n";
    std::time_t timeStart = std::clock();

    std::ifstream file(filename);
    if (!file.is_open()) {
        throw std::runtime_error("Could not open file: " + filename);
    }

    std::string line;
    while (getline(file, line)) {
        line = trim(line);

        if (line[0] == '#' || line.find("VERSION") == 0 ||
            line.find("BUSBITCHARS") == 0 || line.find("DIVIDERCHAR") == 0) {
            continue;
        }

        if (line.find("SITE") == 0) {
            auto tokens = tokenize(line);
            if (tokens.size() >= 2) {
                Site* site = parseSite(file, tokens[1], sites);
                if (site) {
                    sites.push_back(site);
                }
            }
        }

        else if (line.find("MACRO") == 0) {
            auto tokens = tokenize(line);
            if (tokens.size() >= 2) {
                Macro* macro = parseMacro(file, tokens[1]);
                if (macro) {
                    macros.push_back(macro);
                }
            }
        }
    }

    std::time_t timeStop = std::clock() - timeStart;
    std::time_t timeValMin = 0;
    std::time_t timeValSec = 0;
    std::time_t timeValMsec = timeStop;

    if (timeStop < 1000) {
        if (timeStop == 0)
            timeValMsec = 1;
    }
    else {
        if (timeStop > 1000) {
            timeValSec = timeStop / 1000;
            timeValMsec = timeStop - timeValSec * 1000;
        }
        if (timeValSec > 60) {
            timeValMin = timeValSec / 60;
            timeValSec = timeValSec - (timeValMin * 60);
        }
    }

    std::cout << "Done reading input file. File has been read in "
        << timeValMin << " min(s) " << timeValSec << " sec(s) " << timeValMsec << " msec(s)" << std::endl;
        file.close();
      
        return true;
    }

Macro* LEF_READER::getMacroByName(const std::string& name) const {
    for (auto macro : macros) {
        if (macro->name == name) {
            return macro;
        }
    }
    return nullptr;
}

void LEF_READER::exportToFile(const std::string& filename) const {
    std::ofstream out(filename);
    if (!out.is_open()) {
        throw std::runtime_error("Could not create file: " + filename);
    }

    for (const auto& macro : macros) {
        out << "MACRO " << macro->name << "\n";
        out << "  SIZE " << macro->sizeX << " BY " << macro->sizeY << "\n";
    }
    out.close();
}

std::string LEF_READER::trim(const std::string& str) {
    size_t first = str.find_first_not_of(" \t;");
    if (first == std::string::npos) return "";
    size_t last = str.find_last_not_of(" \t;");
    return str.substr(first, last - first + 1);
}

std::vector<std::string> LEF_READER::tokenize(const std::string& line) {
    std::vector<std::string> tokens;
    std::istringstream iss(line);
    std::string token;
    while (iss >> token) {
        tokens.push_back(token);
    }
    return tokens;
}

double LEF_READER::safeStod(const std::string& str) {
    try {
        return std::stod(str);
    }
    catch (...) {
        return 0.0;
    }
}

Rect* LEF_READER::parseRect(const std::vector<std::string>& tokens) const {
    if (tokens.size() < 5) return nullptr;
    Rect* rect = new Rect{
        safeStod(tokens[1]),
        safeStod(tokens[2]),
        safeStod(tokens[3]),
        safeStod(tokens[4])
                  
    };
   
    return rect;
}

Port_lef* LEF_READER::parsePort(std::ifstream& file) const {
    Port_lef* port = new Port_lef();
    std::string line;

    while (getline(file, line)) {
        line = trim(line);
        if (line.empty()) continue;

        auto tokens = tokenize(line);
        if (tokens.empty()) continue;

        if (tokens[0] == "LAYER") {
            port->layer = tokens[1];
        }
        else if (tokens[0] == "RECT") {
            Rect* rect = parseRect(tokens);
            port->rects.push_back(*rect);
            delete rect;
        }
        else if (tokens[0] == "END") {
            break;
        }
    }
    return port;
}

Pin* LEF_READER::parsePin(std::ifstream& file, const std::string& name) const {
    Pin* pin = new Pin();
    pin->name = name;
    std::string line;

    while (getline(file, line)) {
        line = trim(line);
        if (line.empty()) continue;

        auto tokens = tokenize(line);
        if (tokens.empty()) continue;

        if (tokens[0] == "DIRECTION") {
            pin->direction = tokens[1];
        }
        else if (tokens[0] == "USE") {
            pin->use = tokens[1];
        }
        else if (tokens[0] == "SHAPE") {
            pin->shape = tokens[1];
        }
        else if (tokens[0] == "PORT") {
            Port_lef* port = parsePort(file);
            pin->port = *port;
            delete port;
        }
        else if (tokens[0] == "END" && tokens.size() > 1 && tokens[1] == name) {
            break;
        }
    }
    return pin;
}

Obs* LEF_READER::parseObs(std::ifstream& file) const {
    Obs* obs = new Obs();
    std::string currentLayer;
    std::vector<Rect> currentRects;
    std::string line;

    while (getline(file, line)) {
        line = trim(line);
        if (line.empty()) continue;

        auto tokens = tokenize(line);
        if (tokens.empty()) continue;

        if (tokens[0] == "LAYER") {
            if (!currentLayer.empty()) {
                obs->geometries.emplace_back(currentLayer, currentRects);
                currentRects.clear();
            }
            currentLayer = tokens[1];
        }
        else if (tokens[0] == "RECT") {
            Rect* rect = parseRect(tokens);
            currentRects.push_back(*rect);
            delete rect;
        }
        else if (tokens[0] == "END") {
            if (!currentLayer.empty()) {
                obs->geometries.emplace_back(currentLayer, currentRects);
            }
            break;
        }
    }
    return obs;
}

Macro* LEF_READER::parseMacro(std::ifstream& file, const std::string& name) {
    Macro* macro = new Macro();
    macro->name = name;
    std::string line;
    bool hasXYSymmetry = false;

    while (getline(file, line)) {
        line = trim(line);
        if (line.empty()) continue;

        auto tokens = tokenize(line);
        if (tokens.empty()) continue;

        if (tokens[0] == "CLASS") {
            macro->className = tokens[1];
        }
        else if (tokens[0] == "FOREIGN") {
            macro->foreignName = tokens[1];
        }
        else if (tokens[0] == "SIZE") {
            macro->sizeX = safeStod(tokens[1]);
            macro->sizeY = safeStod(tokens[3]);
        }
        else if (tokens[0] == "SYMMETRY") {
            for (size_t i = 1; i < tokens.size(); ++i) {
                macro->symmetry += tokens[i];
            }
            hasXYSymmetry = (macro->symmetry == "XY");
        }
        else if (tokens[0] == "PIN") {
            Pin* pin = parsePin(file, tokens[1]);
            macro->pins.push_back(*pin);
            delete pin;
        }
        else if (tokens[0] == "OBS") {
            Obs* obs = parseObs(file);
            macro->obs = *obs;
            delete obs;
        }
        else if (tokens[0] == "END" && tokens.size() > 1 && tokens[1] == name) {
            break;
        }
    }

    if (!hasXYSymmetry) {
        std::cout << "Macro '" << macro->name << "' has non-XY symmetry: " << macro->symmetry << std::endl;
    }

    return macro;
}

Site* LEF_READER::parseSite(std::ifstream& file, const std::string& name, const std::vector<Site*>& existingSites) {

    for (const auto& site : existingSites) {
        if (site->name == name) {
            std::string line;
            while (getline(file, line)) {
                line = trim(line);
                if (line.empty()) continue;

                std::vector<std::string> tokens = tokenize(line);
                if (tokens.empty()) continue;

                if (tokens[0] == "END" && tokens.size() > 1 && tokens[1] == name) {
                    break;
                }
            }
            return nullptr;
        }
    }

    Site* site = new Site();
    site->name = name;
    std::string line;

    while (getline(file, line)) {
        line = trim(line);
        if (line.empty()) continue;

        auto tokens = tokenize(line);
        if (tokens.empty()) continue;

        if (tokens[0] == "CLASS") {
            site->className = tokens[1];
        }
        else if (tokens[0] == "SIZE") {
            site->sizeX = safeStod(tokens[1]);
            site->sizeY = safeStod(tokens[3]);
        }
        else if (tokens[0] == "SYMMETRY") {
            site->symmetry = tokens[1];
        }
        else if (tokens[0] == "END" && tokens.size() > 1 && tokens[1] == name) {
            break;
        }
    }
    return site;
}

void LEF_READER::clear() {
    for (auto macro : macros) delete macro;
    for (auto site : sites) delete site;
    macros.clear();
    sites.clear();
}