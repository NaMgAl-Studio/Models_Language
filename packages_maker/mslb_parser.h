#pragma once

#include <string>
#include <vector>
#include <memory>

enum class MslbVarType {
    NUMBER,
    STRING
};

struct MslbVarDef {
    std::string name;
    MslbVarType type;
};

struct MslbGrammarParam {
    std::string name;
};

struct MslbLine {
    std::string name;
    std::string grammar;
    std::string funcName;
    std::vector<MslbGrammarParam> grammarParams;
    std::string content;
};

struct MslbBag {
    std::string bagName;
    std::vector<MslbVarDef> varDefs;
    std::string codeType;
    std::vector<MslbLine> lines;
};

class MslbParser {
public:
    MslbParser(const std::string& source, const std::string& file);
    MslbBag parse();

private:
    std::string source;
    std::string file;
    size_t pos;
    int line;
    int column;

    void skipWhitespaceAndComments();
    std::string readIdentifier();
    std::string readUntilNewline();
    std::string readContentBlock();
    std::string extractFuncName(const std::string& grammar);
    std::vector<MslbGrammarParam> extractGrammarParams(const std::string& grammar);
    char peek();
    char advance();
    bool isAtEnd();
    bool isAlpha(char c);
    bool isAlphaNumeric(char c);
    bool isWhitespace(char c);
};
