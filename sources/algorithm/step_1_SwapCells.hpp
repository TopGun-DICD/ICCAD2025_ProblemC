#pragma once
#include "../verilog/Verilog.hpp"
#include "../def/DEFReader.hpp"
#include <string>
#include <fstream>


int recalculating_links(verilog::Instance ins, def::Position pos);
void step_1_SwapCells_u(verilog::Module* top, def::DEF_File* def);