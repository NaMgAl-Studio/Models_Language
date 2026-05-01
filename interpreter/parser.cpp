#include "parser.h"
#include <algorithm>

Parser::Parser(const std::vector<Token>& tokens) : tokens(tokens), pos(0) {}

Token& Parser::cur() {
    return tokens[pos];
}

Token& Parser::peekAt(int offset) {
    size_t idx = pos + offset;
    if (idx >= tokens.size()) return tokens.back();
    return tokens[idx];
}

bool Parser::check(TokenType type) {
    return cur().type == type;
}

bool Parser::checkVal(const std::string& v) {
    return cur().value == v;
}

Token Parser::advance() {
    Token tok = cur();
    if (!isAtEnd()) pos++;
    return tok;
}

Token Parser::expect(TokenType type, const std::string& msg) {
    if (check(type)) return advance();
    throw SyntaxFormatError(cur().file, cur().line, cur().column, msg);
}

void Parser::skipNewlines() {
    while (check(TokenType::NEWLINE)) advance();
}

bool Parser::isAtEnd() {
    return cur().type == TokenType::END_OF_FILE;
}

VarType Parser::parseParamType() {
    if (check(TokenType::OPERATOR_MINUS)) {
        advance();
        if (checkVal("n")) {
            advance();
            return VarType::NUMBER;
        } else if (checkVal("w")) {
            advance();
            return VarType::STRING;
        } else if (checkVal("main")) {
            advance();
            return VarType::MIXED;
        } else {
            throw SyntaxFormatError(cur().file, cur().line, cur().column,
                "Expected 'n', 'w', or 'main' after '-'");
        }
    }
    return VarType::MIXED;
}

std::shared_ptr<ProgramNode> Parser::parse() {
    auto program = std::make_shared<ProgramNode>();
    program->type = ASTNodeType::PROGRAM;
    skipNewlines();

    while (!isAtEnd()) {
        skipNewlines();
        if (isAtEnd()) break;
        auto stmt = parseStatement();
        if (stmt) {
            if (stmt->type == ASTNodeType::DEF_MOD) {
                program->modules.push_back(stmt);
            } else {
                program->initCode.push_back(stmt);
            }
        }
        skipNewlines();
    }
    return program;
}

std::shared_ptr<ASTNode> Parser::parseStatement() {
    skipNewlines();
    if (isAtEnd()) return nullptr;

    if (check(TokenType::KEYWORD_DEF)) {
        advance();
        if (check(TokenType::KEYWORD_VAR)) return parseDefVar();
        if (check(TokenType::KEYWORD_FUN)) return parseDefFun();
        if (check(TokenType::KEYWORD_MOD)) return parseDefMod();
        throw SyntaxFormatError(cur().file, cur().line, cur().column,
            "Expected 'var', 'fun', or 'mod' after 'def'");
    }
    if (check(TokenType::KEYWORD_ASS)) {
        advance();
        if (check(TokenType::KEYWORD_VAR)) return parseAssVar();
        if (check(TokenType::KEYWORD_FUN)) return parseAssFun();
        throw SyntaxFormatError(cur().file, cur().line, cur().column,
            "Expected 'var' or 'fun' after 'ass'");
    }
    if (check(TokenType::KEYWORD_INS)) { advance(); return parseInsFun(); }
    if (check(TokenType::KEYWORD_IF)) { advance(); return parseIf(); }
    if (check(TokenType::KEYWORD_PRINT)) { advance(); return parsePrint(); }
    if (check(TokenType::KEYWORD_INPUT)) { advance(); return parseInput(); }
    if (check(TokenType::KEYWORD_JUMP)) { advance(); return parseJump(); }
    if (check(TokenType::KEYWORD_CALL)) { advance(); return parseCall(); }
    if (check(TokenType::KEYWORD_RUN)) { advance(); return parseRun(); }
    if (check(TokenType::KEYWORD_IMPORT)) { advance(); return parseImport(); }
    if (check(TokenType::DOT)) { return parsePackageCall(); }
    if (check(TokenType::KEYWORD_EXIT)) { advance(); return parseExit(); }
    if (check(TokenType::KEYWORD_WHILE)) { advance(); return parseWhile(); }
    if (check(TokenType::KEYWORD_WAIT)) { advance(); return parseWait(); }
    if (check(TokenType::KEYWORD_CHECK_TIP)) { advance(); return parseCheckTip(); }
    if (check(TokenType::KEYWORD_KILL)) { advance(); return parseKill(); }

    throw SyntaxFormatError(cur().file, cur().line, cur().column,
        "Unexpected token '" + cur().value + "'");
}

