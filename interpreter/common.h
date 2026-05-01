#pragma once

#include <string>
#include <vector>
#include <memory>
#include <map>
#include <functional>

enum class TokenType {
    KEYWORD_DEF,
    KEYWORD_VAR,
    KEYWORD_FUN,
    KEYWORD_MOD,
    KEYWORD_ASS,
    KEYWORD_INS,
    KEYWORD_DEFEND,
    KEYWORD_ASSEND,
    KEYWORD_MODEND,
    KEYWORD_IF,
    KEYWORD_ELSE,
    KEYWORD_IFEND,
    KEYWORD_RUN,
    KEYWORD_JUMP,
    KEYWORD_CALL,
    KEYWORD_EXIT,
    KEYWORD_WHILE,
    KEYWORD_WHILEEND,
    KEYWORD_WAIT,
    KEYWORD_CHECK_TIP,
    KEYWORD_KILL,
    KEYWORD_PRINT,
    KEYWORD_INPUT,
    KEYWORD_IMPORT,
    IDENTIFIER,
    NUMBER_INT,
    NUMBER_FLOAT,
    STRING_LITERAL,
    OPERATOR_PLUS,
    OPERATOR_MINUS,
    OPERATOR_STAR,
    OPERATOR_SLASH,
    OPERATOR_ASSIGN,
    OPERATOR_GT,
    OPERATOR_LT,
    OPERATOR_GE,
    OPERATOR_LE,
    OPERATOR_LE_ALT,
    OPERATOR_EQ,
    OPERATOR_NE,
    LPAREN,
    RPAREN,
    LBRACKET,
    RBRACKET,
    COMMA,
    SEMICOLON,
    COLON,
    UNDERSCORE,
    AT,
    DOLLAR,
    DOT,
    NEWLINE,
    END_OF_FILE
};

struct Token {
    TokenType type;
    std::string value;
    int line;
    int column;
    std::string file;
};

enum class ASTNodeType {
    PROGRAM,
    DEF_VAR,
    ASS_VAR,
    DEF_FUN,
    INS_FUN,
    ASS_FUN,
    DEF_MOD,
    IF_STMT,
    PRINT_STMT,
    INPUT_STMT,
    JUMP_STMT,
    CALL_STMT,
    RUN_STMT,
    EXIT_STMT,
    WHILE_STMT,
    WAIT_STMT,
    CHECK_TIP_STMT,
    KILL_STMT,
    BINARY_EXPR,
    UNARY_EXPR,
    LITERAL,
    VAR_REF,
    FUN_VALUE_REF,
    TYPE_FUNC,
    WPS_FUNC,
    IMPORT_STMT,
    PACKAGE_CALL
};

enum class VarType {
    NUMBER,
    STRING,
    MIXED
};

enum class ErrorCode {
    UNDEFINED_VARIABLE,
    UNDEFINED_FUN_TYPE,
    UNINSTANTIATED_FUN,
    TYPE_MISMATCH,
    DIMENSION_MISMATCH,
    DUPLICATE_DEFINITION,
    DUPLICATE_INSTANTIATION,
    MISSING_PARAMETER,
    SYNTAX_FORMAT,
    ASSIGNMENT_DIRECTION,
    MODULE_NOT_FOUND,
    FILE_NOT_FOUND,
    MIXED_TYPE_OPERATION,
    MATRIX_INDEX_OUT_OF_BOUNDS,
    MISMATCHED_END_MARKER,
    MATRIX_PRINT_DIMENSION,
    MATRIX_PRINT_MIXED_CONTENT,
    PACKAGE_NOT_FOUND,
    DUPLICATE_ALIAS,
    PACKAGE_FUNCTION_NOT_FOUND,
    WHILE_NOT_IN_LOOP,
    CHECKPOINT_NOT_FOUND,
    KILL_TARGET_NOT_FOUND,
    PACKAGE_GRAMMAR_FORMAT_MISMATCH
};

struct MatrixDimension {
    bool isRange = false;
    int startIndex = 0;
    int endIndex = 0;
    bool startIsVar = false;
    bool endIsVar = false;
    std::string startVarName;
    std::string endVarName;
    bool isVarIndexed = false;
};

struct MatrixIndex {
    std::vector<MatrixDimension> dims;
};

struct ASTNode {
    ASTNodeType type;
    int line = 0;
    int column = 0;
    std::string file;
    virtual ~ASTNode() = default;
};

struct LiteralNode : ASTNode {
    enum LitType { INT_VAL, FLOAT_VAL, STR_VAL } litType = INT_VAL;
    long long intVal = 0;
    double floatVal = 0.0;
    std::string strVal;
};

struct VarRefNode : ASTNode {
    std::string name;
    MatrixIndex matrixIdx;
    bool isMatrix = false;
};

struct FunValueRefNode : ASTNode {
    std::string instanceName;
    MatrixIndex instanceMatrixIdx;
    bool isInstanceMatrix = false;
    std::string valueName;
};

struct BinaryExprNode : ASTNode {
    std::string op;
    std::shared_ptr<ASTNode> left;
    std::shared_ptr<ASTNode> right;
};

