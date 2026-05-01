#include "interpreter.h"
#include "lexer.h"
#include "parser.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <cmath>
#include <algorithm>
#include <filesystem>
#include <cstdlib>
#include <set>

#ifdef _WIN32
#include <windows.h>
#endif

namespace fs = std::filesystem;

Interpreter::Interpreter() : shouldExit(false), whileLoopExitFlag(false), nextProcessId(1) {}

void Interpreter::execute(std::shared_ptr<ProgramNode> program, const std::string& path) {
    scriptPath = path;
    shouldExit = false;

    for (auto& stmt : program->modules) {
        if (stmt->type == ASTNodeType::DEF_MOD) {
            auto mod = std::static_pointer_cast<DefModNode>(stmt);
            modules[mod->name] = mod;
        }
    }

    executeBlock(program->initCode);
    if (shouldExit) return;

    for (auto& stmt : program->modules) {
        if (stmt->type == ASTNodeType::DEF_MOD) {
            auto mod = std::static_pointer_cast<DefModNode>(stmt);
            if (mod->isMain) {
                executeBlock(mod->body);
                return;
            }
        }
    }
}

void Interpreter::executeBlock(const std::vector<std::shared_ptr<ASTNode>>& block) {
    for (auto& stmt : block) {
        if (shouldExit) return;
        if (whileLoopExitFlag) return;
        switch (stmt->type) {
            case ASTNodeType::DEF_VAR: executeDefVar(std::static_pointer_cast<DefVarNode>(stmt)); break;
            case ASTNodeType::ASS_VAR: executeAssVar(std::static_pointer_cast<AssVarNode>(stmt)); break;
            case ASTNodeType::DEF_FUN: executeDefFun(std::static_pointer_cast<DefFunNode>(stmt)); break;
            case ASTNodeType::INS_FUN: executeInsFun(std::static_pointer_cast<InsFunNode>(stmt)); break;
            case ASTNodeType::ASS_FUN: executeAssFun(std::static_pointer_cast<AssFunNode>(stmt)); break;
            case ASTNodeType::IF_STMT: executeIf(std::static_pointer_cast<IfStmtNode>(stmt)); break;
            case ASTNodeType::PRINT_STMT: executePrint(std::static_pointer_cast<PrintStmtNode>(stmt)); break;
            case ASTNodeType::INPUT_STMT: executeInput(std::static_pointer_cast<InputStmtNode>(stmt)); break;
            case ASTNodeType::JUMP_STMT: executeJump(std::static_pointer_cast<JumpStmtNode>(stmt)); break;
            case ASTNodeType::CALL_STMT: executeCall(std::static_pointer_cast<CallStmtNode>(stmt)); break;
            case ASTNodeType::RUN_STMT: executeRun(std::static_pointer_cast<RunStmtNode>(stmt)); break;
            case ASTNodeType::IMPORT_STMT: executeImport(std::static_pointer_cast<ImportStmtNode>(stmt)); break;
            case ASTNodeType::PACKAGE_CALL: executePackageCall(std::static_pointer_cast<PackageCallNode>(stmt)); break;
            case ASTNodeType::WHILE_STMT: executeWhile(std::dynamic_pointer_cast<WhileStmtNode>(stmt)); break;
            case ASTNodeType::WAIT_STMT: executeWait(std::dynamic_pointer_cast<WaitStmtNode>(stmt)); break;
            case ASTNodeType::CHECK_TIP_STMT: executeCheckTip(std::dynamic_pointer_cast<CheckTipStmtNode>(stmt)); break;
            case ASTNodeType::KILL_STMT: executeKill(std::dynamic_pointer_cast<KillStmtNode>(stmt)); break;
            case ASTNodeType::EXIT_STMT: executeExit(std::dynamic_pointer_cast<ExitStmtNode>(stmt)); if (shouldExit) return; break;
            default: break;
        }
    }
}

void Interpreter::executeDefVar(std::shared_ptr<DefVarNode> node) {
    if (variables.count(node->name) || matrixVars.count(node->name)) {
        throw DuplicateDefinitionError(node->file, node->line, node->column,
            "Variable '" + node->name + "' already defined");
    }
    varTypes[node->name] = node->varType;

    if (node->isMatrix) {
        MatrixData mat;
        mat.varType = node->varType;
        for (auto& dim : node->matrixIdx.dims) {
            if (dim.isRange) {
                mat.dimSizes.push_back(dim.endIndex - dim.startIndex + 1);
            } else {
                throw SyntaxFormatError(node->file, node->line, node->column,
                    "Matrix definition must use range dimensions [start:end]");
            }
        }
        mat.data.resize(mat.totalSize());
        for (auto& v : mat.data) {
            if (node->varType == VarType::NUMBER) v = Value{Value::INT_VAL, 0, 0.0, ""};
            else if (node->varType == VarType::STRING) v = Value{Value::STR_VAL, 0, 0.0, ""};
        }
        matrixVars[node->name] = mat;
    } else {
        Value v;
        if (node->varType == VarType::NUMBER) v = Value{Value::INT_VAL, 0, 0.0, ""};
        else if (node->varType == VarType::STRING) v = Value{Value::STR_VAL, 0, 0.0, ""};
        else v = Value{Value::INT_VAL, 0, 0.0, ""};
        variables[node->name] = v;
    }
}