std::shared_ptr<DefVarNode> Parser::parseDefVar() {
    auto node = std::make_shared<DefVarNode>();
    node->type = ASTNodeType::DEF_VAR;
    expect(TokenType::KEYWORD_VAR, "Expected 'var'");
    node->line = cur().line; node->column = cur().column; node->file = cur().file;

    if (!check(TokenType::IDENTIFIER))
        throw SyntaxFormatError(cur().file, cur().line, cur().column, "Expected variable name");
    node->name = advance().value;

    if (check(TokenType::UNDERSCORE)) {
        advance();
        node->isMatrix = true;
        node->matrixIdx = parseMatrixIndex();
    }

    VarType vt = parseParamType();
    if (vt == VarType::MIXED && !checkVal("main")) {
        throw MissingParameterError(cur().file, cur().line, cur().column,
            "Missing parameter -n or -w in def var");
    }
    node->varType = (vt == VarType::MIXED) ? VarType::NUMBER : vt;
    return node;
}

std::shared_ptr<AssVarNode> Parser::parseAssVar() {
    auto node = std::make_shared<AssVarNode>();
    node->type = ASTNodeType::ASS_VAR;
    expect(TokenType::KEYWORD_VAR, "Expected 'var'");
    node->line = cur().line; node->column = cur().column; node->file = cur().file;

    if (!check(TokenType::IDENTIFIER))
        throw SyntaxFormatError(cur().file, cur().line, cur().column, "Expected variable name");
    node->name = advance().value;

    if (check(TokenType::UNDERSCORE)) {
        advance();
        node->matrixIdx = parseMatrixIndex();
        node->isMatrixAssign = true;
        bool hasRange = false, hasSingle = false;
        for (auto& d : node->matrixIdx.dims) {
            if (d.isRange) hasRange = true; else hasSingle = true;
        }
        node->isIndexAssign = hasSingle;
        node->isRangeAssign = hasRange;
    }

    expect(TokenType::OPERATOR_ASSIGN, "Expected '=' in ass var");

    if (check(TokenType::IDENTIFIER)) {
        size_t savePos = pos;
        Token nextTok = advance();
        if (check(TokenType::UNDERSCORE)) {
            pos = savePos;
            node->hasSourceMatrix = true;
            node->sourceMatrixName = advance().value;
            if (check(TokenType::UNDERSCORE)) {
                advance();
                node->sourceMatrixIdx = parseMatrixIndex();
            }
            return node;
        }
        pos = savePos;
    }

    node->value = parseExpression();
    return node;
}

std::shared_ptr<DefFunNode> Parser::parseDefFun() {
    auto node = std::make_shared<DefFunNode>();
    node->type = ASTNodeType::DEF_FUN;
    expect(TokenType::KEYWORD_FUN, "Expected 'fun'");
    node->line = cur().line; node->column = cur().column; node->file = cur().file;

    if (!check(TokenType::IDENTIFIER))
        throw SyntaxFormatError(cur().file, cur().line, cur().column, "Expected function type name");
    node->name = advance().value;

    if (check(TokenType::OPERATOR_MINUS)) {
        VarType vt = parseParamType();
        if (vt != VarType::MIXED) node->funType = vt;
        else node->funType = VarType::MIXED;
    } else {
        node->funType = VarType::MIXED;
    }

    skipNewlines();
    while (!checkVal("defend") && !isAtEnd()) {
        skipNewlines();
        if (checkVal("defend")) break;
        if (check(TokenType::KEYWORD_DEF)) {
            advance();
            FunInternalDef idef;
            if (!check(TokenType::IDENTIFIER))
                throw SyntaxFormatError(cur().file, cur().line, cur().column, "Expected identifier in fun def");
            idef.name = advance().value;
            expect(TokenType::OPERATOR_ASSIGN, "Expected '=' in fun def");

            if (check(TokenType::IDENTIFIER) && (cur().value == "SUM" || cur().value == "AVERAGE")) {
                std::string fname = advance().value;
                if (check(TokenType::LPAREN)) {
                    advance();
                    idef.isWPS = true;
                    idef.wpsFuncName = fname;
                    idef.wpsArg = parseExpression();
                    expect(TokenType::RPAREN, "Expected ')' after WPS function arg");
                } else {
                    idef.expr = std::make_shared<VarRefNode>();
                    idef.expr->type = ASTNodeType::VAR_REF;
                    auto vr = std::static_pointer_cast<VarRefNode>(idef.expr);
                    vr->name = fname;
                }
            } else {
                idef.expr = parseExpression();
            }
            node->internalDefs.push_back(idef);
        }
        skipNewlines();
    }

    if (!checkVal("defend"))
        throw MismatchedEndMarkerError(cur().file, cur().line, cur().column, "Expected 'defend'");
    advance();
    return node;
}

