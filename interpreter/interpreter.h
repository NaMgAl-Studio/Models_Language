#pragma once

#include <string>
#include <vector>
#include <map>
#include <memory>
#include <variant>
#include <thread>
#include <chrono>
#include <condition_variable>
#include <mutex>
#include <cctype>
#include "common.h"
#include "error.h"

struct Value {
    enum Type { INT_VAL, FLOAT_VAL, STR_VAL } type = INT_VAL;
    long long intVal = 0;
    double floatVal = 0.0;
    std::string strVal;

    bool isNumber() const { return type == INT_VAL || type == FLOAT_VAL; }
    double asNumber() const { return type == FLOAT_VAL ? floatVal : static_cast<double>(intVal); }
    long long asInt() const { return type == INT_VAL ? intVal : static_cast<long long>(floatVal); }
    std::string toString() const {
        if (type == INT_VAL) return std::to_string(intVal);
        if (type == FLOAT_VAL) {
            std::string s = std::to_string(floatVal);
            size_t dot = s.find('.');
            if (dot != std::string::npos) {
                size_t last = s.find_last_not_of('0');
                if (last > dot) s = s.substr(0, last + 1);
                else s = s.substr(0, dot + 2);
            }
            return s;
        }
        return strVal;
    }
    bool isBool() const {
        if (type == INT_VAL) return intVal == 0 || intVal == 1;
        return false;
    }
    bool asBool() const {
        if (type == INT_VAL) return intVal != 0;
        if (type == FLOAT_VAL) return floatVal != 0.0;
        return !strVal.empty();
    }
};

struct MatrixData {
    std::vector<int> dimSizes;
    std::vector<Value> data;
    VarType varType = VarType::NUMBER;

    int totalSize() const {
        int s = 1;
        for (int d : dimSizes) s *= d;
        return s;
    }
    int flatIndex(const std::vector<int>& indices) const {
        int idx = 0;
        int multiplier = 1;
        for (int i = static_cast<int>(dimSizes.size()) - 1; i >= 0; i--) {
            idx += indices[i] * multiplier;
            multiplier *= dimSizes[i];
        }
        return idx;
    }
};

struct FunInstanceValue {
    std::string name;
    Value value;
    bool isLinked = false;
    std::string linkVarName;
};

struct FunInstance {
    std::string typeName;
    std::map<std::string, FunInstanceValue> values;
};

struct FunTypeData {
    std::string name;
    VarType funType = VarType::MIXED;
    std::vector<FunInternalDef> internalDefs;
};

// 包函数信息
struct PackageFuncInfo {
    std::string name;
    std::string grammar;
};

// 包信息
struct PackageInfo {
    std::string name;
    std::string dllPath;
    std::string codeType;
    std::map<std::string, PackageFuncInfo> functions;
    std::map<std::string, Value> variables;
};

class ExitCurrentProcessException : public std::exception {
public:
    const char* what() const noexcept override { return "Exit current process"; }
};

class Interpreter {
public:
    Interpreter();
    void execute(std::shared_ptr<ProgramNode> program, const std::string& scriptPath);

private:
    std::map<std::string, Value> variables;
    std::map<std::string, MatrixData> matrixVars;
    std::map<std::string, VarType> varTypes;
    std::map<std::string, FunTypeData> funTypes;
    std::map<std::string, FunInstance> funInstances;
    std::map<std::string, MatrixData> funInstanceMatrices;
    std::map<std::string, std::shared_ptr<DefModNode>> modules;
    std::string scriptPath;
    bool shouldExit;
    bool whileLoopExitFlag;

    std::map<std::string, bool> checkpoints;
    std::map<std::string, std::condition_variable*> checkpointCVs;
    std::map<std::string, std::mutex*> checkpointMutexes;
    std::map<std::string, std::vector<int>> processGroups;
    std::map<int, std::thread*> runningThreads;
    std::map<int, bool> threadStopFlags;
    int nextProcessId;

    // 包系统
    std::map<std::string, PackageInfo> packages;
    std::map<std::string, std::string> aliasToPackage;

    void executeInit(std::shared_ptr<ProgramNode> program);
    void executeModule(const std::string& name, const std::string& file);
    void executeBlock(const std::vector<std::shared_ptr<ASTNode>>& block);
    void executeDefVar(std::shared_ptr<DefVarNode> node);
    void executeAssVar(std::shared_ptr<AssVarNode> node);
    void executeDefFun(std::shared_ptr<DefFunNode> node);
    void executeInsFun(std::shared_ptr<InsFunNode> node);
    void executeAssFun(std::shared_ptr<AssFunNode> node);
    void executeIf(std::shared_ptr<IfStmtNode> node);
    void executePrint(std::shared_ptr<PrintStmtNode> node);
    void executeInput(std::shared_ptr<InputStmtNode> node);
    void executeJump(std::shared_ptr<JumpStmtNode> node);
    void executeCall(std::shared_ptr<CallStmtNode> node);
    void executeRun(std::shared_ptr<RunStmtNode> node);
    void executeImport(std::shared_ptr<ImportStmtNode> node);
    void executePackageCall(std::shared_ptr<PackageCallNode> node);
    void executeWhile(std::shared_ptr<WhileStmtNode> node);
    void executeWait(std::shared_ptr<WaitStmtNode> node);
    void executeCheckTip(std::shared_ptr<CheckTipStmtNode> node);
    void executeKill(std::shared_ptr<KillStmtNode> node);
    void executeExit(std::shared_ptr<ExitStmtNode> node);
    std::string findDllFile(const std::string& packageName);

    Value evaluate(std::shared_ptr<ASTNode> node);
    Value evaluateBinary(std::shared_ptr<BinaryExprNode> node);
    Value evaluateUnary(std::shared_ptr<UnaryExprNode> node);
    Value evaluateVarRef(std::shared_ptr<VarRefNode> node);
    Value evaluateFunValueRef(std::shared_ptr<FunValueRefNode> node);
    Value evaluateTypeFunc(std::shared_ptr<TypeFuncNode> node);
    Value evaluateWPSFunc(std::shared_ptr<WPSFuncNode> node);

    bool evaluateCondition(const ConditionGroup& group);
    bool isTruthy(const Value& v);

    Value getVariable(const std::string& name);
    void setVariable(const std::string& name, const Value& val);

    int resolveDimIndex(const MatrixDimension& dim, bool isEnd);
    std::vector<int> getMatrixDimSizes(const MatrixIndex& idx, const std::vector<int>& fullDims);
    void collectMatrixValues(const std::string& name, const MatrixIndex& idx,
                             std::vector<std::vector<int>>& indices);
    void collectMatrixRangeIndices(const MatrixIndex& idx, int dimIdx,
                                   std::vector<int> current,
                                   std::vector<std::vector<int>>& result);

    void printMatrix(const std::string& name, const MatrixIndex& idx);
    void printFunMatrix(const std::string& instanceName, const MatrixIndex& idx, const std::string& valueName);

    Value getLinkedValue(const std::string& varName);
    void setLinkedValue(const std::string& varName, const Value& val);

    std::string findMslaFile(const std::string& name);
    std::string readFile(const std::string& path);

    void checkTypeCompatibility(VarType varType, const Value& val, int line, int col, const std::string& file);
};
