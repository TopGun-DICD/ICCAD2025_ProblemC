#pragma once
#include "../verilog/Verilog.hpp"
#include "../def/DEFReader.hpp"
#include <string>
#include <fstream>

uint64_t recalculating_links_max(verilog::Instance& inst, def::Position pos);
uint64_t recalculating_links(verilog::Instance &ins, def::Position pos);
void step_1_SwapCells_u(verilog::Module* top, def::DEF_File* def);