std::shared_ptr<InsFunNode> Parser::parseInsFun() {
    auto node = std::make_shared<InsFunNode>();
    node->type = ASTNodeType::INS_FUN;
    node->line = cur().line; node->column = cur().column; node->file = cur().file;

    if (!check(TokenType::IDENTIFIER))
        throw SyntaxFormatError(cur().file, cur().line, cur().column, "Expected function type name");
    node->typeName = advance().value;

    if (!check(TokenType::IDENTIFIER))
        throw SyntaxFormatError(cur().file, cur().line, cur().column, "Expected instance name");
    node->instanceName = advance().value;

    if (check(TokenType::UNDERSCORE)) {
        advance();
        node->isMatrix = true;
        node->instanceMatrixIdx = parseMatrixIndex();
    }
    return node;
}

std::shared_ptr<AssFunNode> Parser::parseAssFun() {
    auto node = std::make_shared<AssFunNode>();
    node->type = ASTNodeType::ASS_FUN;
    expect(TokenType::KEYWORD_FUN, "Expected 'fun'");
    node->line = cur().line; node->column = cur().column; node->file = cur().file;

    if (!check(TokenType::IDENTIFIER))
        throw SyntaxFormatError(cur().file, cur().line, cur().column, "Expected function instance name");
    node->instanceName = advance().value;

    if (check(TokenType::UNDERSCORE)) {
        advance();
        node->isInstanceMatrix = true;
        node->instanceMatrixIdx = parseMatrixIndex();
    }

    skipNewlines();
    while (!checkVal("assend") && !isAtEnd()) {
        skipNewlines();
        if (checkVal("assend")) break;

        FunAssignItem item;
        if (!check(TokenType::IDENTIFIER))
            throw SyntaxFormatError(cur().file, cur().line, cur().column, "Expected identifier in ass fun");
        item.names.push_back(advance().value);

        while (check(TokenType::COMMA)) {
            advance();
            if (!check(TokenType::IDENTIFIER))
                throw SyntaxFormatError(cur().file, cur().line, cur().column, "Expected identifier after comma");
            item.names.push_back(advance().value);
        }

        expect(TokenType::OPERATOR_ASSIGN, "Expected '=' in ass fun item");

        if (check(TokenType::IDENTIFIER)) {
            size_t savePos = pos;
            Token valTok = advance();
            bool isArith = check(TokenType::OPERATOR_PLUS) || check(TokenType::OPERATOR_MINUS) ||
                           check(TokenType::OPERATOR_STAR) || check(TokenType::OPERATOR_SLASH) ||
                           check(TokenType::OPERATOR_GT) || check(TokenType::OPERATOR_LT) ||
                           check(TokenType::OPERATOR_GE) || check(TokenType::OPERATOR_LE) ||
                           check(TokenType::OPERATOR_EQ) || check(TokenType::OPERATOR_NE);
            if (isArith) {
                pos = savePos;
                item.value = parseExpression();
            } else {
                pos = savePos;
                item.isLink = true;
                item.linkVarName = advance().value;
                item.value = std::make_shared<VarRefNode>();
                item.value->type = ASTNodeType::VAR_REF;
                auto vr = std::static_pointer_cast<VarRefNode>(item.value);
                vr->name = item.linkVarName;
            }
        } else {
            item.value = parseExpression();
        }

        node->assignments.push_back(item);
        skipNewlines();
    }

    if (!checkVal("assend"))
        throw MismatchedEndMarkerError(cur().file, cur().line, cur().column, "Expected 'assend'");
    advance();
    return node;
}

std::shared_ptr<DefModNode> Parser::parseDefMod() {
    auto node = std::make_shared<DefModNode>();
    node->type = ASTNodeType::DEF_MOD;
    expect(TokenType::KEYWORD_MOD, "Expected 'mod'");
    node->line = cur().line; node->column = cur().column; node->file = cur().file;

    if (!check(TokenType::IDENTIFIER))
        throw SyntaxFormatError(cur().file, cur().line, cur().column, "Expected module name");
    node->name = advance().value;

    if (check(TokenType::OPERATOR_MINUS) && peekAt(1).value == "main") {
        advance(); advance();
        node->isMain = true;
    }

    skipNewlines();
    while (!checkVal("modend") && !isAtEnd()) {
        skipNewlines();
        if (checkVal("modend")) break;
        auto stmt = parseStatement();
        if (stmt) node->body.push_back(stmt);
        skipNewlines();
    }

    if (!checkVal("modend"))
        throw MismatchedEndMarkerError(cur().file, cur().line, cur().column, "Expected 'modend'");
    advance();
    return node;
}

