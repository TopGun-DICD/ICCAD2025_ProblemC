#include <iomanip>
#include <stdexcept>
#include <sstream> 
#include <ctime>

#include "LEF_READER.hpp"

lef::LEF_READER::~LEF_READER() {
}

bool lef::LEF_READER::read(const std::string& filename, LEFData &lefs) {

    std::cout << "Reading input LEF file '" << filename << "'...\n";
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
                parseSite(file, tokens[1], lefs);
            }
        }

        else if (line.find("MACRO") == 0) {
            auto tokens = tokenize(line);
            if (tokens.size() >= 2) {
                parseMacro(file, tokens[1], lefs);
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

    std::cout << "Done reading input LEF file. File has been read in "
        << timeValMin << " min(s) " << timeValSec << " sec(s) " << timeValMsec << " msec(s)" << std::endl;
        file.close();
      
        return true;
    }

/*
lef::Macro* lef::LEF_READER::getMacroByName(const std::string& name) const {
    for (auto macro : macros) {
        if (macro->name == name) {
            return macro;
        }
    }
    return nullptr;
}
*/

/*
void lef::LEF_READER::exportToFile(const std::string& filename) const {
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
*/

std::string lef::LEF_READER::trim(const std::string& str) {
    size_t first = str.find_first_not_of(" \t;");
    if (first == std::string::npos) return "";
    size_t last = str.find_last_not_of(" \t;");
    return str.substr(first, last - first + 1);
}

std::vector<std::string> lef::LEF_READER::tokenize(const std::string& line) {
    std::vector<std::string> tokens;
    std::istringstream iss(line);
    std::string token;
    while (iss >> token) {
        tokens.push_back(token);
    }
    return tokens;
}

double lef::LEF_READER::safeStod(const std::string& str) {
    try {
        return std::stod(str);
    }
    catch (...) {
        return 0.0;
    }
}

void lef::LEF_READER::parseRect(const std::vector<std::string>& tokens, std::vector<Rect>& rects) const {
    if (tokens.size() < 5) return;
    rects.push_back(Rect{
        safeStod(tokens[1]),
        safeStod(tokens[2]),
        safeStod(tokens[3]),
        safeStod(tokens[4])
        }
    );
}

lef::Port* lef::LEF_READER::parsePort(std::ifstream& file) const {
    Port* port = new Port;
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
            parseRect(tokens, port->rects);
        }
        else if (tokens[0] == "END") {
            break;
        }
    }
    return port;
}

void lef::LEF_READER::parsePin(std::ifstream& file, const std::string& name, Macro *macro) const {
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
            Port* port = parsePort(file);
            pin->port = *port;
            delete port;
        }
        else if (tokens[0] == "END" && tokens.size() > 1 && tokens[1] == name) {
            break;
        }
    }
    macro->pins.push_back(pin);
}

void lef::LEF_READER::parseObs(std::ifstream& file, Macro *macro) const {
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
                macro->obs.geometries.emplace_back(currentLayer, currentRects);
                currentRects.clear();
            }
            currentLayer = tokens[1];
        }
        else if (tokens[0] == "RECT") {
            parseRect(tokens, currentRects);
        }
        else if (tokens[0] == "END") {
            if (!currentLayer.empty()) {
                macro->obs.geometries.emplace_back(currentLayer, currentRects);
            }
            break;
        }
    }
}

void lef::LEF_READER::parseMacro(std::ifstream& file, const std::string& name, LEFData &lefs) {
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
            parsePin(file, tokens[1], macro);
        }
        else if (tokens[0] == "OBS") {
            parseObs(file, macro);
        }
        else if (tokens[0] == "END" && tokens.size() > 1 && tokens[1] == name) {
            break;
        }
    }

    if (!hasXYSymmetry) {
        std::cout << "Macro '" << macro->name << "' has non-XY symmetry: " << macro->symmetry << std::endl;
    }

    lefs.addMacro(macro);
}

void lef::LEF_READER::parseSite(std::ifstream& file, const std::string& name, LEFData& lefs) {
    if (lefs.getSiteByName(name)) {
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
        return;
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
    lefs.addSite(site);
}

/*
void lef::LEF_READER::clear() {
    for (auto macro : macros) delete macro;
    for (auto site : sites) delete site;
    macros.clear();
    sites.clear();
}
*/
