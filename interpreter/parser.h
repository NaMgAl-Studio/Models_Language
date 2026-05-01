#pragma once

#include <vector>
#include <memory>
#include <string>
#include "common.h"
#include "error.h"

class Parser {
public:
    Parser(const std::vector<Token>& tokens);
    std::shared_ptr<ProgramNode> parse();

private:
    std::vector<Token> tokens;
    size_t pos;

    Token& cur();
    Token& peekAt(int offset);
    bool check(TokenType type);
    bool checkVal(const std::string& v);
    Token advance();
    Token expect(TokenType type, const std::string& msg);
    void skipNewlines();
    bool isAtEnd();

    std::shared_ptr<ASTNode> parseStatement();
    std::shared_ptr<DefVarNode> parseDefVar();
    std::shared_ptr<AssVarNode> parseAssVar();
    std::shared_ptr<DefFunNode> parseDefFun();
    std::shared_ptr<InsFunNode> parseInsFun();
    std::shared_ptr<AssFunNode> parseAssFun();
    std::shared_ptr<DefModNode> parseDefMod();
    std::shared_ptr<IfStmtNode> parseIf();
    std::shared_ptr<PrintStmtNode> parsePrint();
    std::shared_ptr<InputStmtNode> parseInput();
    std::shared_ptr<JumpStmtNode> parseJump();
    std::shared_ptr<CallStmtNode> parseCall();
    std::shared_ptr<RunStmtNode> parseRun();
    std::shared_ptr<ExitStmtNode> parseExit();
    std::shared_ptr<ASTNode> parseWhile();
    std::shared_ptr<ASTNode> parseWait();
    std::shared_ptr<ASTNode> parseCheckTip();
    std::shared_ptr<ASTNode> parseKill();
    std::shared_ptr<ImportStmtNode> parseImport();
    std::shared_ptr<PackageCallNode> parsePackageCall();

    std::shared_ptr<ASTNode> parseExpression();
    std::shared_ptr<ASTNode> parseComparison();
    std::shared_ptr<ASTNode> parseAddSub();
    std::shared_ptr<ASTNode> parseMulDiv();
    std::shared_ptr<ASTNode> parseUnary();
    std::shared_ptr<ASTNode> parsePrimary();

    MatrixIndex parseMatrixIndex();
    ConditionGroup parseCondition();

    bool isFunValueRef();
    std::shared_ptr<ASTNode> parseFunValueRef();

    VarType parseParamType();
};