ConditionGroup Parser::parseCondition() {
    ConditionGroup group;
    expect(TokenType::LPAREN, "Expected '(' in condition");

    std::vector<std::shared_ptr<ASTNode>> currentAnd;

    while (!check(TokenType::RPAREN) && !isAtEnd()) {
        currentAnd.push_back(parseExpression());

        if (check(TokenType::COMMA)) {
            advance();
        } else if (check(TokenType::SEMICOLON)) {
            advance();
            group.orGroups.push_back(currentAnd);
            currentAnd.clear();
        }
    }
    if (!currentAnd.empty()) group.orGroups.push_back(currentAnd);

    expect(TokenType::RPAREN, "Expected ')' after condition");
    return group;
}

std::shared_ptr<IfStmtNode> Parser::parseIf() {
    auto node = std::make_shared<IfStmtNode>();
    node->type = ASTNodeType::IF_STMT;
    node->line = cur().line; node->column = cur().column; node->file = cur().file;

    node->condition = parseCondition();
    skipNewlines();

    while (!checkVal("else") && !checkVal("ifend") && !isAtEnd()) {
        skipNewlines();
        if (checkVal("else") || checkVal("ifend")) break;
        auto stmt = parseStatement();
        if (stmt) node->thenBlock.push_back(stmt);
        skipNewlines();
    }

    while (checkVal("else")) {
        advance();
        if (check(TokenType::KEYWORD_IF)) {
            advance();
            ConditionGroup elifCond = parseCondition();
            skipNewlines();
            std::vector<std::shared_ptr<ASTNode>> elifBlock;
            while (!checkVal("else") && !checkVal("ifend") && !isAtEnd()) {
                skipNewlines();
                if (checkVal("else") || checkVal("ifend")) break;
                auto stmt = parseStatement();
                if (stmt) elifBlock.push_back(stmt);
                skipNewlines();
            }
            node->elseIfBlocks.emplace_back(elifCond, elifBlock);
        } else {
            skipNewlines();
            while (!checkVal("ifend") && !isAtEnd()) {
                skipNewlines();
                if (checkVal("ifend")) break;
                auto stmt = parseStatement();
                if (stmt) node->elseBlock.push_back(stmt);
                skipNewlines();
            }
            break;
        }
    }

    if (!checkVal("ifend"))
        throw MismatchedEndMarkerError(cur().file, cur().line, cur().column, "Expected 'ifend'");
    advance();
    return node;
}

bool Parser::isFunValueRef() {
    size_t savePos = pos;
    if (!check(TokenType::IDENTIFIER)) return false;
    pos++;
    while (pos < tokens.size() && check(TokenType::UNDERSCORE)) {
        pos++;
        if (pos < tokens.size() && check(TokenType::LBRACKET)) {
            pos++;
            int depth = 1;
            while (depth > 0 && pos < tokens.size()) {
                if (tokens[pos].type == TokenType::LBRACKET) depth++;
                if (tokens[pos].type == TokenType::RBRACKET) depth--;
                pos++;
            }
        }
    }
    bool result = (pos < tokens.size() && tokens[pos].type == TokenType::COLON);
    pos = savePos;
    return result;
}

std::shared_ptr<ASTNode> Parser::parseFunValueRef() {
    auto node = std::make_shared<FunValueRefNode>();
    node->type = ASTNodeType::FUN_VALUE_REF;
    node->line = cur().line; node->column = cur().column; node->file = cur().file;

    node->instanceName = advance().value;
    if (check(TokenType::UNDERSCORE)) {
        advance();
        node->isInstanceMatrix = true;
        node->instanceMatrixIdx = parseMatrixIndex();
    }
    expect(TokenType::COLON, "Expected ':' in function value reference");
    if (!check(TokenType::IDENTIFIER))
        throw SyntaxFormatError(cur().file, cur().line, cur().column, "Expected value name after ':'");
    node->valueName = advance().value;
    return node;
}

