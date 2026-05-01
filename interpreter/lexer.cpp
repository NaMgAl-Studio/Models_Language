#include "lexer.h"
#include <cctype>

Lexer::Lexer(const std::string& source, const std::string& file)
    : source(source), file(file), pos(0), line(1), column(1) {}

char Lexer::peek() {
    if (isAtEnd()) return '\0';
    return source[pos];
}

char Lexer::peekNext() {
    if (pos + 1 >= source.size()) return '\0';
    return source[pos + 1];
}

char Lexer::advance() {
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

bool Lexer::isAtEnd() {
    return pos >= source.size();
}

bool Lexer::isAlpha(char c) {
    return std::isalpha(static_cast<unsigned char>(c)) || c == '_';
}

bool Lexer::isDigit(char c) {
    return std::isdigit(static_cast<unsigned char>(c));
}

bool Lexer::isAlphaNumeric(char c) {
    return isAlpha(c) || isDigit(c);
}

void Lexer::skipWhitespaceAndComments() {
    while (!isAtEnd()) {
        char c = peek();
        if (c == ' ' || c == '\t' || c == '\r') {
            advance();
        } else if (c == '#') {
            // 检查是否为多行注释 ### ... ###
            if (pos + 2 < source.size() && source[pos + 1] == '#' && source[pos + 2] == '#') {
                advance(); advance(); advance(); // 跳过开头的 ###
                // 跳过所有内容，直到遇到单独成行的 ###
                while (!isAtEnd()) {
                    if (peek() == '\n') {
                        advance();
                        // 新行开始，跳过前导空白
                        while (!isAtEnd() && (peek() == ' ' || peek() == '\t' || peek() == '\r')) {
                            advance();
                        }
                        // 检查是否为结尾的 ###
                        if (!isAtEnd() && peek() == '#'
                            && pos + 2 < source.size() && source[pos + 1] == '#' && source[pos + 2] == '#') {
                            advance(); advance(); advance(); // 跳过结尾的 ###
                            break;
                        }
                    } else {
                        advance();
                    }
                }
            } else {
                // 单行注释
                while (!isAtEnd() && peek() != '\n') {
                    advance();
                }
            }
        } else {
            break;
        }
    }
}

Token Lexer::makeToken(TokenType type, const std::string& val, int startLine, int startCol) {
    return Token{type, val, startLine, startCol, file};
}

Token Lexer::scanIdentifierOrKeyword() {
    int startLine = line;
    int startCol = column;
    std::string value;
    while (!isAtEnd() && isAlphaNumeric(peek())) {
        if (peek() == '_' && pos + 1 < source.size() && source[pos + 1] == '[') {
            break;
        }
        value += advance();
    }

    static const std::map<std::string, TokenType> keywords = {
        {"def", TokenType::KEYWORD_DEF},
        {"var", TokenType::KEYWORD_VAR},
        {"fun", TokenType::KEYWORD_FUN},
        {"mod", TokenType::KEYWORD_MOD},
        {"ass", TokenType::KEYWORD_ASS},
        {"ins", TokenType::KEYWORD_INS},
        {"defend", TokenType::KEYWORD_DEFEND},
        {"assend", TokenType::KEYWORD_ASSEND},
        {"modend", TokenType::KEYWORD_MODEND},
        {"if", TokenType::KEYWORD_IF},
        {"else", TokenType::KEYWORD_ELSE},
        {"ifend", TokenType::KEYWORD_IFEND},
        {"run", TokenType::KEYWORD_RUN},
        {"jump", TokenType::KEYWORD_JUMP},
        {"call", TokenType::KEYWORD_CALL},
        {"exit", TokenType::KEYWORD_EXIT},
        {"print", TokenType::KEYWORD_PRINT},
        {"input", TokenType::KEYWORD_INPUT},
        {"import", TokenType::KEYWORD_IMPORT},
        {"while", TokenType::KEYWORD_WHILE},
        {"whileend", TokenType::KEYWORD_WHILEEND},
        {"wait", TokenType::KEYWORD_WAIT},
        {"check_tip", TokenType::KEYWORD_CHECK_TIP},
        {"kill", TokenType::KEYWORD_KILL}
    };

    auto it = keywords.find(value);
    if (it != keywords.end()) {
        return Token{it->second, value, startLine, startCol, file};
    }
    return Token{TokenType::IDENTIFIER, value, startLine, startCol, file};
}

Token Lexer::scanNumber() {
    int startLine = line;
    int startCol = column;
    std::string value;
    bool isFloat = false;

    while (!isAtEnd() && isDigit(peek())) {
        value += advance();
    }

    if (!isAtEnd() && peek() == '.' && peekNext() != '\0' && isDigit(peekNext())) {
        isFloat = true;
        value += advance();
        while (!isAtEnd() && isDigit(peek())) {
            value += advance();
        }
    }

    if (isFloat) {
        return Token{TokenType::NUMBER_FLOAT, value, startLine, startCol, file};
    }
    return Token{TokenType::NUMBER_INT, value, startLine, startCol, file};
}

Token Lexer::scanString() {
    int startLine = line;
    int startCol = column;
    advance();
    std::string value;
    while (!isAtEnd() && peek() != '"') {
        if (peek() == '\\') {
            advance();
            if (!isAtEnd()) {
                char c = peek();
                if (c == 'n') value += '\n';
                else if (c == 't') value += '\t';
                else if (c == '\\') value += '\\';
                else if (c == '"') value += '"';
                else value += c;
                advance();
            }
        } else {
            value += advance();
        }
    }
    if (!isAtEnd()) advance();
    return Token{TokenType::STRING_LITERAL, value, startLine, startCol, file};
}

Token Lexer::scanToken() {
    skipWhitespaceAndComments();

    if (isAtEnd()) {
        return Token{TokenType::END_OF_FILE, "", line, column, file};
    }

    int startLine = line;
    int startCol = column;
    char c = peek();

    if (c == '\n') {
        advance();
        return Token{TokenType::NEWLINE, "\\n", startLine, startCol, file};
    }

    if (c == '"') {
        return scanString();
    }

    if (c == '_' && pos + 1 < source.size() && source[pos + 1] == '[') {
        advance();
        return makeToken(TokenType::UNDERSCORE, "_", startLine, startCol);
    }

    if (isAlpha(c)) {
        return scanIdentifierOrKeyword();
    }

    if (isDigit(c)) {
        return scanNumber();
    }

    // 识别 .x .xx .xxx 格式的包简化名
    if (c == '.' && pos + 1 < source.size() && isAlpha(source[pos + 1])) {
        advance();
        std::string value = ".";
        int letterCount = 0;
        while (!isAtEnd() && isAlpha(peek()) && letterCount < 3) {
            value += advance();
            letterCount++;
        }
        return makeToken(TokenType::DOT, value, startLine, startCol);
    }

    advance();

    switch (c) {
        case '+': return makeToken(TokenType::OPERATOR_PLUS, "+", startLine, startCol);
        case '-': return makeToken(TokenType::OPERATOR_MINUS, "-", startLine, startCol);
        case '*': return makeToken(TokenType::OPERATOR_STAR, "*", startLine, startCol);
        case '/': return makeToken(TokenType::OPERATOR_SLASH, "/", startLine, startCol);
        case '=': {
            if (!isAtEnd() && peek() == '=') {
                advance();
                return makeToken(TokenType::OPERATOR_EQ, "==", startLine, startCol);
            }
            if (!isAtEnd() && peek() == '<') {
                advance();
                return makeToken(TokenType::OPERATOR_LE_ALT, "=<", startLine, startCol);
            }
            return makeToken(TokenType::OPERATOR_ASSIGN, "=", startLine, startCol);
        }
        case '>': {
            if (!isAtEnd() && peek() == '=') {
                advance();
                return makeToken(TokenType::OPERATOR_GE, ">=", startLine, startCol);
            }
            return makeToken(TokenType::OPERATOR_GT, ">", startLine, startCol);
        }
        case '<': {
            if (!isAtEnd() && peek() == '=') {
                advance();
                return makeToken(TokenType::OPERATOR_LE, "<=", startLine, startCol);
            }
            return makeToken(TokenType::OPERATOR_LT, "<", startLine, startCol);
        }
        case '!': {
            if (!isAtEnd() && peek() == '=') {
                advance();
                return makeToken(TokenType::OPERATOR_NE, "!=", startLine, startCol);
            }
            throw SyntaxFormatError(file, startLine, startCol, "Unexpected character '!'");
        }
        case '(': return makeToken(TokenType::LPAREN, "(", startLine, startCol);
        case ')': return makeToken(TokenType::RPAREN, ")", startLine, startCol);
        case '[': return makeToken(TokenType::LBRACKET, "[", startLine, startCol);
        case ']': return makeToken(TokenType::RBRACKET, "]", startLine, startCol);
        case ',': return makeToken(TokenType::COMMA, ",", startLine, startCol);
        case ';': return makeToken(TokenType::SEMICOLON, ";", startLine, startCol);
        case ':': return makeToken(TokenType::COLON, ":", startLine, startCol);
        case '_': return makeToken(TokenType::UNDERSCORE, "_", startLine, startCol);
        case '@': return makeToken(TokenType::AT, "@", startLine, startCol);
        case '$': return makeToken(TokenType::DOLLAR, "$", startLine, startCol);
        default:
            throw SyntaxFormatError(file, startLine, startCol,
                std::string("Illegal character '") + c + "'");
    }
}

std::vector<Token> Lexer::tokenize() {
    std::vector<Token> tokens;
    while (!isAtEnd()) {
        Token tok = scanToken();
        if (tok.type == TokenType::END_OF_FILE) break;
        tokens.push_back(tok);
    }
    tokens.push_back(Token{TokenType::END_OF_FILE, "", line, column, file});
    return tokens;
}
