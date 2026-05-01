#pragma once

#include <string>

class DllCompiler {
public:
    bool compile(const std::string& cppSourcePath, const std::string& outputDllPath);
};
