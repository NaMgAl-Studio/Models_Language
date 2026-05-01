#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <filesystem>
#include "mslb_parser.h"
#include "dll_generator.h"
#include "compiler.h"

namespace fs = std::filesystem;

static std::string readFile(const std::string& path) {
    std::ifstream file(path);
    if (!file.is_open()) return "";
    std::stringstream ss;
    ss << file.rdbuf();
    return ss.str();
}

int main(int argc, char* argv[]) {
    if (argc < 2) {
        std::cout << "MSLA Packages Maker v0.0.2" << std::endl;
        std::cout << "Usage: packages_maker <file.mslb> [-o <output_dir>]" << std::endl;
        return 0;
    }

    std::string inputPath = argv[1];
    std::string outputDir;

    for (int i = 2; i < argc; i++) {
        std::string arg = argv[i];
        if (arg == "-o" && i + 1 < argc) {
            outputDir = argv[++i];
        }
    }

    if (!fs::exists(inputPath)) {
        std::cerr << "Error: File '" << inputPath << "' not found." << std::endl;
        return 1;
    }

    std::string source = readFile(inputPath);
    if (source.empty()) {
        std::cerr << "Error: Cannot read file '" << inputPath << "' or file is empty." << std::endl;
        return 1;
    }

    MslbParser parser(source, inputPath);
    MslbBag bag = parser.parse();

    if (bag.bagName.empty()) {
        std::cerr << "Error: MSLB file must contain a 'bagname' statement." << std::endl;
        return 1;
    }

    std::cout << "Package: " << bag.bagName << std::endl;
    std::cout << "Code type: " << bag.codeType << std::endl;
    std::cout << "Functions: " << bag.lines.size() << std::endl;

    if (outputDir.empty()) {
        outputDir = fs::path(inputPath).parent_path().string();
    }
    if (outputDir.empty()) {
        outputDir = ".";
    }

    std::string dllOutputPath = (fs::path(outputDir) / (bag.bagName + ".dll")).string();

    DllGenerator generator;
    std::string cppSourcePath = generator.generate(bag, dllOutputPath);

    std::cout << "Generated source: " << cppSourcePath << std::endl;

    DllCompiler compiler;
    if (!compiler.compile(cppSourcePath, dllOutputPath)) {
        std::cerr << "Error: Failed to compile DLL." << std::endl;
        return 1;
    }

    std::cout << "Successfully built: " << dllOutputPath << std::endl;

    fs::remove(cppSourcePath);

    return 0;
}
