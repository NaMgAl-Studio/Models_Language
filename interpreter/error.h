#pragma once

#include <string>
#include <stdexcept>
#include "common.h"

class MslaError : public std::exception {
public:
    ErrorCode errorType;
    std::string file;
    int line;
    int column;
    std::string message;
    mutable std::string formatted;

    MslaError(ErrorCode type, const std::string& file, int line, int col, const std::string& msg)
        : errorType(type), file(file), line(line), column(col), message(msg) {}

    virtual ~MslaError() = default;

    virtual std::string errorName() const { return "Error"; }

    const char* what() const noexcept override {
        formatted = "[" + errorName() + "] " + file + ":" + std::to_string(line) + ":" + std::to_string(column) + ": " + message;
        return formatted.c_str();
    }
};

class UndefinedVariableError : public MslaError {
public:
    UndefinedVariableError(const std::string& f, int l, int c, const std::string& msg)
        : MslaError(ErrorCode::UNDEFINED_VARIABLE, f, l, c, msg) {}
    std::string errorName() const override { return "UndefinedVariableError"; }
};

class UndefinedFunTypeError : public MslaError {
public:
    UndefinedFunTypeError(const std::string& f, int l, int c, const std::string& msg)
        : MslaError(ErrorCode::UNDEFINED_FUN_TYPE, f, l, c, msg) {}
    std::string errorName() const override { return "UndefinedFunTypeError"; }
};

class UninstantiatedFunError : public MslaError {
public:
    UninstantiatedFunError(const std::string& f, int l, int c, const std::string& msg)
        : MslaError(ErrorCode::UNINSTANTIATED_FUN, f, l, c, msg) {}
    std::string errorName() const override { return "UninstantiatedFunError"; }
};

class TypeMismatchError : public MslaError {
public:
    TypeMismatchError(const std::string& f, int l, int c, const std::string& msg)
        : MslaError(ErrorCode::TYPE_MISMATCH, f, l, c, msg) {}
    std::string errorName() const override { return "TypeMismatchError"; }
};

class DimensionMismatchWarning : public MslaError {
public:
    DimensionMismatchWarning(const std::string& f, int l, int c, const std::string& msg)
        : MslaError(ErrorCode::DIMENSION_MISMATCH, f, l, c, msg) {}
    std::string errorName() const override { return "DimensionMismatchWarning"; }
};

class DuplicateDefinitionError : public MslaError {
public:
    DuplicateDefinitionError(const std::string& f, int l, int c, const std::string& msg)
        : MslaError(ErrorCode::DUPLICATE_DEFINITION, f, l, c, msg) {}
    std::string errorName() const override { return "DuplicateDefinitionError"; }
};

class DuplicateInstantiationError : public MslaError {
public:
    DuplicateInstantiationError(const std::string& f, int l, int c, const std::string& msg)
        : MslaError(ErrorCode::DUPLICATE_INSTANTIATION, f, l, c, msg) {}
    std::string errorName() const override { return "DuplicateInstantiationError"; }
};

class MissingParameterError : public MslaError {
public:
    MissingParameterError(const std::string& f, int l, int c, const std::string& msg)
        : MslaError(ErrorCode::MISSING_PARAMETER, f, l, c, msg) {}
    std::string errorName() const override { return "MissingParameterError"; }
};

class SyntaxFormatError : public MslaError {
public:
    SyntaxFormatError(const std::string& f, int l, int c, const std::string& msg)
        : MslaError(ErrorCode::SYNTAX_FORMAT, f, l, c, msg) {}
    std::string errorName() const override { return "SyntaxFormatError"; }
};

class AssignmentDirectionError : public MslaError {
public:
    AssignmentDirectionError(const std::string& f, int l, int c, const std::string& msg)
        : MslaError(ErrorCode::ASSIGNMENT_DIRECTION, f, l, c, msg) {}
    std::string errorName() const override { return "AssignmentDirectionError"; }
};

class ModuleNotFoundError : public MslaError {
public:
    ModuleNotFoundError(const std::string& f, int l, int c, const std::string& msg)
        : MslaError(ErrorCode::MODULE_NOT_FOUND, f, l, c, msg) {}
    std::string errorName() const override { return "ModuleNotFoundError"; }
};