std::shared_ptr<PrintStmtNode> Parser::parsePrint() {
    auto node = std::make_shared<PrintStmtNode>();
    node->type = ASTNodeType::PRINT_STMT;
    node->line = cur().line; node->column = cur().column; node->file = cur().file;

    expect(TokenType::LPAREN, "Expected '(' after 'print'");

    if (isFunValueRef()) {
        auto ref = std::static_pointer_cast<FunValueRefNode>(parseFunValueRef());
        node->isFunMatrixPrint = true;
        node->funInstanceName = ref->instanceName;
        node->funInstanceMatrixIdx = ref->instanceMatrixIdx;
        node->funValueName = ref->valueName;
        node->expr = ref;
        expect(TokenType::RPAREN, "Expected ')' after print argument");
        return node;
    }

    if (check(TokenType::IDENTIFIER)) {
        size_t savePos = pos;
        Token nameTok = advance();
        if (check(TokenType::UNDERSCORE)) {
            advance();
            MatrixIndex idx = parseMatrixIndex();
            bool hasRange = false;
            for (auto& d : idx.dims) if (d.isRange) { hasRange = true; break; }
            if (hasRange) {
                node->isMatrixPrint = true;
                node->matrixName = nameTok.value;
                node->matrixIdx = idx;
                node->expr = std::make_shared<VarRefNode>();
                node->expr->type = ASTNodeType::VAR_REF;
                auto vr = std::static_pointer_cast<VarRefNode>(node->expr);
                vr->name = nameTok.value;
                vr->matrixIdx = idx;
                vr->isMatrix = true;
                expect(TokenType::RPAREN, "Expected ')' after print argument");
                return node;
            }
        }
        pos = savePos;
    }

    node->expr = parseExpression();
    node->exprs.push_back(node->expr);
    while (check(TokenType::COMMA)) {
        advance();
        auto nextExpr = parseExpression();
        node->exprs.push_back(nextExpr);
    }
    expect(TokenType::RPAREN, "Expected ')' after print argument");
    return node;
}

std::shared_ptr<InputStmtNode> Parser::parseInput() {
    auto node = std::make_shared<InputStmtNode>();
    node->type = ASTNodeType::INPUT_STMT;
    node->line = cur().line; node->column = cur().column; node->file = cur().file;

    if (!check(TokenType::IDENTIFIER))
        throw SyntaxFormatError(cur().file, cur().line, cur().column, "Expected variable name after 'input'");
    node->varName = advance().value;

    if (check(TokenType::LPAREN)) {
        advance();
        if (!check(TokenType::STRING_LITERAL))
            throw SyntaxFormatError(cur().file, cur().line, cur().column, "Expected string literal as prompt");
        node->prompt = advance().value;
        expect(TokenType::RPAREN, "Expected ')' after input prompt");
    }

    if (check(TokenType::OPERATOR_MINUS)) {
        VarType vt = parseParamType();
        node->inputType = vt;
    }
    return node;
}

std::shared_ptr<JumpStmtNode> Parser::parseJump() {
    auto node = std::make_shared<JumpStmtNode>();
    node->type = ASTNodeType::JUMP_STMT;
    node->line = cur().line; node->column = cur().column; node->file = cur().file;

    if (check(TokenType::AT)) {
        advance();
        node->isLocal = true;
    } else if (check(TokenType::IDENTIFIER)) {
        node->fileName = cur().value;
        advance();
        node->isLocal = false;
    } else {
        throw SyntaxFormatError(cur().file, cur().line, cur().column, "Expected '@' or file name after 'jump'");
    }

    if (!check(TokenType::IDENTIFIER))
        throw SyntaxFormatError(cur().file, cur().line, cur().column, "Expected module name in jump");
    node->moduleName = advance().value;
    return node;
}

std::shared_ptr<CallStmtNode> Parser::parseCall() {
    auto node = std::make_shared<CallStmtNode>();
    node->type = ASTNodeType::CALL_STMT;
    node->line = cur().line; node->column = cur().column; node->file = cur().file;

    if (check(TokenType::AT)) {
        advance();
        node->isLocal = true;
    } else if (check(TokenType::IDENTIFIER)) {
        node->fileName = cur().value;
        advance();
        node->isLocal = false;
    } else {
        throw SyntaxFormatError(cur().file, cur().line, cur().column, "Expected '@' or file name after 'call'");
    }

    if (!check(TokenType::IDENTIFIER))
        throw SyntaxFormatError(cur().file, cur().line, cur().column, "Expected module name in call");
    node->moduleName = advance().value;

    // 检查是否有分组语法 ; <组名>
    if (check(TokenType::SEMICOLON)) {
        advance(); // skip ';'
        if (check(TokenType::IDENTIFIER)) {
            node->groupName = cur().value;
            advance(); // skip group name
        }
    }

    return node;
}

std::shared_ptr<RunStmtNode> Parser::parseRun() {
    auto node = std::make_shared<RunStmtNode>();
    node->type = ASTNodeType::RUN_STMT;
    node->line = cur().line; node->column = cur().column; node->file = cur().file;

    if (!check(TokenType::STRING_LITERAL))
        throw SyntaxFormatError(cur().file, cur().line, cur().column, "Expected string literal after 'run'");
    node->path = advance().value;
    if (node->path.size() >= 2 && node->path[0] == '\\' && node->path[1] == '\\') {
        node->isRelative = true;
    }
    return node;
}

