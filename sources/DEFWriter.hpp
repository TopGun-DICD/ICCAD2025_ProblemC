#pragma once

#include "DEFReader.hpp"

string Orientation_transform(Orientation o);
string SOURCE_class_transform(SOURCE_class o);
string FIXED_class_transform(FIXED_class o);
string DIRECTION_class_transform(DIRECTION_class o);
string USE_class_transform(USE_class o);

void OutDEF(DEF_File* def, string nameOutFile);