class FileNotFoundError : public MslaError {
public:
    FileNotFoundError(const std::string& f, int l, int c, const std::string& msg)
        : MslaError(ErrorCode::FILE_NOT_FOUND, f, l, c, msg) {}
    std::string errorName() const override { return "FileNotFoundError"; }
};

class MixedTypeOperationError : public MslaError {
public:
    MixedTypeOperationError(const std::string& f, int l, int c, const std::string& msg)
        : MslaError(ErrorCode::MIXED_TYPE_OPERATION, f, l, c, msg) {}
    std::string errorName() const override { return "MixedTypeOperationError"; }
};

class MatrixIndexOutOfBoundsError : public MslaError {
public:
    MatrixIndexOutOfBoundsError(const std::string& f, int l, int c, const std::string& msg)
        : MslaError(ErrorCode::MATRIX_INDEX_OUT_OF_BOUNDS, f, l, c, msg) {}
    std::string errorName() const override { return "MatrixIndexOutOfBoundsError"; }
};

class MismatchedEndMarkerError : public MslaError {
public:
    MismatchedEndMarkerError(const std::string& f, int l, int c, const std::string& msg)
        : MslaError(ErrorCode::MISMATCHED_END_MARKER, f, l, c, msg) {}
    std::string errorName() const override { return "MismatchedEndMarkerError"; }
};

class MatrixPrintDimensionError : public MslaError {
public:
    MatrixPrintDimensionError(const std::string& f, int l, int c, const std::string& msg)
        : MslaError(ErrorCode::MATRIX_PRINT_DIMENSION, f, l, c, msg) {}
    std::string errorName() const override { return "MatrixPrintDimensionError"; }
};

class MatrixPrintMixedContentError : public MslaError {
public:
    MatrixPrintMixedContentError(const std::string& f, int l, int c, const std::string& msg)
        : MslaError(ErrorCode::MATRIX_PRINT_MIXED_CONTENT, f, l, c, msg) {}
    std::string errorName() const override { return "MatrixPrintMixedContentError"; }
};

// 包未找到错误
class PackageNotFoundError : public MslaError {
public:
    PackageNotFoundError(const std::string& f, int l, int c, const std::string& msg)
        : MslaError(ErrorCode::PACKAGE_NOT_FOUND, f, l, c, msg) {}
    std::string errorName() const override { return "PackageNotFoundError"; }
};

// 简化名重复错误
class DuplicateAliasError : public MslaError {
public:
    DuplicateAliasError(const std::string& f, int l, int c, const std::string& msg)
        : MslaError(ErrorCode::DUPLICATE_ALIAS, f, l, c, msg) {}
    std::string errorName() const override { return "DuplicateAliasError"; }
};

// 包函数未找到错误
class PackageFunctionNotFoundError : public MslaError {
public:
    PackageFunctionNotFoundError(const std::string& f, int l, int c, const std::string& msg)
        : MslaError(ErrorCode::PACKAGE_FUNCTION_NOT_FOUND, f, l, c, msg) {}
    std::string errorName() const override { return "PackageFunctionNotFoundError"; }
};

class WhileNotInLoopError : public MslaError {
public:
    WhileNotInLoopError(const std::string& f, int l, int c, const std::string& msg)
        : MslaError(ErrorCode::WHILE_NOT_IN_LOOP, f, l, c, msg) {}
    std::string errorName() const override { return "WhileNotInLoopError"; }
};

class CheckpointNotFoundError : public MslaError {
public:
    CheckpointNotFoundError(const std::string& f, int l, int c, const std::string& msg)
        : MslaError(ErrorCode::CHECKPOINT_NOT_FOUND, f, l, c, msg) {}
    std::string errorName() const override { return "CheckpointNotFoundError"; }
};

class KillTargetNotFoundError : public MslaError {
public:
    KillTargetNotFoundError(const std::string& f, int l, int c, const std::string& msg)
        : MslaError(ErrorCode::KILL_TARGET_NOT_FOUND, f, l, c, msg) {}
    std::string errorName() const override { return "KillTargetNotFoundError"; }
};

class PackageGrammarFormatMismatchError : public MslaError {
public:
    PackageGrammarFormatMismatchError(const std::string& f, int l, int c, const std::string& msg)
        : MslaError(ErrorCode::PACKAGE_GRAMMAR_FORMAT_MISMATCH, f, l, c, msg) {}
    std::string errorName() const override { return "PackageGrammarFormatMismatchError"; }
};