std::shared_ptr<ASTNode> Parser::parseWhile() {
    auto node = std::make_shared<WhileStmtNode>();
    node->line = cur().line; node->column = cur().column; node->file = cur().file;

    // 检查是否有条件
    if (check(TokenType::LPAREN)) {
        advance(); // skip '('
        // 解析条件，与 if 语句相同的条件解析逻辑
        while (!check(TokenType::RPAREN)) {
            node->condition.push_back(parseExpression());
            if (check(TokenType::COMMA) || check(TokenType::SEMICOLON)) {
                advance(); // skip ',' or ';'
            } else {
                break;
            }
        }
        advance(); // skip ')'
    }

    // 解析循环体
    while (!check(TokenType::KEYWORD_WHILEEND) && !isAtEnd()) {
        skipNewlines();
        if (check(TokenType::KEYWORD_WHILEEND)) break;
        auto stmt = parseStatement();
        if (stmt) node->body.push_back(stmt);
        skipNewlines();
    }

    if (!check(TokenType::KEYWORD_WHILEEND))
        throw MismatchedEndMarkerError(cur().file, cur().line, cur().column, "Expected 'whileend'");
    advance(); // skip 'whileend'
    return node;
}

std::shared_ptr<ASTNode> Parser::parseWait() {
    auto node = std::make_shared<WaitStmtNode>();
    node->line = cur().line; node->column = cur().column; node->file = cur().file;

    if (!check(TokenType::LPAREN)) {
        throw SyntaxFormatError(cur().file, cur().line, cur().column,
                         "Expected '(' after 'wait'");
    }
    advance(); // skip '('

    // 判断是定时等待还是检查点等待
    if (check(TokenType::NUMBER_INT) || check(TokenType::NUMBER_FLOAT)) {
        // 定时等待
        node->isTimedWait = true;
        node->waitSeconds = std::stod(cur().value);
        advance(); // skip number
    } else if (check(TokenType::IDENTIFIER)) {
        // 检查点等待
        node->isTimedWait = false;
        node->checkPointName = cur().value;
        advance(); // skip identifier
    } else {
        throw SyntaxFormatError(cur().file, cur().line, cur().column,
                         "Expected number or checkpoint name in wait()");
    }

    if (!check(TokenType::RPAREN)) {
        throw SyntaxFormatError(cur().file, cur().line, cur().column,
                         "Expected ')' after wait argument");
    }
    advance(); // skip ')'

    return node;
}

std::shared_ptr<ASTNode> Parser::parseCheckTip() {
    auto node = std::make_shared<CheckTipStmtNode>();
    node->line = cur().line; node->column = cur().column; node->file = cur().file;

    if (!check(TokenType::IDENTIFIER)) {
        throw SyntaxFormatError(cur().file, cur().line, cur().column,
                         "Expected checkpoint name after 'check_tip'");
    }
    node->checkPointName = cur().value;
    advance(); // skip identifier

    return node;
}

std::shared_ptr<ASTNode> Parser::parseKill() {
    auto node = std::make_shared<KillStmtNode>();
    node->line = cur().line; node->column = cur().column; node->file = cur().file;

    // kill 不带参数 - 终止所有其他进程
    if (check(TokenType::END_OF_FILE) || check(TokenType::NEWLINE)) {
        node->killAll = true;
        return node;
    }

    // kill N/A - 终止未分组进程
    if (check(TokenType::IDENTIFIER) && cur().value == "N") {
        advance(); // skip 'N'
        if (check(TokenType::OPERATOR_SLASH)) {
            advance(); // skip '/'
            if (check(TokenType::IDENTIFIER) && cur().value == "A") {
                advance(); // skip 'A'
                node->killUnassigned = true;
                return node;
            }
        }
        throw SyntaxFormatError(cur().file, cur().line, cur().column,
                         "Expected 'N/A' after 'kill'");
    }

    // kill <组名> 或 kill <组1,组2,...>
    if (check(TokenType::IDENTIFIER)) {
        node->groupNames.push_back(cur().value);
        advance(); // skip first group name

        while (check(TokenType::COMMA)) {
            advance(); // skip ','
            if (!check(TokenType::IDENTIFIER)) {
                throw SyntaxFormatError(cur().file, cur().line, cur().column,
                                 "Expected group name after ',' in kill statement");
            }
            node->groupNames.push_back(cur().value);
            advance();
        }
        return node;
    }

    // 默认为 kill all
    node->killAll = true;
    return node;
}

