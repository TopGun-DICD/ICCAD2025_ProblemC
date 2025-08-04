#pragma once

#include "DEF.hpp"

namespace def {

    class DEFWriter {
    public:
        void OutDEF(std::string nameOutFile, DEF_File& def, const std::vector<std::tuple<std::string, std::string, std::string, double, double, std::string>>& replacements);
        static std::string Orientation_transform(Orientation o);
    private:
        std::string SOURCE_class_transform(SOURCE_class o);
        std::string FIXED_class_transform(FIXED_class o);
        std::string DIRECTION_class_transform(DIRECTION_class o);
        std::string USE_class_transform(USE_class o);
    };

}