void Interpreter::executeAssVar(std::shared_ptr<AssVarNode> node) {
    if (node->isMatrixAssign) {
        if (matrixVars.find(node->name) == matrixVars.end()) {
            throw UndefinedVariableError(node->file, node->line, node->column,
                "Matrix variable '" + node->name + "' not defined");
        }
        auto& mat = matrixVars[node->name];

        if (node->hasSourceMatrix) {
            if (matrixVars.find(node->sourceMatrixName) == matrixVars.end()) {
                throw UndefinedVariableError(node->file, node->line, node->column,
                    "Source matrix '" + node->sourceMatrixName + "' not defined");
            }
            auto& srcMat = matrixVars[node->sourceMatrixName];
            std::vector<int> srcDimSizes;
            if (node->sourceMatrixIdx.dims.empty()) {
                srcDimSizes = srcMat.dimSizes;
            } else {
                srcDimSizes = getMatrixDimSizes(node->sourceMatrixIdx, srcMat.dimSizes);
            }
            std::vector<int> dstDimSizes = getMatrixDimSizes(node->matrixIdx, mat.dimSizes);
            std::vector<std::vector<int>> srcIndices, dstIndices;

            if (node->sourceMatrixIdx.dims.empty()) {
                collectMatrixRangeIndices(MatrixIndex{std::vector<MatrixDimension>()}, 0, {}, srcIndices);
                for (int i = 0; i < srcMat.totalSize(); i++) {
                    std::vector<int> idx(srcMat.dimSizes.size(), 0);
                    int temp = i;
                    for (int d = srcMat.dimSizes.size() - 1; d >= 0; d--) {
                        idx[d] = temp % srcMat.dimSizes[d];
                        temp /= srcMat.dimSizes[d];
                    }
                    srcIndices.push_back(idx);
                }
            } else {
                for (size_t di = 0; di < node->sourceMatrixIdx.dims.size(); di++) {
                    auto& dim = node->sourceMatrixIdx.dims[di];
                    if (dim.isRange) {
                        int start = resolveDimIndex(dim, false);
                        int end = resolveDimIndex(dim, true);
                    }
                }
                collectMatrixRangeIndices(node->sourceMatrixIdx, 0, {}, srcIndices);
            }
            collectMatrixRangeIndices(node->matrixIdx, 0, {}, dstIndices);

            int count = std::min(static_cast<int>(dstIndices.size()), static_cast<int>(srcIndices.size()));
            for (int i = 0; i < count; i++) {
                int dstFlat = mat.flatIndex(dstIndices[i]);
                int srcFlat = srcMat.flatIndex(srcIndices[i]);
                mat.data[dstFlat] = srcMat.data[srcFlat];
            }
            return;
        }

        Value val = evaluate(node->value);
        checkTypeCompatibility(mat.varType, val, node->line, node->column, node->file);

        bool isFullAssign = true;
        for (auto& dim : node->matrixIdx.dims) {
            if (!dim.isRange) { isFullAssign = false; break; }
        }

        if (isFullAssign) {
            bool allFullRange = true;
            for (size_t i = 0; i < node->matrixIdx.dims.size(); i++) {
                auto& dim = node->matrixIdx.dims[i];
                int start = resolveDimIndex(dim, false);
                int end = resolveDimIndex(dim, true);
                if (start != 0 || end != mat.dimSizes[i] - 1) {
                    allFullRange = false;
                    break;
                }
            }
            if (allFullRange) {
                for (auto& elem : mat.data) elem = val;
                return;
            }
        }

        // 收集矩阵索引，支持变量索引部分越界
        std::vector<std::vector<int>> indices;
        collectMatrixRangeIndices(node->matrixIdx, 0, {}, indices);
        for (auto& idx : indices) {
            bool anyInBounds = false;
            bool allOutOfVars = true;
            for (size_t d = 0; d < idx.size(); d++) {
                if (idx[d] >= 0 && idx[d] < mat.dimSizes[d]) anyInBounds = true;
                if (!node->matrixIdx.dims[d].isVarIndexed) allOutOfVars = false;
            }
            if (!anyInBounds && !allOutOfVars) {
                bool hasVarIndex = false;
                for (auto& dim : node->matrixIdx.dims) {
                    if (dim.isVarIndexed) { hasVarIndex = true; break; }
                }
                if (!hasVarIndex) {
                    throw MatrixIndexOutOfBoundsError(node->file, node->line, node->column,
                        "Matrix index out of bounds for '" + node->name + "'");
                }
            }
            if (anyInBounds) {
                std::vector<int> clampedIdx;
                for (size_t d = 0; d < idx.size(); d++) {
                    clampedIdx.push_back(std::max(0, std::min(idx[d], mat.dimSizes[d] - 1)));
                }
                mat.data[mat.flatIndex(clampedIdx)] = val;
            }
        }
    } else {
        if (matrixVars.find(node->name) != matrixVars.end()) {
            auto& mat = matrixVars[node->name];
            Value val = evaluate(node->value);
            checkTypeCompatibility(mat.varType, val, node->line, node->column, node->file);
            for (auto& elem : mat.data) elem = val;
        } else if (variables.find(node->name) != variables.end()) {
            Value val = evaluate(node->value);
            VarType vt = varTypes[node->name];
            checkTypeCompatibility(vt, val, node->line, node->column, node->file);
            variables[node->name] = val;
        } else {
            throw UndefinedVariableError(node->file, node->line, node->column,
                "Variable '" + node->name + "' not defined");
        }
    }
}

void Interpreter::executeDefFun(std::shared_ptr<DefFunNode> node) {
    if (funTypes.count(node->name)) {
        throw DuplicateDefinitionError(node->file, node->line, node->column,
            "Function type '" + node->name + "' already defined");
    }
    FunTypeData ft;
    ft.name = node->name;
    ft.funType = node->funType;
    ft.internalDefs = node->internalDefs;
    funTypes[node->name] = ft;
}

void Interpreter::executeInsFun(std::shared_ptr<InsFunNode> node) {
    if (funTypes.find(node->typeName) == funTypes.end()) {
        throw UndefinedFunTypeError(node->file, node->line, node->column,
            "Function type '" + node->typeName + "' not defined");
    }
    if (funInstances.count(node->instanceName) || funInstanceMatrices.count(node->instanceName)) {
        throw DuplicateInstantiationError(node->file, node->line, node->column,
            "Function instance '" + node->instanceName + "' already exists");
    }

    auto& ft = funTypes[node->typeName];

    if (node->isMatrix) {
        MatrixData mat;
        mat.varType = VarType::NUMBER;
        for (auto& dim : node->instanceMatrixIdx.dims) {
            if (dim.isRange) {
                mat.dimSizes.push_back(dim.endIndex - dim.startIndex + 1);
            } else {
                throw SyntaxFormatError(node->file, node->line, node->column,
                    "Function matrix instantiation must use range dimensions");
            }
        }
        std::vector<FunInstance> instances;
        instances.resize(mat.totalSize());
        for (auto& inst : instances) {
            inst.typeName = node->typeName;
            for (auto& idef : ft.internalDefs) {
                FunInstanceValue fv;
                fv.name = idef.name;
                fv.value = Value{Value::INT_VAL, 0, 0.0, ""};
                inst.values[idef.name] = fv;
            }
        }
        funInstanceMatrices[node->instanceName] = mat;

        for (int i = 0; i < mat.totalSize(); i++) {
            std::string key = node->instanceName + "#" + std::to_string(i);
            funInstances[key] = instances[i];
        }
    } else {
        FunInstance inst;
        inst.typeName = node->typeName;
        for (auto& idef : ft.internalDefs) {
            FunInstanceValue fv;
            fv.name = idef.name;
            fv.value = Value{Value::INT_VAL, 0, 0.0, ""};
            inst.values[idef.name] = fv;
        }
        funInstances[node->instanceName] = inst;
    }
}

void Interpreter::executeAssFun(std::shared_ptr<AssFunNode> node) {
    std::vector<FunInstance*> targets;

    if (node->isInstanceMatrix) {
        if (funInstanceMatrices.find(node->instanceName) == funInstanceMatrices.end()) {
            throw UninstantiatedFunError(node->file, node->line, node->column,
                "Function instance matrix '" + node->instanceName + "' not instantiated");
        }
        auto& mat = funInstanceMatrices[node->instanceName];
        std::vector<std::vector<int>> indices;
        collectMatrixRangeIndices(node->instanceMatrixIdx, 0, {}, indices);
        for (auto& idx : indices) {
            int flat = 0;
            int mult = 1;
            for (int i = static_cast<int>(mat.dimSizes.size()) - 1; i >= 0; i--) {
                flat += idx[i] * mult;
                mult *= mat.dimSizes[i];
            }
            std::string key = node->instanceName + "#" + std::to_string(flat);
            if (funInstances.find(key) != funInstances.end()) {
                targets.push_back(&funInstances[key]);
            }
        }
    } else {
        if (funInstances.find(node->instanceName) == funInstances.end()) {
            throw UninstantiatedFunError(node->file, node->line, node->column,
                "Function instance '" + node->instanceName + "' not instantiated");
        }
        targets.push_back(&funInstances[node->instanceName]);
    }

    for (auto& item : node->assignments) {
        for (auto& inst : targets) {
            for (auto& name : item.names) {
                if (inst->values.find(name) == inst->values.end()) {
                    inst->values[name] = FunInstanceValue{name, Value{Value::INT_VAL, 0, 0.0, ""}, false, ""};
                }
            }
        }

        if (item.isLink) {
            for (auto& inst : targets) {
                for (auto& name : item.names) {
                    inst->values[name].isLinked = true;
                    inst->values[name].linkVarName = item.linkVarName;
                    inst->values[name].value = getLinkedValue(item.linkVarName);
                }
            }
        } else {
            Value val = evaluate(item.value);
            for (auto& inst : targets) {
                for (auto& name : item.names) {
                    inst->values[name].value = val;
                    inst->values[name].isLinked = false;
                }
            }
        }
    }
}

