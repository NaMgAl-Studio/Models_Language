#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include "lexer.h"
#include "parser.h"
#include "interpreter.h"
#include "error.h"

static std::string readFile(const std::string& path) {
    std::ifstream file(path);
    if (!file.is_open()) return "";
    std::stringstream ss;
    ss << file.rdbuf();
    return ss.str();
}

int main(int argc, char* argv[]) {
    if (argc < 2) {
        std::cout << "MSLA Interpreter v0.0.2" << std::endl;
        std::cout << "Usage: msla_interpreter <file.msla>" << std::endl;
        return 0;
    }

    std::string filePath = argv[1];
    std::string source = readFile(filePath);
    if (source.empty()) {
        std::cerr << "Error: Cannot read file '" << filePath << "'" << std::endl;
        return 1;
    }

    try {
        Lexer lexer(source, filePath);
        auto tokens = lexer.tokenize();

        Parser parser(tokens);
        auto program = parser.parse();

        Interpreter interpreter;
        interpreter.execute(program, filePath);
    } catch (const MslaError& e) {
        std::cerr << e.what() << std::endl;
        return 1;
    } catch (const std::exception& e) {
        std::cerr << "Internal error: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}
