#include "error.h"

std::string errorToString(ErrorCode code) {
    switch (code) {
        case ErrorCode::UNDEFINED_VARIABLE: return "UndefinedVariableError";
        case ErrorCode::UNDEFINED_FUN_TYPE: return "UndefinedFunTypeError";
        case ErrorCode::UNINSTANTIATED_FUN: return "UninstantiatedFunError";
        case ErrorCode::TYPE_MISMATCH: return "TypeMismatchError";
        case ErrorCode::DIMENSION_MISMATCH: return "DimensionMismatchWarning";
        case ErrorCode::DUPLICATE_DEFINITION: return "DuplicateDefinitionError";
        case ErrorCode::DUPLICATE_INSTANTIATION: return "DuplicateInstantiationError";
        case ErrorCode::MISSING_PARAMETER: return "MissingParameterError";
        case ErrorCode::SYNTAX_FORMAT: return "SyntaxFormatError";
        case ErrorCode::ASSIGNMENT_DIRECTION: return "AssignmentDirectionError";
        case ErrorCode::MODULE_NOT_FOUND: return "ModuleNotFoundError";
        case ErrorCode::FILE_NOT_FOUND: return "FileNotFoundError";
        case ErrorCode::MIXED_TYPE_OPERATION: return "MixedTypeOperationError";
        case ErrorCode::MATRIX_INDEX_OUT_OF_BOUNDS: return "MatrixIndexOutOfBoundsError";
        case ErrorCode::MISMATCHED_END_MARKER: return "MismatchedEndMarkerError";
        case ErrorCode::MATRIX_PRINT_DIMENSION: return "MatrixPrintDimensionError";
        case ErrorCode::MATRIX_PRINT_MIXED_CONTENT: return "MatrixPrintMixedContentError";
        case ErrorCode::PACKAGE_NOT_FOUND: return "PackageNotFoundError";
        case ErrorCode::DUPLICATE_ALIAS: return "DuplicateAliasError";
        case ErrorCode::PACKAGE_FUNCTION_NOT_FOUND: return "PackageFunctionNotFoundError";
        case ErrorCode::WHILE_NOT_IN_LOOP: return "WhileNotInLoopError";
        case ErrorCode::CHECKPOINT_NOT_FOUND: return "CheckpointNotFoundError";
        case ErrorCode::KILL_TARGET_NOT_FOUND: return "KillTargetNotFoundError";
        case ErrorCode::PACKAGE_GRAMMAR_FORMAT_MISMATCH: return "PackageGrammarFormatMismatchError";
        default: return "UnknownError";
    }
}