void Interpreter::executeIf(std::shared_ptr<IfStmtNode> node) {
    if (evaluateCondition(node->condition)) {
        executeBlock(node->thenBlock);
        return;
    }

    for (auto& elif : node->elseIfBlocks) {
        if (evaluateCondition(elif.first)) {
            executeBlock(elif.second);
            return;
        }
    }

    if (!node->elseBlock.empty()) {
        executeBlock(node->elseBlock);
    }
}

void Interpreter::executeWhile(std::shared_ptr<WhileStmtNode> node) {
    whileLoopExitFlag = false;
    while (true) {
        if (!node->condition.empty()) {
            bool condResult = true;
            for (auto& expr : node->condition) {
                if (!isTruthy(evaluate(expr))) {
                    condResult = false;
                    break;
                }
            }
            if (!condResult) break;
        }

        executeBlock(node->body);
        if (whileLoopExitFlag) {
            whileLoopExitFlag = false;
            return;
        }
        if (shouldExit) return;
    }
}

void Interpreter::executeWait(std::shared_ptr<WaitStmtNode> node) {
    if (node->isTimedWait) {
        int ms = static_cast<int>(node->waitSeconds * 1000);
        std::this_thread::sleep_for(std::chrono::milliseconds(ms));
    } else {
        std::string cpName = node->checkPointName;
        if (checkpoints.find(cpName) == checkpoints.end()) {
            checkpoints[cpName] = false;
            checkpointMutexes[cpName] = new std::mutex();
            checkpointCVs[cpName] = new std::condition_variable();
        }

        std::unique_lock<std::mutex> lock(*checkpointMutexes[cpName]);
        checkpointCVs[cpName]->wait(lock, [this, cpName]() {
            return checkpoints[cpName];
        });
        checkpoints[cpName] = false;
    }
}

void Interpreter::executeCheckTip(std::shared_ptr<CheckTipStmtNode> node) {
    std::string cpName = node->checkPointName;
    if (checkpoints.find(cpName) == checkpoints.end()) {
        checkpoints[cpName] = false;
        checkpointMutexes[cpName] = new std::mutex();
        checkpointCVs[cpName] = new std::condition_variable();
    }

    checkpoints[cpName] = true;
    checkpointCVs[cpName]->notify_all();
}

void Interpreter::executeKill(std::shared_ptr<KillStmtNode> node) {
    if (node->killAll) {
        shouldExit = true;
        for (auto& pair : runningThreads) {
            threadStopFlags[pair.first] = true;
        }
        for (auto& pair : runningThreads) {
            if (pair.second && pair.second->joinable()) {
                pair.second->join();
            }
            delete pair.second;
        }
        runningThreads.clear();
        threadStopFlags.clear();
    } else if (node->killUnassigned) {
        std::set<int> assignedPids;
        for (auto& group : processGroups) {
            for (int pid : group.second) {
                assignedPids.insert(pid);
            }
        }
        for (auto it = runningThreads.begin(); it != runningThreads.end(); ) {
            if (assignedPids.find(it->first) == assignedPids.end()) {
                threadStopFlags[it->first] = true;
                if (it->second && it->second->joinable()) {
                    it->second->join();
                }
                delete it->second;
                it = runningThreads.erase(it);
            } else {
                ++it;
            }
        }
    } else if (!node->groupNames.empty()) {
        for (auto& groupName : node->groupNames) {
            if (processGroups.find(groupName) != processGroups.end()) {
                for (int pid : processGroups[groupName]) {
                    threadStopFlags[pid] = true;
                    if (runningThreads.find(pid) != runningThreads.end()) {
                        if (runningThreads[pid] && runningThreads[pid]->joinable()) {
                            runningThreads[pid]->join();
                        }
                        delete runningThreads[pid];
                        runningThreads.erase(pid);
                    }
                }
                processGroups.erase(groupName);
            }
        }
    }
}

void Interpreter::executeExit(std::shared_ptr<ExitStmtNode> node) {
    switch (node->exitType) {
        case EXIT_ALL:
            shouldExit = true;
            break;
        case EXIT_CURRENT:
            throw ExitCurrentProcessException();
            break;
        case EXIT_WHILE:
            whileLoopExitFlag = true;
            break;
    }
}

void Interpreter::executePrint(std::shared_ptr<PrintStmtNode> node) {
    if (node->isMatrixPrint) {
        printMatrix(node->matrixName, node->matrixIdx);
        return;
    }
    if (node->isFunMatrixPrint) {
        printFunMatrix(node->funInstanceName, node->funInstanceMatrixIdx, node->funValueName);
        return;
    }

    if (node->exprs.size() > 1) {
        for (size_t i = 0; i < node->exprs.size(); i++) {
            Value val = evaluate(node->exprs[i]);
            if (i > 0) std::cout << " ";
            std::cout << val.toString();
        }
        std::cout << std::endl;
        return;
    }

    if (node->expr->type == ASTNodeType::VAR_REF) {
        auto varRef = std::static_pointer_cast<VarRefNode>(node->expr);
        if (!varRef->isMatrix && matrixVars.find(varRef->name) != matrixVars.end()) {
            MatrixIndex fullIdx;
            auto& mat = matrixVars[varRef->name];
            for (size_t i = 0; i < mat.dimSizes.size(); i++) {
                MatrixDimension dim;
                dim.isRange = true;
                dim.startIndex = 0;
                dim.endIndex = mat.dimSizes[i] - 1;
                fullIdx.dims.push_back(dim);
            }
            printMatrix(varRef->name, fullIdx);
            return;
        }
    }

    Value val = evaluate(node->expr);
    std::cout << val.toString() << std::endl;
}

void Interpreter::executeInput(std::shared_ptr<InputStmtNode> node) {
    std::cout << node->prompt;
    std::string input;
    std::getline(std::cin, input);

    std::string filteredInput;
    if (node->isNumberInput) {
        for (char c : input) {
            if (std::isdigit(static_cast<unsigned char>(c)) || c == '.') {
                filteredInput += c;
            }
        }
    } else if (node->isWordInput) {
        for (char c : input) {
            if (!std::isdigit(static_cast<unsigned char>(c))) {
                filteredInput += c;
            }
        }
    } else {
        filteredInput = input;
    }

    if (variables.find(node->varName) == variables.end()) {
        throw UndefinedVariableError(node->file, node->line, node->column,
            "Variable '" + node->varName + "' not defined for input");
    }

    Value val;
    if (node->inputType == VarType::NUMBER) {
        try {
            if (filteredInput.find('.') != std::string::npos) {
                val = Value{Value::FLOAT_VAL, 0, std::stod(filteredInput), ""};
            } else {
                val = Value{Value::INT_VAL, std::stoll(filteredInput), 0.0, ""};
            }
        } catch (...) {
            val = Value{Value::INT_VAL, 0, 0.0, ""};
        }
    } else if (node->inputType == VarType::STRING) {
        val = Value{Value::STR_VAL, 0, 0.0, filteredInput};
    } else {
        try {
            if (filteredInput.find('.') != std::string::npos) {
                val = Value{Value::FLOAT_VAL, 0, std::stod(filteredInput), ""};
            } else if (!filteredInput.empty() && std::all_of(filteredInput.begin(), filteredInput.end(), [](char c) { return std::isdigit(static_cast<unsigned char>(c)) || c == '-'; })) {
                val = Value{Value::INT_VAL, std::stoll(filteredInput), 0.0, ""};
            } else {
                val = Value{Value::STR_VAL, 0, 0.0, filteredInput};
            }
        } catch (...) {
            val = Value{Value::STR_VAL, 0, 0.0, filteredInput};
        }
    }

    VarType vt = varTypes[node->varName];
    checkTypeCompatibility(vt, val, node->line, node->column, node->file);
    variables[node->varName] = val;
}