std::shared_ptr<ExitStmtNode> Parser::parseExit() {
    auto node = std::make_shared<ExitStmtNode>();
    node->type = ASTNodeType::EXIT_STMT;
    node->line = cur().line; node->column = cur().column; node->file = cur().file;
    node->exitType = EXIT_ALL; // default

    if (check(TokenType::LPAREN)) {
        advance(); // skip '('

        if (check(TokenType::NUMBER_INT) && cur().value == "0") {
            node->exitType = EXIT_ALL;
            advance(); // skip '0'
        } else if (check(TokenType::NUMBER_INT) && cur().value == "1") {
            node->exitType = EXIT_CURRENT;
            advance(); // skip '1'
        } else if (check(TokenType::KEYWORD_WHILE)) {
            node->exitType = EXIT_WHILE;
            advance(); // skip 'while'
        } else {
            // exit() without argument = exit(0)
            node->exitType = EXIT_ALL;
        }

        if (check(TokenType::RPAREN)) {
            advance(); // skip ')'
        }
    }

    return node;
}

MatrixIndex Parser::parseMatrixIndex() {
    MatrixIndex idx;
    while (true) {
        expect(TokenType::LBRACKET, "Expected '[' in matrix index");
        MatrixDimension dim;
        if (check(TokenType::IDENTIFIER)) {
            dim.startIsVar = true;
            dim.isVarIndexed = true;
            dim.startVarName = advance().value;
        } else if (check(TokenType::NUMBER_INT)) {
            dim.startIndex = std::stoi(advance().value);
        } else {
            throw SyntaxFormatError(cur().file, cur().line, cur().column,
                "Expected number or variable in matrix index");
        }

        if (check(TokenType::COLON)) {
            advance();
            dim.isRange = true;
            if (check(TokenType::IDENTIFIER)) {
                dim.endIsVar = true;
                dim.isVarIndexed = true;
                dim.endVarName = advance().value;
            } else if (check(TokenType::NUMBER_INT)) {
                dim.endIndex = std::stoi(advance().value);
            } else {
                throw SyntaxFormatError(cur().file, cur().line, cur().column,
                    "Expected number or variable after ':' in matrix range");
            }
        }
        idx.dims.push_back(dim);
        expect(TokenType::RBRACKET, "Expected ']' in matrix index");
        if (check(TokenType::UNDERSCORE)) advance(); else break;
    }
    return idx;
}

std::shared_ptr<ASTNode> Parser::parseExpression() {
    return parseComparison();
}

std::shared_ptr<ASTNode> Parser::parseComparison() {
    auto left = parseAddSub();
    while (check(TokenType::OPERATOR_GT) || check(TokenType::OPERATOR_LT) ||
           check(TokenType::OPERATOR_GE) || check(TokenType::OPERATOR_LE) ||
           check(TokenType::OPERATOR_LE_ALT) ||
           check(TokenType::OPERATOR_EQ) || check(TokenType::OPERATOR_NE)) {
        auto node = std::make_shared<BinaryExprNode>();
        node->type = ASTNodeType::BINARY_EXPR;
        node->line = cur().line; node->column = cur().column; node->file = cur().file;
        node->op = advance().value;
        node->left = left;
        node->right = parseAddSub();
        left = node;
    }
    return left;
}

std::shared_ptr<ASTNode> Parser::parseAddSub() {
    auto left = parseMulDiv();
    while (check(TokenType::OPERATOR_PLUS) || check(TokenType::OPERATOR_MINUS)) {
        auto node = std::make_shared<BinaryExprNode>();
        node->type = ASTNodeType::BINARY_EXPR;
        node->line = cur().line; node->column = cur().column; node->file = cur().file;
        node->op = advance().value;
        node->left = left;
        node->right = parseMulDiv();
        left = node;
    }
    return left;
}

std::shared_ptr<ASTNode> Parser::parseMulDiv() {
    auto left = parseUnary();
    while (check(TokenType::OPERATOR_STAR) || check(TokenType::OPERATOR_SLASH)) {
        auto node = std::make_shared<BinaryExprNode>();
        node->type = ASTNodeType::BINARY_EXPR;
        node->line = cur().line; node->column = cur().column; node->file = cur().file;
        node->op = advance().value;
        node->left = left;
        node->right = parseUnary();
        left = node;
    }
    return left;
}

std::shared_ptr<ASTNode> Parser::parseUnary() {
    if (check(TokenType::OPERATOR_MINUS)) {
        auto node = std::make_shared<UnaryExprNode>();
        node->type = ASTNodeType::UNARY_EXPR;
        node->line = cur().line; node->column = cur().column; node->file = cur().file;
        node->op = advance().value;
        node->operand = parsePrimary();
        return node;
    }
    return parsePrimary();
}