struct UnaryExprNode : ASTNode {
    std::string op;
    std::shared_ptr<ASTNode> operand;
};

struct TypeFuncNode : ASTNode {
    std::string funcName;
    std::shared_ptr<ASTNode> arg;
};

struct WPSFuncNode : ASTNode {
    std::string funcName;
    std::shared_ptr<ASTNode> arg;
};

struct DefVarNode : ASTNode {
    std::string name;
    MatrixIndex matrixIdx;
    bool isMatrix = false;
    VarType varType = VarType::NUMBER;
};

struct AssVarNode : ASTNode {
    std::string name;
    MatrixIndex matrixIdx;
    bool isMatrixAssign = false;
    bool isIndexAssign = false;
    bool isRangeAssign = false;
    std::shared_ptr<ASTNode> value;
    std::string sourceMatrixName;
    MatrixIndex sourceMatrixIdx;
    bool hasSourceMatrix = false;
};

struct FunAssignItem {
    std::vector<std::string> names;
    std::shared_ptr<ASTNode> value;
    std::string linkVarName;
    bool isLink = false;
};

struct AssFunNode : ASTNode {
    std::string instanceName;
    MatrixIndex instanceMatrixIdx;
    bool isInstanceMatrix = false;
    std::vector<FunAssignItem> assignments;
};

struct FunInternalDef {
    std::string name;
    std::shared_ptr<ASTNode> expr;
    std::string wpsFuncName;
    std::shared_ptr<ASTNode> wpsArg;
    bool isWPS = false;
};

struct DefFunNode : ASTNode {
    std::string name;
    VarType funType = VarType::MIXED;
    std::vector<FunInternalDef> internalDefs;
};

struct InsFunNode : ASTNode {
    std::string typeName;
    std::string instanceName;
    MatrixIndex instanceMatrixIdx;
    bool isMatrix = false;
};

struct ConditionGroup {
    std::vector<std::vector<std::shared_ptr<ASTNode>>> orGroups;
};

struct IfStmtNode : ASTNode {
    ConditionGroup condition;
    std::vector<std::shared_ptr<ASTNode>> thenBlock;
    std::vector<std::pair<ConditionGroup, std::vector<std::shared_ptr<ASTNode>>>> elseIfBlocks;
    std::vector<std::shared_ptr<ASTNode>> elseBlock;
};

struct PrintStmtNode : ASTNode {
    std::shared_ptr<ASTNode> expr;
    std::vector<std::shared_ptr<ASTNode>> exprs;
    bool isMatrixPrint = false;
    bool isFunMatrixPrint = false;
    std::string matrixName;
    MatrixIndex matrixIdx;
    std::string funInstanceName;
    MatrixIndex funInstanceMatrixIdx;
    std::string funValueName;
};

struct InputStmtNode : ASTNode {
    std::string varName;
    std::string prompt;
    VarType inputType = VarType::MIXED;
    bool isNumberInput = false;
    bool isWordInput = false;
};

struct DefModNode : ASTNode {
    std::string name;
    bool isMain = false;
    std::vector<std::shared_ptr<ASTNode>> body;
};

struct JumpStmtNode : ASTNode {
    bool isLocal = true;
    std::string moduleName;
    std::string fileName;
};

struct CallStmtNode : ASTNode {
    bool isLocal = true;
    std::string moduleName;
    std::string fileName;
    std::string groupName;
};

struct RunStmtNode : ASTNode {
    std::string path;
    bool isRelative = false;
};

const int EXIT_ALL = 0;
const int EXIT_CURRENT = 1;
const int EXIT_WHILE = 2;

struct ExitStmtNode : ASTNode {
    int exitType = EXIT_ALL;
};

struct WhileStmtNode : public ASTNode {
    std::vector<std::shared_ptr<ASTNode>> condition;
    std::vector<std::shared_ptr<ASTNode>> body;
    WhileStmtNode() { type = ASTNodeType::WHILE_STMT; }
};

struct WaitStmtNode : public ASTNode {
    std::string checkPointName;
    double waitSeconds;
    bool isTimedWait;
    WaitStmtNode() : waitSeconds(0), isTimedWait(false) { type = ASTNodeType::WAIT_STMT; }
};

struct CheckTipStmtNode : public ASTNode {
    std::string checkPointName;
    CheckTipStmtNode() { type = ASTNodeType::CHECK_TIP_STMT; }
};

struct KillStmtNode : public ASTNode {
    std::vector<std::string> groupNames;
    bool killAll;
    bool killUnassigned;
    KillStmtNode() : killAll(false), killUnassigned(false) { type = ASTNodeType::KILL_STMT; }
};

struct ProgramNode : ASTNode {
    std::vector<std::shared_ptr<ASTNode>> initCode;
    std::vector<std::shared_ptr<ASTNode>> modules;
};

// import语句节点
struct ImportStmtNode : ASTNode {
    std::string packageName;
    std::string alias;
};

// 包调用语句节点
struct PackageCallNode : ASTNode {
    std::string alias;
    std::string funcName;
    std::vector<std::shared_ptr<ASTNode>> args;
};