void Interpreter::executeJump(std::shared_ptr<JumpStmtNode> node) {
    if (node->isLocal) {
        auto it = modules.find(node->moduleName);
        if (it == modules.end()) {
            throw ModuleNotFoundError(node->file, node->line, node->column,
                "Module '" + node->moduleName + "' not found");
        }
        executeBlock(it->second->body);
    } else {
        executeModule(node->moduleName, node->fileName);
    }
}

void Interpreter::executeCall(std::shared_ptr<CallStmtNode> node) {
    if (!node->groupName.empty()) {
        int pid = nextProcessId++;
        processGroups[node->groupName].push_back(pid);
        threadStopFlags[pid] = false;

        std::string moduleName = node->moduleName;
        bool isLocal = node->isLocal;
        std::string fileName = node->fileName;

        if (isLocal) {
            auto it = modules.find(moduleName);
            if (it == modules.end()) {
                throw ModuleNotFoundError(node->file, node->line, node->column,
                    "Module '" + moduleName + "' not found");
            }
            auto body = it->second->body;
            runningThreads[pid] = new std::thread([this, body, pid]() {
                try {
                    executeBlock(body);
                } catch (const ExitCurrentProcessException&) {}
                threadStopFlags.erase(pid);
            });
        } else {
            runningThreads[pid] = new std::thread([this, moduleName, fileName, pid]() {
                try {
                    executeModule(moduleName, fileName);
                } catch (const ExitCurrentProcessException&) {}
                threadStopFlags.erase(pid);
            });
        }
    } else {
        if (node->isLocal) {
            auto it = modules.find(node->moduleName);
            if (it == modules.end()) {
                throw ModuleNotFoundError(node->file, node->line, node->column,
                    "Module '" + node->moduleName + "' not found");
            }
            executeBlock(it->second->body);
        } else {
            executeModule(node->moduleName, node->fileName);
        }
    }
}

void Interpreter::executeRun(std::shared_ptr<RunStmtNode> node) {
    std::string path = node->path;
    if (node->isRelative) {
        fs::path scriptDir = fs::path(scriptPath).parent_path();
        path = (scriptDir / path.substr(2)).string();
    }

    if (!fs::exists(path)) {
        throw FileNotFoundError(node->file, node->line, node->column,
            "File not found: " + path);
    }

#ifdef _WIN32
    ShellExecuteA(NULL, "open", path.c_str(), NULL, NULL, SW_SHOWNORMAL);
#else
    std::string cmd = "\"" + path + "\" &";
    std::system(cmd.c_str());
#endif
}

void Interpreter::executeModule(const std::string& moduleName, const std::string& fileName) {
    std::string filePath = findMslaFile(fileName);
    if (filePath.empty()) {
        throw FileNotFoundError("", 0, 0, "File for module '" + fileName + "' not found");
    }

    std::string source = readFile(filePath);
    if (source.empty()) {
        throw FileNotFoundError(filePath, 0, 0, "Cannot read file: " + filePath);
    }

    Lexer lexer(source, filePath);
    auto tokens = lexer.tokenize();

    Parser parser(tokens);
    auto program = parser.parse();

    for (auto& stmt : program->modules) {
        if (stmt->type == ASTNodeType::DEF_MOD) {
            auto mod = std::static_pointer_cast<DefModNode>(stmt);
            modules[mod->name] = mod;
        }
    }

    executeBlock(program->initCode);
    if (shouldExit) return;

    auto it = modules.find(moduleName);
    if (it == modules.end()) {
        throw ModuleNotFoundError(filePath, 0, 0, "Module '" + moduleName + "' not found in file '" + filePath + "'");
    }
    executeBlock(it->second->body);
}

Value Interpreter::evaluate(std::shared_ptr<ASTNode> node) {
    switch (node->type) {
        case ASTNodeType::LITERAL: {
            auto lit = std::static_pointer_cast<LiteralNode>(node);
            if (lit->litType == LiteralNode::INT_VAL)
                return Value{Value::INT_VAL, lit->intVal, 0.0, ""};
            if (lit->litType == LiteralNode::FLOAT_VAL)
                return Value{Value::FLOAT_VAL, 0, lit->floatVal, ""};
            return Value{Value::STR_VAL, 0, 0.0, lit->strVal};
        }
        case ASTNodeType::BINARY_EXPR: return evaluateBinary(std::static_pointer_cast<BinaryExprNode>(node));
        case ASTNodeType::UNARY_EXPR: return evaluateUnary(std::static_pointer_cast<UnaryExprNode>(node));
        case ASTNodeType::VAR_REF: return evaluateVarRef(std::static_pointer_cast<VarRefNode>(node));
        case ASTNodeType::FUN_VALUE_REF: return evaluateFunValueRef(std::static_pointer_cast<FunValueRefNode>(node));
        case ASTNodeType::TYPE_FUNC: return evaluateTypeFunc(std::static_pointer_cast<TypeFuncNode>(node));
        case ASTNodeType::WPS_FUNC: return evaluateWPSFunc(std::static_pointer_cast<WPSFuncNode>(node));
        default:
            throw SyntaxFormatError(node->file, node->line, node->column,
                "Cannot evaluate node type");
    }
}