std::shared_ptr<ASTNode> Parser::parsePrimary() {
    if (check(TokenType::NUMBER_INT)) {
        auto node = std::make_shared<LiteralNode>();
        node->type = ASTNodeType::LITERAL;
        node->line = cur().line; node->column = cur().column; node->file = cur().file;
        node->litType = LiteralNode::INT_VAL;
        node->intVal = std::stoll(advance().value);
        return node;
    }
    if (check(TokenType::NUMBER_FLOAT)) {
        auto node = std::make_shared<LiteralNode>();
        node->type = ASTNodeType::LITERAL;
        node->line = cur().line; node->column = cur().column; node->file = cur().file;
        node->litType = LiteralNode::FLOAT_VAL;
        node->floatVal = std::stod(advance().value);
        return node;
    }
    if (check(TokenType::STRING_LITERAL)) {
        auto node = std::make_shared<LiteralNode>();
        node->type = ASTNodeType::LITERAL;
        node->line = cur().line; node->column = cur().column; node->file = cur().file;
        node->litType = LiteralNode::STR_VAL;
        node->strVal = advance().value;
        return node;
    }
    if (check(TokenType::LPAREN)) {
        advance();
        auto expr = parseExpression();
        expect(TokenType::RPAREN, "Expected ')' after expression");
        return expr;
    }
    if (check(TokenType::IDENTIFIER)) {
        std::string val = cur().value;
        if (val == "str" || val == "int" || val == "float" || val == "len") {
            auto node = std::make_shared<TypeFuncNode>();
            node->type = ASTNodeType::TYPE_FUNC;
            node->line = cur().line; node->column = cur().column; node->file = cur().file;
            node->funcName = advance().value;
            expect(TokenType::LPAREN, "Expected '(' after type function");
            node->arg = parseExpression();
            expect(TokenType::RPAREN, "Expected ')' after type function argument");
            return node;
        }
        if (val == "SUM" || val == "AVERAGE") {
            auto node = std::make_shared<WPSFuncNode>();
            node->type = ASTNodeType::WPS_FUNC;
            node->line = cur().line; node->column = cur().column; node->file = cur().file;
            node->funcName = advance().value;
            expect(TokenType::LPAREN, "Expected '(' after WPS function");
            node->arg = parseExpression();
            expect(TokenType::RPAREN, "Expected ')' after WPS function argument");
            return node;
        }
        if (isFunValueRef()) return parseFunValueRef();

        auto node = std::make_shared<VarRefNode>();
        node->type = ASTNodeType::VAR_REF;
        node->line = cur().line; node->column = cur().column; node->file = cur().file;
        node->name = advance().value;
        if (check(TokenType::UNDERSCORE)) {
            advance();
            node->isMatrix = true;
            node->matrixIdx = parseMatrixIndex();
        }
        return node;
    }

    throw SyntaxFormatError(cur().file, cur().line, cur().column,
        "Unexpected token '" + cur().value + "' in expression");
}

// 解析import语句
std::shared_ptr<ImportStmtNode> Parser::parseImport() {
    auto node = std::make_shared<ImportStmtNode>();
    node->type = ASTNodeType::IMPORT_STMT;
    node->line = cur().line; node->column = cur().column; node->file = cur().file;

    if (!check(TokenType::IDENTIFIER))
        throw SyntaxFormatError(cur().file, cur().line, cur().column, "Expected package name after 'import'");
    node->packageName = advance().value;

    if (!check(TokenType::DOT))
        throw SyntaxFormatError(cur().file, cur().line, cur().column, "Expected alias (e.g. .m) after package name");
    node->alias = advance().value;

    return node;
}

// 解析包调用语句
std::shared_ptr<PackageCallNode> Parser::parsePackageCall() {
    auto node = std::make_shared<PackageCallNode>();
    node->type = ASTNodeType::PACKAGE_CALL;
    node->line = cur().line; node->column = cur().column; node->file = cur().file;

    if (!check(TokenType::DOT))
        throw SyntaxFormatError(cur().file, cur().line, cur().column, "Expected package alias");
    node->alias = advance().value;

    if (check(TokenType::COLON)) {
        advance();
    }

    if (!check(TokenType::IDENTIFIER))
        throw SyntaxFormatError(cur().file, cur().line, cur().column, "Expected function name after alias");
    node->funcName = advance().value;

    // 解析可选的参数
    if (check(TokenType::LPAREN)) {
        advance();
        if (!check(TokenType::RPAREN)) {
            node->args.push_back(parseExpression());
            while (check(TokenType::COMMA)) {
                advance();
                node->args.push_back(parseExpression());
            }
        }
        expect(TokenType::RPAREN, "Expected ')' after package call arguments");
    } else if (check(TokenType::STRING_LITERAL)) {
        // 支持无括号参数: .a base64 "hello"
        auto lit = std::make_shared<LiteralNode>();
        lit->type = ASTNodeType::LITERAL;
        lit->line = cur().line; lit->column = cur().column; lit->file = cur().file;
        lit->litType = LiteralNode::STR_VAL;
        lit->strVal = advance().value;
        node->args.push_back(lit);
    }

    return node;
}
