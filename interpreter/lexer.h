#pragma once

#include <string>
#include <vector>
#include "common.h"
#include "error.h"

class Lexer {
public:
    Lexer(const std::string& source, const std::string& file);
    std::vector<Token> tokenize();

private:
    std::string source;
    std::string file;
    size_t pos;
    int line;
    int column;

    char peek();
    char peekNext();
    char advance();
    bool isAtEnd();
    void skipWhitespaceAndComments();
    Token scanToken();
    Token scanIdentifierOrKeyword();
    Token scanNumber();
    Token scanString();
    Token makeToken(TokenType type, const std::string& val, int startLine, int startCol);
    bool isAlpha(char c);
    bool isDigit(char c);
    bool isAlphaNumeric(char c);
};