Value Interpreter::evaluateBinary(std::shared_ptr<BinaryExprNode> node) {
    Value left = evaluate(node->left);
    Value right = evaluate(node->right);

    bool leftIsStr = left.type == Value::STR_VAL;
    bool rightIsStr = right.type == Value::STR_VAL;
    bool leftIsNum = left.isNumber();
    bool rightIsNum = right.isNumber();

    if (node->op == "+") {
        if (leftIsNum && rightIsNum) {
            if (left.type == Value::FLOAT_VAL || right.type == Value::FLOAT_VAL)
                return Value{Value::FLOAT_VAL, 0, left.asNumber() + right.asNumber(), ""};
            return Value{Value::INT_VAL, left.asInt() + right.asInt(), 0.0, ""};
        }
        if (leftIsStr && rightIsStr)
            return Value{Value::STR_VAL, 0, 0.0, left.strVal + right.strVal};
        throw MixedTypeOperationError(node->file, node->line, node->column,
            "Cannot add number and string types");
    }

    if (node->op == "-") {
        if (leftIsNum && rightIsNum) {
            if (left.type == Value::FLOAT_VAL || right.type == Value::FLOAT_VAL)
                return Value{Value::FLOAT_VAL, 0, left.asNumber() - right.asNumber(), ""};
            return Value{Value::INT_VAL, left.asInt() - right.asInt(), 0.0, ""};
        }
        throw MixedTypeOperationError(node->file, node->line, node->column,
            "Cannot subtract non-numeric types");
    }

    if (node->op == "*") {
        if (leftIsNum && rightIsNum) {
            if (left.type == Value::FLOAT_VAL || right.type == Value::FLOAT_VAL)
                return Value{Value::FLOAT_VAL, 0, left.asNumber() * right.asNumber(), ""};
            return Value{Value::INT_VAL, left.asInt() * right.asInt(), 0.0, ""};
        }
        if (leftIsNum && rightIsStr) {
            int n = static_cast<int>(left.asNumber());
            std::string result;
            for (int i = 0; i < n; i++) result += right.strVal;
            return Value{Value::STR_VAL, 0, 0.0, result};
        }
        if (leftIsStr && rightIsNum) {
            int n = static_cast<int>(right.asNumber());
            std::string result;
            for (int i = 0; i < n; i++) result += left.strVal;
            return Value{Value::STR_VAL, 0, 0.0, result};
        }
        throw MixedTypeOperationError(node->file, node->line, node->column,
            "Cannot multiply these types");
    }

    if (node->op == "/") {
        if (leftIsNum && rightIsNum) {
            double r = right.asNumber();
            if (r == 0) throw MixedTypeOperationError(node->file, node->line, node->column, "Division by zero");
            return Value{Value::FLOAT_VAL, 0, left.asNumber() / r, ""};
        }
        throw MixedTypeOperationError(node->file, node->line, node->column,
            "Cannot divide non-numeric types");
    }

    if (node->op == ">") {
        if (leftIsNum && rightIsNum)
            return Value{Value::INT_VAL, left.asNumber() > right.asNumber() ? 1 : 0, 0.0, ""};
        if (leftIsStr && rightIsStr)
            return Value{Value::INT_VAL, left.strVal > right.strVal ? 1 : 0, 0.0, ""};
        throw MixedTypeOperationError(node->file, node->line, node->column, "Cannot compare mixed types");
    }
    if (node->op == "<") {
        if (leftIsNum && rightIsNum)
            return Value{Value::INT_VAL, left.asNumber() < right.asNumber() ? 1 : 0, 0.0, ""};
        if (leftIsStr && rightIsStr)
            return Value{Value::INT_VAL, left.strVal < right.strVal ? 1 : 0, 0.0, ""};
        throw MixedTypeOperationError(node->file, node->line, node->column, "Cannot compare mixed types");
    }
    if (node->op == ">=") {
        if (leftIsNum && rightIsNum)
            return Value{Value::INT_VAL, left.asNumber() >= right.asNumber() ? 1 : 0, 0.0, ""};
        if (leftIsStr && rightIsStr)
            return Value{Value::INT_VAL, left.strVal >= right.strVal ? 1 : 0, 0.0, ""};
        throw MixedTypeOperationError(node->file, node->line, node->column, "Cannot compare mixed types");
    }
    if (node->op == "<=" || node->op == "=<") {
        if (leftIsNum && rightIsNum)
            return Value{Value::INT_VAL, left.asNumber() <= right.asNumber() ? 1 : 0, 0.0, ""};
        if (leftIsStr && rightIsStr)
            return Value{Value::INT_VAL, left.strVal <= right.strVal ? 1 : 0, 0.0, ""};
        throw MixedTypeOperationError(node->file, node->line, node->column, "Cannot compare mixed types");
    }
    if (node->op == "==") {
        if (leftIsNum && rightIsNum)
            return Value{Value::INT_VAL, left.asNumber() == right.asNumber() ? 1 : 0, 0.0, ""};
        if (leftIsStr && rightIsStr)
            return Value{Value::INT_VAL, left.strVal == right.strVal ? 1 : 0, 0.0, ""};
        throw MixedTypeOperationError(node->file, node->line, node->column, "Cannot compare mixed types");
    }
    if (node->op == "!=") {
        if (leftIsNum && rightIsNum)
            return Value{Value::INT_VAL, left.asNumber() != right.asNumber() ? 1 : 0, 0.0, ""};
        if (leftIsStr && rightIsStr)
            return Value{Value::INT_VAL, left.strVal != right.strVal ? 1 : 0, 0.0, ""};
        throw MixedTypeOperationError(node->file, node->line, node->column, "Cannot compare mixed types");
    }

    throw SyntaxFormatError(node->file, node->line, node->column,
        "Unknown operator '" + node->op + "'");
}

Value Interpreter::evaluateUnary(std::shared_ptr<UnaryExprNode> node) {
    Value val = evaluate(node->operand);
    if (node->op == "-") {
        if (val.type == Value::INT_VAL) return Value{Value::INT_VAL, -val.intVal, 0.0, ""};
        if (val.type == Value::FLOAT_VAL) return Value{Value::FLOAT_VAL, 0, -val.floatVal, ""};
        throw MixedTypeOperationError(node->file, node->line, node->column,
            "Cannot negate non-numeric value");
    }
    throw SyntaxFormatError(node->file, node->line, node->column, "Unknown unary operator");
}

Value Interpreter::evaluateVarRef(std::shared_ptr<VarRefNode> node) {
    // 矩阵变量引用，支持变量索引部分越界
    if (node->isMatrix) {
        if (matrixVars.find(node->name) == matrixVars.end()) {
            throw UndefinedVariableError(node->file, node->line, node->column,
                "Matrix variable '" + node->name + "' not defined");
        }
        auto& mat = matrixVars[node->name];
        std::vector<int> indices;
        bool anyInBounds = false;
        for (size_t di = 0; di < node->matrixIdx.dims.size(); di++) {
            auto& dim = node->matrixIdx.dims[di];
            if (dim.isRange) {
                throw SyntaxFormatError(node->file, node->line, node->column,
                    "Cannot use range index to get single value");
            }
            int idx = resolveDimIndex(dim, false);
            if (idx >= 0 && idx < mat.dimSizes[di]) anyInBounds = true;
            indices.push_back(idx);
        }
        if (!anyInBounds) {
            bool hasVarIndex = false;
            for (auto& dim : node->matrixIdx.dims) {
                if (dim.isVarIndexed) { hasVarIndex = true; break; }
            }
            if (!hasVarIndex) {
                throw MatrixIndexOutOfBoundsError(node->file, node->line, node->column,
                    "Matrix index out of bounds for '" + node->name + "'");
            }
            for (size_t d = 0; d < indices.size(); d++) {
                indices[d] = std::max(0, std::min(indices[d], mat.dimSizes[d] - 1));
            }
        }
        return mat.data[mat.flatIndex(indices)];
    }

    if (variables.find(node->name) != variables.end()) {
        return variables[node->name];
    }

    if (matrixVars.find(node->name) != matrixVars.end()) {
        return matrixVars[node->name].data[0];
    }

    if (funInstances.find(node->name) != funInstances.end()) {
        return Value{Value::INT_VAL, 0, 0.0, ""};
    }

    throw UndefinedVariableError(node->file, node->line, node->column,
        "Variable '" + node->name + "' not defined");
}

