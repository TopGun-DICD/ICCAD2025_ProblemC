#include "LibertyReader.hpp"

#include <iostream>
#include <fstream>
#include <unordered_set>
#include <ctime>

#define MAX_TOKEN_LENGTH    256

std::unordered_set<char> oneByteTokens_Liberty{ '(', ')', '[', ']', /*'.',*/ ',', '*', '/', '+', '-', '@', '`', '$', '\\', ';', ':', '#', '<' };

liberty::LibertyReader::~LibertyReader() {
    if (code) {
        delete[] code;
        code = nullptr;
    }
}

bool liberty::LibertyReader::read(const std::string &_fname, Liberty &_liberty, verilog::Netlist &_netlist) {
    if (!readCode(_fname))
        return false;

    liberty = &_liberty;
    netlist = &_netlist;

    char token[MAX_TOKEN_LENGTH];
    while (posInCode < codeLength) {
        readToken(token);
        if (token[0] == '\0')
            continue;
        switch (token[0]) {
            case 'l':
                if (!strcmp(token, "library")) {
                    if (!readLibrary())
                        return false;
                }
                break;
            default:
                std::cerr << "  __err__ : unsupported token '" << token << "'. Abort." << std::endl;
                return false;
        }
    }

    liberty->fileName = _fname;

    postProcess();

    return true;
}

bool liberty::LibertyReader::readCode(const std::string &fname) {
    std::ifstream in(fname, std::ios::in);
    if (!in.is_open())
        return false;

    in.seekg(0, std::ios::end);
    codeLength = in.tellg();
    in.seekg(0, std::ios::beg);
    code = new char[codeLength + 1];
    in.read(code, codeLength);
    in.close();
    code[codeLength] = '\0';

    posInCode = 0;
    line = 1;
    return true;
}

void liberty::LibertyReader::readToken(char *token) {
    token[0] = '\0';
    int i = 0;
token_start:
    if (posInCode >= codeLength)
        return;
    while (code[posInCode] == ' ' || code[posInCode] == '\t')
        ++posInCode;
    if (code[posInCode] == '\n') {
        ++posInCode;
        ++line;
        goto token_start;
    }

    while (posInCode < codeLength) {
        if (code[posInCode] == '/' && code[posInCode + 1] == '/') {
            readToTheEOL();
            goto token_start;
        }
        if (code[posInCode] == '/' && code[posInCode + 1] == '*') {
            while (posInCode < codeLength) {
                if (code[posInCode] == '\n')
                    ++line;
                if (code[posInCode] == '*' && code[posInCode + 1] == '/') {
                    posInCode += 2;
                    break;
                }
                ++posInCode;
            }
            goto token_start;
        }
        if (code[posInCode] == '(' && code[posInCode + 1] == '*') {
            while (posInCode < codeLength) {
                if (code[posInCode] == '*' && code[posInCode + 1] == ')') {
                    posInCode += 2;
                    break;
                }
                ++posInCode;
            }
            goto token_start;
        }
        if (oneByteTokens_Liberty.find(code[posInCode]) != oneByteTokens_Liberty.end()) {
            if (!i) {
                token[i] = code[posInCode];
                token[i + 1] = '\0';
                ++posInCode;
                return;
            }
            token[i] = '\0';
            return;
        }
        if (code[posInCode] == ' ' || code[posInCode] == '\t') {
            ++posInCode;
            token[i] = '\0';
            return;
        }
        if (code[posInCode] == '\n') {
            ++posInCode;
            ++line;
            token[i] = '\0';
            return;
        }
        token[i++] = code[posInCode++];
    }
}

void liberty::LibertyReader::readIdentifier(char* token) {
    readToken(token);
    if (token[0] != '\\')
        return;
    int j = 1;
    while (code[posInCode] != ' ') {
        token[j] = code[posInCode];
        ++j;
        ++posInCode;
    }
    token[j] = '\0';
}

void liberty::LibertyReader::readToTheEOL() {
    while (code[posInCode] != '\n' && posInCode < codeLength)
        ++posInCode;
    ++posInCode;
    ++line;
}

void liberty::LibertyReader::readUntil(char symbol) {
    while (code[posInCode] != symbol && posInCode < codeLength) {
        ++posInCode;
        if (code[posInCode] == '\n')
            ++line;
    }
    ++posInCode;
}

