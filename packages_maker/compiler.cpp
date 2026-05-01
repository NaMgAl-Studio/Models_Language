#include "compiler.h"
#include <cstdlib>
#include <iostream>

bool DllCompiler::compile(const std::string& cppSourcePath, const std::string& outputDllPath) {
    std::string compiler = "g++";
    if (cppSourcePath.size() >= 2 && cppSourcePath.substr(cppSourcePath.size() - 2) == ".c") {
        compiler = "gcc";
    }
    std::string cmd = compiler + " -shared -o \"" + outputDllPath + "\" \"" + cppSourcePath + "\" -static-libgcc 2>&1";

    FILE* pipe = _popen(cmd.c_str(), "r");
    if (!pipe) {
        std::cerr << "Error: Cannot run g++ compiler. Make sure MinGW is installed and in PATH." << std::endl;
        return false;
    }

    std::string output;
    char buffer[256];
    while (fgets(buffer, sizeof(buffer), pipe)) {
        output += buffer;
    }
    int status = _pclose(pipe);

    if (status != 0) {
        std::cerr << "Compilation failed:" << std::endl;
        std::cerr << output << std::endl;
        return false;
    }

    return true;
}