Value Interpreter::evaluateFunValueRef(std::shared_ptr<FunValueRefNode> node) {
    if (node->isInstanceMatrix) {
        if (funInstanceMatrices.find(node->instanceName) == funInstanceMatrices.end()) {
            throw UninstantiatedFunError(node->file, node->line, node->column,
                "Function instance matrix '" + node->instanceName + "' not instantiated");
        }
        auto& mat = funInstanceMatrices[node->instanceName];
        std::vector<int> indices;
        for (auto& dim : node->instanceMatrixIdx.dims) {
            if (dim.isRange) {
                throw SyntaxFormatError(node->file, node->line, node->column,
                    "Cannot use range index to get single function value");
            }
            int idx = resolveDimIndex(dim, false);
            indices.push_back(idx);
        }
        int flat = 0;
        int mult = 1;
        for (int i = static_cast<int>(mat.dimSizes.size()) - 1; i >= 0; i--) {
            flat += indices[i] * mult;
            mult *= mat.dimSizes[i];
        }
        std::string key = node->instanceName + "#" + std::to_string(flat);
        if (funInstances.find(key) == funInstances.end()) {
            throw UninstantiatedFunError(node->file, node->line, node->column,
                "Function instance not found in matrix");
        }
        auto& inst = funInstances[key];
        if (inst.values.find(node->valueName) == inst.values.end()) {
            throw UndefinedVariableError(node->file, node->line, node->column,
                "Value '" + node->valueName + "' not found in function instance");
        }
        auto& fv = inst.values[node->valueName];
        if (fv.isLinked) return getLinkedValue(fv.linkVarName);
        return fv.value;
    }

    if (funInstances.find(node->instanceName) == funInstances.end()) {
        throw UninstantiatedFunError(node->file, node->line, node->column,
            "Function instance '" + node->instanceName + "' not instantiated");
    }
    auto& inst = funInstances[node->instanceName];
    if (inst.values.find(node->valueName) == inst.values.end()) {
        throw UndefinedVariableError(node->file, node->line, node->column,
            "Value '" + node->valueName + "' not found in function instance '" + node->instanceName + "'");
    }
    auto& fv = inst.values[node->valueName];
    if (fv.isLinked) return getLinkedValue(fv.linkVarName);
    return fv.value;
}

Value Interpreter::evaluateTypeFunc(std::shared_ptr<TypeFuncNode> node) {
    Value arg = evaluate(node->arg);
    if (node->funcName == "str") {
        return Value{Value::STR_VAL, 0, 0.0, arg.toString()};
    }
    if (node->funcName == "int") {
        if (arg.type == Value::STR_VAL) {
            try { return Value{Value::INT_VAL, std::stoll(arg.strVal), 0.0, ""}; }
            catch (...) { return Value{Value::INT_VAL, 0, 0.0, ""}; }
        }
        return Value{Value::INT_VAL, arg.asInt(), 0.0, ""};
    }
    if (node->funcName == "float") {
        if (arg.type == Value::STR_VAL) {
            try { return Value{Value::FLOAT_VAL, 0, std::stod(arg.strVal), ""}; }
            catch (...) { return Value{Value::FLOAT_VAL, 0, 0.0, ""}; }
        }
        return Value{Value::FLOAT_VAL, 0, arg.asNumber(), ""};
    }
    if (node->funcName == "len") {
        if (arg.type == Value::STR_VAL)
            return Value{Value::INT_VAL, static_cast<long long>(arg.strVal.size()), 0.0, ""};
        if (matrixVars.find(node->arg->type == ASTNodeType::VAR_REF ?
                std::static_pointer_cast<VarRefNode>(node->arg)->name : "") != matrixVars.end()) {
            auto& mat = matrixVars[std::static_pointer_cast<VarRefNode>(node->arg)->name];
            return Value{Value::INT_VAL, static_cast<long long>(mat.totalSize()), 0.0, ""};
        }
        throw TypeMismatchError(node->file, node->line, node->column,
            "len() requires string or matrix argument");
    }
    throw SyntaxFormatError(node->file, node->line, node->column,
        "Unknown type function '" + node->funcName + "'");
}

Value Interpreter::evaluateWPSFunc(std::shared_ptr<WPSFuncNode> node) {
    if (node->funcName == "SUM") {
        if (node->arg->type == ASTNodeType::VAR_REF) {
            auto varRef = std::static_pointer_cast<VarRefNode>(node->arg);
            if (matrixVars.find(varRef->name) != matrixVars.end()) {
                auto& mat = matrixVars[varRef->name];
                double sum = 0;
                bool hasFloat = false;
                for (auto& v : mat.data) {
                    if (v.type == Value::FLOAT_VAL) hasFloat = true;
                    sum += v.asNumber();
                }
                if (hasFloat) return Value{Value::FLOAT_VAL, 0, sum, ""};
                return Value{Value::INT_VAL, static_cast<long long>(sum), 0.0, ""};
            }
        }
        Value arg = evaluate(node->arg);
        return arg;
    }
    if (node->funcName == "AVERAGE") {
        if (node->arg->type == ASTNodeType::VAR_REF) {
            auto varRef = std::static_pointer_cast<VarRefNode>(node->arg);
            if (matrixVars.find(varRef->name) != matrixVars.end()) {
                auto& mat = matrixVars[varRef->name];
                double sum = 0;
                for (auto& v : mat.data) sum += v.asNumber();
                return Value{Value::FLOAT_VAL, 0, sum / mat.totalSize(), ""};
            }
        }
        Value arg = evaluate(node->arg);
        return Value{Value::FLOAT_VAL, 0, arg.asNumber(), ""};
    }
    throw SyntaxFormatError(node->file, node->line, node->column,
        "Unknown WPS function '" + node->funcName + "'");
}

bool Interpreter::evaluateCondition(const ConditionGroup& group) {
    for (auto& andGroup : group.orGroups) {
        bool allTrue = true;
        for (auto& expr : andGroup) {
            if (!isTruthy(evaluate(expr))) {
                allTrue = false;
                break;
            }
        }
        if (allTrue) return true;
    }
    return false;
}

bool Interpreter::isTruthy(const Value& v) {
    if (v.type == Value::INT_VAL) return v.intVal != 0;
    if (v.type == Value::FLOAT_VAL) return v.floatVal != 0.0;
    if (v.type == Value::STR_VAL) return !v.strVal.empty();
    return false;
}

Value Interpreter::getVariable(const std::string& name) {
    if (variables.find(name) != variables.end()) return variables[name];
    return Value{Value::INT_VAL, 0, 0.0, ""};
}

void Interpreter::setVariable(const std::string& name, const Value& val) {
    variables[name] = val;
}

int Interpreter::resolveDimIndex(const MatrixDimension& dim, bool isEnd) {
    if (isEnd) {
        if (dim.endIsVar) {
            auto it = variables.find(dim.endVarName);
            if (it == variables.end()) throw UndefinedVariableError("", 0, 0,
                "Variable '" + dim.endVarName + "' not defined for matrix index");
            return static_cast<int>(it->second.asInt());
        }
        return dim.endIndex;
    }
    if (dim.startIsVar) {
        auto it = variables.find(dim.startVarName);
        if (it == variables.end()) throw UndefinedVariableError("", 0, 0,
            "Variable '" + dim.startVarName + "' not defined for matrix index");
        return static_cast<int>(it->second.asInt());
    }
    return dim.startIndex;
}

std::vector<int> Interpreter::getMatrixDimSizes(const MatrixIndex& idx, const std::vector<int>& fullDims) {
    std::vector<int> sizes;
    for (size_t i = 0; i < idx.dims.size() && i < fullDims.size(); i++) {
        auto& dim = idx.dims[i];
        if (dim.isRange) {
            int start = resolveDimIndex(dim, false);
            int end = resolveDimIndex(dim, true);
            sizes.push_back(end - start + 1);
        } else {
            sizes.push_back(1);
        }
    }
    return sizes;
}