void liberty::LibertyReader::readUntil2(char symbol1, char symbol2) {
    int depth = 1;
    while ((code[posInCode] != symbol2 || depth != 1) && posInCode < codeLength) {
        ++posInCode;
        if (code[posInCode] == '\n')
            ++line;
        if (code[posInCode] == symbol1)
            ++depth;
        if (code[posInCode] == symbol2)
            --depth;
    }
    ++posInCode;
}

bool liberty::LibertyReader::readLibrary() {
    char token[MAX_TOKEN_LENGTH];
    token[0] = '\0';

    Library *library = new Library;
    liberty->libraries.push_back(library);
    readToken(token);  // (
    readIdentifier(token);
    library->name = token;
    readToken(token);  // )
    readToken(token);  // {

    while (posInCode < codeLength) {
        readToken(token);
        if (token[0] == '\0')
            continue;
        if (!strcmp(token, "cell")) {
            if (!readCell(library))
                return false;
            continue;
        }
        if (!strcmp(token, "operating_conditions")) {
            readUntil('}');
            continue;
        }
        if (!strcmp(token, "lu_table_template")) {
            readUntil('}');
            continue;
        }
        if (!strcmp(token, "power_lut_template")) {
            readUntil('}');
            continue;
        }
        if (!strcmp(token, "input_voltage")) {
            readUntil('}');
            continue;
        }
        if (!strcmp(token, "output_voltage")) {
            readUntil('}');
            continue;
        }
        if (!strcmp(token, "normalized_driver_waveform")) {
            readUntil('}');
            continue;
        }
        readUntil(';');
        
    }
    return true;
}

bool liberty::LibertyReader::readCell(Library *library) {
    char token[MAX_TOKEN_LENGTH];
    token[0] = '\0';

    Cell *cell = new Cell;
    library->cells.push_back(cell);
    readToken(token);  // (
    readIdentifier(token);
    cell->name = token;
    readToken(token);  // )
    readToken(token);  // {

    while (posInCode < codeLength) {
        readIdentifier(token);
        if (!strcmp(token, "area")) {
            readToken(token);   // :
            readToken(token);
            cell->area = std::atof(token);
            readToken(token);   // ;
            continue;
        }
        if (!strcmp(token, "pg_pin")) {
            readUntil('}');
            continue;
        }
        if (!strcmp(token, "leakage_power")) {
            readUntil('}');
            continue;
        }
        if (!strcmp(token, "pin")) {
            if (!readPin(cell))
                return false;
            continue;
        }
        if (token[0] == '}') {
            break;
        }
    }

    return true;
}

bool liberty::LibertyReader::readPin(Cell *_cell) {
    char token[MAX_TOKEN_LENGTH];
    token[0] = '\0';

    Pin *pin = new Pin;
    readToken(token);  // (
    readIdentifier(token);
    pin->name = token;
    readToken(token);  // )
    readToken(token);  // {

    while (posInCode < codeLength) {
        readIdentifier(token);
        if (token[0] == '}')
            break;
        if (!strcmp(token, "direction")) {
            readToken(token);   // :
            readToken(token);
            if (!strcmp(token, "input"))
                _cell->ins.push_back(pin);
            else
                if (!strcmp(token, "output"))
                    _cell->outs.push_back(pin);
                else {
                    std::cerr << "__err__ : unsupported pin direction '" << token << "' for pin '" << pin->name 
                              << "' of cell '" << _cell->name << "'\n. Abort reading liberty file.";
                    return false;
                }
            readToken(token);   // ;
            continue;
        }
        if (!strcmp(token, "capacitance")) {
            readToken(token);   // :
            readToken(token);
            pin->capacitance = std::atof(token);
            readToken(token);   // ;
            continue;
        }
        if (!strcmp(token, "max_capacitance")) {
            readToken(token);   // :
            readToken(token);
            pin->capacitance = std::atof(token);
            readToken(token);   // ;
            continue;
        }
        if (!strcmp(token, "timing")) {
            readUntil2('{', '}');
            continue;
        }
        if (!strcmp(token, "internal_power")) {
            readUntil2('{', '}');
            continue;
        }
        readUntil(';');

    }
    return true;
}

bool liberty::LibertyReader::postProcess() {
    for (verilog::Module *module : netlist->library)
        for (verilog::Instance *instance : module->instances) {
            for(Library *library : liberty->libraries)
                for (Cell *cell : library->cells) {
                    if (cell->name == instance->instanceOf->name)
                        instance->libertyCell = cell;
                }
        }
    return true;
}

