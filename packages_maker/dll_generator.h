#pragma once

#include <string>
#include "mslb_parser.h"

class DllGenerator {
public:
    std::string generate(const MslbBag& bag, const std::string& dllOutputPath);
};