void Interpreter::collectMatrixRangeIndices(const MatrixIndex& idx, int dimIdx,
    std::vector<int> current, std::vector<std::vector<int>>& result) {
    if (dimIdx >= static_cast<int>(idx.dims.size())) {
        result.push_back(current);
        return;
    }
    auto& dim = idx.dims[dimIdx];
    if (dim.isRange) {
        int start = resolveDimIndex(dim, false);
        int end = resolveDimIndex(dim, true);
        for (int i = start; i <= end; i++) {
            auto next = current;
            next.push_back(i);
            collectMatrixRangeIndices(idx, dimIdx + 1, next, result);
        }
    // 非范围索引，支持变量索引部分越界
    } else {
        int idx_val = resolveDimIndex(dim, false);
        auto next = current;
        next.push_back(idx_val);
        collectMatrixRangeIndices(idx, dimIdx + 1, next, result);
    }
}

void Interpreter::printMatrix(const std::string& name, const MatrixIndex& idx) {
    if (matrixVars.find(name) == matrixVars.end()) {
        throw UndefinedVariableError("", 0, 0, "Matrix '" + name + "' not defined");
    }
    auto& mat = matrixVars[name];

    int varyingCount = 0;
    int firstVarying = -1, secondVarying = -1;
    for (size_t i = 0; i < idx.dims.size(); i++) {
        if (idx.dims[i].isRange) {
            varyingCount++;
            if (firstVarying == -1) firstVarying = static_cast<int>(i);
            else if (secondVarying == -1) secondVarying = static_cast<int>(i);
        }
    }

    if (varyingCount > 2) {
        throw MatrixPrintDimensionError("", 0, 0,
            "Cannot print matrix with more than 2 varying dimensions");
    }

    std::vector<std::vector<int>> indices;
    collectMatrixRangeIndices(idx, 0, {}, indices);

    std::vector<std::string> strValues;
    size_t maxLen = 0;
    for (auto& index : indices) {
        std::string s = mat.data[mat.flatIndex(index)].toString();
        if (s.size() > maxLen) maxLen = s.size();
        strValues.push_back(s);
    }

    if (varyingCount == 0 || varyingCount == 1) {
        for (auto& s : strValues) {
            std::cout << s << std::endl;
        }
        return;
    }

    int dim1Start = resolveDimIndex(idx.dims[firstVarying], false);
    int dim1End = resolveDimIndex(idx.dims[firstVarying], true);
    int dim2Start = resolveDimIndex(idx.dims[secondVarying], false);
    int dim2End = resolveDimIndex(idx.dims[secondVarying], true);
    int dim1Size = dim1End - dim1Start + 1;
    int dim2Size = dim2End - dim2Start + 1;

    int hDim, vDim;
    if (dim1Size <= dim2Size) {
        hDim = firstVarying;
        vDim = secondVarying;
    } else {
        hDim = secondVarying;
        vDim = firstVarying;
    }

    int hStart = resolveDimIndex(idx.dims[hDim], false);
    int hEnd = resolveDimIndex(idx.dims[hDim], true);
    int vStart = resolveDimIndex(idx.dims[vDim], false);
    int vEnd = resolveDimIndex(idx.dims[vDim], true);
    int hSize = hEnd - hStart + 1;
    int vSize = vEnd - vStart + 1;

    for (int v = 0; v < vSize; v++) {
        for (int h = 0; h < hSize; h++) {
            std::vector<int> queryIdx;
            for (size_t d = 0; d < idx.dims.size(); d++) {
                if (static_cast<int>(d) == hDim) queryIdx.push_back(hStart + h);
                else if (static_cast<int>(d) == vDim) queryIdx.push_back(vStart + v);
                else queryIdx.push_back(resolveDimIndex(idx.dims[d], false));
            }
            std::string s = mat.data[mat.flatIndex(queryIdx)].toString();
            if (h > 0) std::cout << " ";
            std::cout << s;
            size_t pad = maxLen - s.size();
            for (size_t p = 0; p < pad; p++) std::cout << " ";
        }
        std::cout << std::endl;
    }
}

void Interpreter::printFunMatrix(const std::string& instanceName, const MatrixIndex& idx, const std::string& valueName) {
    if (funInstanceMatrices.find(instanceName) == funInstanceMatrices.end()) {
        throw UninstantiatedFunError("", 0, 0,
            "Function instance matrix '" + instanceName + "' not instantiated");
    }
    auto& mat = funInstanceMatrices[instanceName];

    int varyingCount = 0;
    int firstVarying = -1, secondVarying = -1;
    for (size_t i = 0; i < idx.dims.size(); i++) {
        if (idx.dims[i].isRange) {
            varyingCount++;
            if (firstVarying == -1) firstVarying = static_cast<int>(i);
            else if (secondVarying == -1) secondVarying = static_cast<int>(i);
        }
    }

    if (varyingCount > 2) {
        throw MatrixPrintDimensionError("", 0, 0,
            "Cannot print function matrix with more than 2 varying dimensions");
    }

    std::vector<std::vector<int>> indices;
    collectMatrixRangeIndices(idx, 0, {}, indices);

    std::vector<std::string> strValues;
    size_t maxLen = 0;
    for (auto& index : indices) {
        int flat = 0;
        int mult = 1;
        for (int i = static_cast<int>(mat.dimSizes.size()) - 1; i >= 0; i--) {
            flat += index[i] * mult;
            mult *= mat.dimSizes[i];
        }
        std::string key = instanceName + "#" + std::to_string(flat);
        std::string s;
        if (funInstances.find(key) != funInstances.end()) {
            auto& inst = funInstances[key];
            if (inst.values.find(valueName) != inst.values.end()) {
                auto& fv = inst.values[valueName];
                if (fv.isLinked) s = getLinkedValue(fv.linkVarName).toString();
                else s = fv.value.toString();
            } else {
                s = "0";
            }
        } else {
            s = "0";
        }
        if (s.size() > maxLen) maxLen = s.size();
        strValues.push_back(s);
    }

    if (varyingCount == 0 || varyingCount == 1) {
        for (auto& s : strValues) std::cout << s << std::endl;
        return;
    }

    int hDim, vDim;
    int dim1Start = resolveDimIndex(idx.dims[firstVarying], false);
    int dim1End = resolveDimIndex(idx.dims[firstVarying], true);
    int dim2Start = resolveDimIndex(idx.dims[secondVarying], false);
    int dim2End = resolveDimIndex(idx.dims[secondVarying], true);
    int dim1Size = dim1End - dim1Start + 1;
    int dim2Size = dim2End - dim2Start + 1;

    if (dim1Size <= dim2Size) { hDim = firstVarying; vDim = secondVarying; }
    else { hDim = secondVarying; vDim = firstVarying; }

    int hStart = resolveDimIndex(idx.dims[hDim], false);
    int hEnd = resolveDimIndex(idx.dims[hDim], true);
    int vStart = resolveDimIndex(idx.dims[vDim], false);
    int vEnd = resolveDimIndex(idx.dims[vDim], true);
    int hSize = hEnd - hStart + 1;
    int vSize = vEnd - vStart + 1;

    for (int v = 0; v < vSize; v++) {
        for (int h = 0; h < hSize; h++) {
            std::vector<int> queryIdx;
            for (size_t d = 0; d < idx.dims.size(); d++) {
                if (static_cast<int>(d) == hDim) queryIdx.push_back(hStart + h);
                else if (static_cast<int>(d) == vDim) queryIdx.push_back(vStart + v);
                else queryIdx.push_back(resolveDimIndex(idx.dims[d], false));
            }
            int flat = 0;
            int mult = 1;
            for (int i = static_cast<int>(mat.dimSizes.size()) - 1; i >= 0; i--) {
                flat += queryIdx[i] * mult;
                mult *= mat.dimSizes[i];
            }
            std::string key = instanceName + "#" + std::to_string(flat);
            std::string s;
            if (funInstances.find(key) != funInstances.end() &&
                funInstances[key].values.find(valueName) != funInstances[key].values.end()) {
                auto& fv = funInstances[key].values[valueName];
                s = fv.isLinked ? getLinkedValue(fv.linkVarName).toString() : fv.value.toString();
            } else {
                s = "0";
            }
            if (h > 0) std::cout << " ";
            std::cout << s;
            size_t pad = maxLen - s.size();
            for (size_t p = 0; p < pad; p++) std::cout << " ";
        }
        std::cout << std::endl;
    }
}

