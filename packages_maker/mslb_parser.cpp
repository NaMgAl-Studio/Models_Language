#include "mslb_parser.h"
#include <algorithm>
#include <cctype>

MslbParser::MslbParser(const std::string& source, const std::string& file)
    : source(source), file(file), pos(0), line(1), column(1) {}

char MslbParser::peek() {
    if (isAtEnd()) return '\0';
    return source[pos];
}

char MslbParser::advance() {
    char c = source[pos];
    pos++;
    if (c == '\n') {
        line++;
        column = 1;
    } else {
        column++;
    }
    return c;
}

bool MslbParser::isAtEnd() {
    return pos >= source.size();
}

bool MslbParser::isAlpha(char c) {
    return std::isalpha(static_cast<unsigned char>(c)) || c == '_';
}

bool MslbParser::isAlphaNumeric(char c) {
    return isAlpha(c) || std::isdigit(static_cast<unsigned char>(c));
}

bool MslbParser::isWhitespace(char c) {
    return c == ' ' || c == '\t' || c == '\r';
}

void MslbParser::skipWhitespaceAndComments() {
    while (!isAtEnd()) {
        if (isWhitespace(peek()) || peek() == '\n') {
            advance();
        } else if (peek() == '#') {
            while (!isAtEnd() && peek() != '\n') {
                advance();
            }
        } else {
            break;
        }
    }
}

std::string MslbParser::readIdentifier() {
    std::string result;
    while (!isAtEnd() && (isAlphaNumeric(peek()) || peek() == '.')) {
        result += advance();
    }
    return result;
}

std::string MslbParser::readUntilNewline() {
    std::string result;
    while (!isAtEnd() && peek() != '\n') {
        result += advance();
    }
    if (!isAtEnd()) advance();
    return result;
}

std::string MslbParser::readContentBlock() {
    std::string result;
    while (!isAtEnd()) {
        size_t savedPos = pos;
        int savedLine = line;
        int savedCol = column;

        while (!isAtEnd() && isWhitespace(peek())) {
            advance();
        }
        if (!isAtEnd() && isAlpha(peek())) {
            std::string word;
            while (!isAtEnd() && isAlphaNumeric(peek())) {
                word += advance();
            }
            if (word == "contentend") {
                if (!result.empty() && result.back() == '\n') {
                    result.pop_back();
                }
                return result;
            }
        }

        pos = savedPos;
        line = savedLine;
        column = savedCol;

        while (!isAtEnd() && peek() != '\n') {
            result += advance();
        }
        if (!isAtEnd()) {
            result += '\n';
            advance();
        }
    }

    if (!result.empty() && result.back() == '\n') {
        result.pop_back();
    }
    return result;
}

std::string MslbParser::extractFuncName(const std::string& grammar) {
    for (size_t i = 0; i < grammar.size(); i++) {
        if (std::isalpha(static_cast<unsigned char>(grammar[i])) || grammar[i] == '_') {
            std::string name;
            while (i < grammar.size() && (std::isalnum(static_cast<unsigned char>(grammar[i])) || grammar[i] == '_')) {
                name += grammar[i];
                i++;
            }
            return name;
        }
    }
    return grammar;
}

std::vector<MslbGrammarParam> MslbParser::extractGrammarParams(const std::string& grammar) {
    std::vector<MslbGrammarParam> params;
    for (size_t i = 0; i < grammar.size(); i++) {
        if (grammar[i] == '<') {
            size_t j = i + 1;
            std::string name;
            while (j < grammar.size() && grammar[j] != '>') {
                name += grammar[j];
                j++;
            }
            if (j < grammar.size() && grammar[j] == '>' && !name.empty()) {
                params.push_back({name});
            }
            i = j;
        }
    }
    return params;
}

MslbBag MslbParser::parse() {
    MslbBag bag;

    while (!isAtEnd()) {
        skipWhitespaceAndComments();
        if (isAtEnd()) break;

        std::string keyword = readIdentifier();
        if (keyword.empty()) {
            advance();
            continue;
        }

        if (keyword == "bagname" || keyword == "packagename") {
            skipWhitespaceAndComments();
            bag.bagName = readIdentifier();
        } else if (keyword == "def") {
            skipWhitespaceAndComments();
            std::string subKw = readIdentifier();
            if (subKw == "var") {
                MslbVarDef varDef;
                skipWhitespaceAndComments();
                varDef.name = readIdentifier();
                skipWhitespaceAndComments();
                if (peek() == '-') {
                    advance();
                    std::string typeFlag;
                    typeFlag += advance();
                    if (typeFlag == "n") {
                        varDef.type = MslbVarType::NUMBER;
                    } else if (typeFlag == "w") {
                        varDef.type = MslbVarType::STRING;
                    }
                }
                bag.varDefs.push_back(varDef);
            }
        } else if (keyword == "code_type") {
            skipWhitespaceAndComments();
            bag.codeType = readIdentifier();
        } else if (keyword == "line") {
            MslbLine lineNode;
            skipWhitespaceAndComments();
            lineNode.name = readIdentifier();

            while (!isAtEnd()) {
                skipWhitespaceAndComments();
                if (isAtEnd()) break;

                size_t savedPos = pos;
                std::string subWord = readIdentifier();

                if (subWord == "lineend") {
                    break;
                } else if (subWord == "grammar") {
                    if (!isAtEnd() && peek() == ':') {
                        advance();
                    }
                    skipWhitespaceAndComments();
                    std::string grammarLine = readUntilNewline();
                    size_t start = grammarLine.find_first_not_of(" \t\r");
                    size_t end = grammarLine.find_last_not_of(" \t\r\n");
                    if (start != std::string::npos && end != std::string::npos) {
                        grammarLine = grammarLine.substr(start, end - start + 1);
                    }
                    lineNode.grammar = grammarLine;
                    lineNode.funcName = extractFuncName(grammarLine);
                    lineNode.grammarParams = extractGrammarParams(grammarLine);
                } else if (subWord == "content") {
                    skipWhitespaceAndComments();
                    lineNode.content = readContentBlock();
                } else {
                    pos = savedPos;
                    readUntilNewline();
                }
            }

            bag.lines.push_back(lineNode);
        } else {
            readUntilNewline();
        }
    }

    return bag;
}