Value Interpreter::getLinkedValue(const std::string& varName) {
    if (variables.find(varName) != variables.end()) return variables[varName];
    if (matrixVars.find(varName) != matrixVars.end()) return matrixVars[varName].data[0];
    return Value{Value::INT_VAL, 0, 0.0, ""};
}

void Interpreter::setLinkedValue(const std::string& varName, const Value& val) {
    if (variables.find(varName) != variables.end()) variables[varName] = val;
}

void Interpreter::checkTypeCompatibility(VarType varType, const Value& val, int line, int col, const std::string& file) {
    if (varType == VarType::NUMBER && val.type == Value::STR_VAL) {
        throw TypeMismatchError(file, line, col, "Cannot assign string to number variable");
    }
    if (varType == VarType::STRING && val.isNumber()) {
        throw TypeMismatchError(file, line, col, "Cannot assign number to string variable");
    }
}

std::string Interpreter::findMslaFile(const std::string& name) {
    fs::path scriptDir = fs::path(scriptPath).parent_path();

    for (const auto& entry : fs::recursive_directory_iterator(scriptDir)) {
        if (entry.is_regular_file() && entry.path().extension() == ".msla") {
            if (entry.path().stem() == name) {
                return entry.path().string();
            }
        }
    }

    std::string directPath = (scriptDir / (name + ".msla")).string();
    if (fs::exists(directPath)) return directPath;

    return "";
}

std::string Interpreter::readFile(const std::string& path) {
    std::ifstream file(path);
    if (!file.is_open()) return "";
    std::stringstream ss;
    ss << file.rdbuf();
    return ss.str();
}

// 执行import语句
void Interpreter::executeImport(std::shared_ptr<ImportStmtNode> node) {
    if (aliasToPackage.find(node->alias) != aliasToPackage.end()) {
        throw DuplicateAliasError(node->file, node->line, node->column,
            "Alias '" + node->alias + "' already used");
    }

    std::string dllPath = findDllFile(node->packageName);
    if (dllPath.empty()) {
        throw PackageNotFoundError(node->file, node->line, node->column,
            "Package '" + node->packageName + "' not found (missing .dll)");
    }

    PackageInfo pkg;
    pkg.name = node->packageName;
    pkg.dllPath = dllPath;

#ifdef _WIN32
    HMODULE hDll = LoadLibraryA(dllPath.c_str());
    if (!hDll) {
        throw PackageNotFoundError(node->file, node->line, node->column,
            "Cannot load package '" + node->packageName + "' from " + dllPath);
    }

    // 获取包信息函数
    typedef const char* (*GetInfoFunc)();
    auto getInfo = (GetInfoFunc)GetProcAddress(hDll, "msla_get_info");
    if (getInfo) {
        std::string info = getInfo();
        size_t start = 0;
        for (size_t i = 0; i <= info.size(); i++) {
            if (i == info.size() || info[i] == '|') {
                std::string part = info.substr(start, i - start);
                if (!part.empty()) {
                    if (part.substr(0, 5) == "lang:") {
                        pkg.codeType = part.substr(5);
                    } else {
                        size_t spacePos = part.find(' ');
                        if (spacePos != std::string::npos) {
                            std::string funcName = part.substr(0, spacePos);
                            std::string rest = part.substr(spacePos + 1);
                            PackageFuncInfo fi;
                            fi.name = funcName;
                            if (rest.substr(0, 8) == "grammar:") {
                                fi.grammar = rest.substr(8);
                            }
                            pkg.functions[funcName] = fi;
                        } else {
                            PackageFuncInfo fi;
                            fi.name = part;
                            pkg.functions[part] = fi;
                        }
                    }
                }
                start = i + 1;
            }
        }
    }
#endif

    packages[node->packageName + "#" + node->alias] = pkg;
    aliasToPackage[node->alias] = node->packageName + "#" + node->alias;
}

// 执行包调用语句
void Interpreter::executePackageCall(std::shared_ptr<PackageCallNode> node) {
    auto it = aliasToPackage.find(node->alias);
    if (it == aliasToPackage.end()) {
        throw PackageNotFoundError(node->file, node->line, node->column,
            "Package alias '" + node->alias + "' not imported");
    }

    auto& pkg = packages[it->second];

    auto funcIt = pkg.functions.find(node->funcName);
    if (funcIt == pkg.functions.end()) {
        throw PackageFunctionNotFoundError(node->file, node->line, node->column,
            "Function '" + node->funcName + "' not found in package '" + pkg.name + "'");
    }

    // 准备参数
    std::vector<std::string> argStrings;
    for (auto& arg : node->args) {
        Value val = evaluate(arg);
        argStrings.push_back(val.toString());
    }

#ifdef _WIN32
    // 调用DLL中的函数
    std::string funcSymbol = "msla_" + node->funcName;
    HMODULE hDll = LoadLibraryA(pkg.dllPath.c_str());
    if (!hDll) {
        throw PackageNotFoundError(node->file, node->line, node->column,
            "Cannot load package DLL");
    }

    typedef const char* (*CallFunc)(const char** args, int argc);
    auto callFunc = (CallFunc)GetProcAddress(hDll, funcSymbol.c_str());
    if (!callFunc) {
        throw PackageFunctionNotFoundError(node->file, node->line, node->column,
            "Cannot find function '" + node->funcName + "' in package DLL");
    }

    std::vector<const char*> cArgs;
    for (auto& s : argStrings) cArgs.push_back(s.c_str());
    const char* result = callFunc(cArgs.data(), static_cast<int>(cArgs.size()));
    if (result && strlen(result) > 0) {
        std::cout << result << std::endl;
    }
#else
    // 非Windows平台的简单实现
    std::cout << "[Package call: " << pkg.name << "." << node->funcName << "(";
    for (size_t i = 0; i < argStrings.size(); i++) {
        if (i > 0) std::cout << ", ";
        std::cout << argStrings[i];
    }
    std::cout << ")]" << std::endl;
#endif
}

// 查找DLL文件
std::string Interpreter::findDllFile(const std::string& packageName) {
    fs::path scriptDir = fs::path(scriptPath).parent_path();

    // 在脚本目录及子目录中查找 <packageName>.dll
    try {
        for (const auto& entry : fs::recursive_directory_iterator(scriptDir, fs::directory_options::skip_permission_denied)) {
            if (entry.is_regular_file() && entry.path().extension() == ".dll") {
                if (entry.path().stem() == packageName) {
                    return entry.path().string();
                }
            }
        }
    } catch (const fs::filesystem_error&) {
        // 忽略文件系统遍历错误
    }

    std::string directPath = (scriptDir / (packageName + ".dll")).string();
    if (fs::exists(directPath)) return directPath;

    return "";
}
