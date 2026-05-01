# MSLA Interpreter User Guide

**Version**: Beta 0.0.2  
**Language Full Name**: Models Language  
**File Extension**: `.msla`

---

## 1. Feature Overview

The MSLA Interpreter is a command-line tool that can directly execute `.msla` program files. It supports the full MSLA Beta 0.0.2 syntax, including variable definition and assignment, matrix operations (with variable indexing and out-of-bounds tolerance), function type definition and instantiation, modular execution, conditional statements, jump/call invocations, and the package system (importing DLL packages). When a program contains errors, the interpreter outputs detailed error messages including the error type, error location (file name, line number, column number), and the cause of the error.

---

## 2. Usage

### 2.1 Basic Usage

```
msla_interpreter.exe <文件路径>.msla
```

### 2.2 Example

```
msla_interpreter.exe test.msla
```

### 2.3 No-Argument Invocation

If no file path argument is provided, the interpreter outputs usage instructions:

```
MSLA Interpreter v0.0.2
Usage: msla_interpreter <file.msla>
```

### 2.4 File Not Found

If the specified `.msla` file does not exist, the interpreter outputs an error message and exits.

---

## 3. Supported Syntax Overview

### 3.1 Variable Definition and Assignment

```msla
def var count -n          # 定义数字变量
def var name -w           # 定义字符串变量
ass var count = 10        # 赋值
ass var count = 1 + 2     # 运算赋值
```

### 3.2 Variable Matrices

```msla
def var mat_[0:2]_[0:2] -n       # 定义 3x3 数字矩阵
ass var mat = 1                    # 整体赋值
ass var mat_[0]_[2] = 5           # 坐标赋值
ass var mat_[0]_[0:2] = other     # 范围赋值
```

**Variable Indexing Rules**: Variables may be used as indices within matrix dimensions (e.g., `mat_[0]_[idx]`, where `idx` is a variable). When a variable index causes partial out-of-bounds access across dimensions, no error is raised as long as at least one valid value exists; the out-of-bounds portions are ignored.

### 3.3 Function Type Definition and Instantiation

```msla
def fun player -n
    def a = b + c
    def d = SUM(e)
defend

ins player player1              # 实例化
ins player player3_[0:2]_[0:2]  # 函数矩阵实例化

ass fun player1
    c = 1
    a, b = 1
    e = in            # 链接变量
assend
```

### 3.4 Function Value Reference

```msla
player1:a                 # 引用实例函数值
player3_[0]_[0]:a        # 引用函数矩阵值
```

### 3.5 Modules

```msla
def mod amod -main        # -main 表示初始化后首先执行
    print("hello")
    jump @ bmod           # 跳转到本文件内的 bmod 模块
modend

def mod bmod
    call @ amod           # 在新进程中调用 amod 模块
modend
```

### 3.6 Conditional Statements

```msla
if(x > 5)
    print("yes")
else if(x > 3)
    print("maybe")
else
    print("no")
ifend

# 多条件逻辑：逗号=AND，分号=OR
if(a > 0, b > 0 ; c > 0)   # (a>0 AND b>0) OR c>0
    print("condition met")
ifend
```

### 3.7 print Statement

```msla
print("hello")                    # 打印字符串
print(len("hello"))               # 打印函数结果
print(mat)                        # 打印变量矩阵（二维表格）
print(mat_[0:2]_[0])             # 打印指定范围
print(player3_[0:2]_[0:2]:a)     # 打印函数矩阵值
```

### 3.8 input Statement

```msla
input in ("请输入：") -n    # 限制输入数字
input name ("姓名：") -w   # 限制输入文字
```

### 3.9 Other Commands

```msla
run "C:\game\main.exe"     # 运行外部程序
run "\\script\test.py"     # 相对路径运行
exit()                      # 退出程序
```

### 3.10 Python-Type Function Compatibility

```msla
str(123)       # "123"
int("456")     # 456
float("3.14")  # 3.14
len("hello")   # 5
```

### 3.11 Package System

```msla
import base64 .a           # 导入 base64 包，别名 .a
.a encode "hello"          # 调用 .a 包的 encode 函数，传入参数 "hello"
.a decode "aGVsbG8="      # 调用 .a 包的 decode 函数
```

**Package Notes**:
- `import <package_name> <alias>`: Loads the DLL package with the specified name and registers an alias.
- The alias format is `.x`, `.xx`, or `.xxx` (1 to 3 letters).
- Packages are provided as DLL files and must be placed in the same directory as the script or in a subdirectory.
- The DLL must export the `msla_get_info` function (which returns the list of available functions) as well as each functional function.

---

## 4. Error Messages

When a program encounters an error, the interpreter outputs a formatted error message:

```
[报错类型] 文件名:行号:列号: 报错原因
```

### 4.1 All Error Types

| Error Type | Description |
|------------|-------------|
| UndefinedVariableError | An undefined variable was used |
| UndefinedFunTypeError | An undefined function type was instantiated |
| UninstantiatedFunError | An uninstantiated function was used |
| TypeMismatchError | Type mismatch (e.g., assigning a string to a numeric variable) |
| DimensionMismatchWarning | Matrix dimension mismatch (warning, program continues) |
| DuplicateDefinitionError | Duplicate definition of a variable or function type with the same name |
| DuplicateInstantiationError | Duplicate instantiation of an instance function with the same name |
| MissingParameterError | `def var` is missing the `-n` or `-w` parameter |
| SyntaxFormatError | Syntax format error |
| AssignmentDirectionError | Assignment direction/master-slave relationship error |
| ModuleNotFoundError | Jump/call to a non-existent module |
| FileNotFoundError | File specified in `run` command not found |
| MixedTypeOperationError | Illegal operation between different types |
| MatrixIndexOutOfBoundsError | Matrix index out of bounds |
| MismatchedEndMarkerError | Mismatched closing marker |
| MatrixPrintDimensionError | Printing a matrix with more than two varying dimensions |
| MatrixPrintMixedContentError | Mixed other content when printing a matrix |
| PackageNotFoundError | Imported package DLL file not found |
| DuplicateAliasError | Duplicate alias used in `import` |
| PackageFunctionNotFoundError | The called package function does not exist |

### 4.2 Error Examples

```
[UndefinedVariableError] test.msla:5:13: Variable 'unknown' not defined
[TypeMismatchError] test.msla:3:9: Cannot assign string to number variable
[MissingParameterError] test.msla:1:14: Missing parameter -n or -w in def var
[DuplicateDefinitionError] test.msla:2:9: Variable 'x' already defined
```

---

## 5. Example Programs

### 5.1 Simple Calculation

```msla
def var x -n
def var y -n
ass var x = 10
ass var y = 3
print(x + y)
print(x * y)
exit()
```

Output:
```
13
30
```

### 5.2 Functions and Modules

```msla
def fun main -n
    def z = a + b + c
defend

def var in -n
def var out -n

ins main startfun

ass fun startfun
    a, b = 1
    c = in
    z = out
assend

def mod amod -main
    print("hi")
    ass var in = 1
    if(out > 3)
        print("yes1")
    ifend
    jump @ bmod
modend

def mod bmod
    if(startfun:z > 4 , in > 4 ; in > 6)
        print("yes2")
    else
        print("yes4")
    ifend
    exit()
modend
```

Output:
```
hi
yes4
```

### 5.3 Matrix Operations

```msla
def var mat_[0:2]_[0:2] -n
ass var mat = 5
ass var mat_[1]_[1] = 99
print(mat)
exit()
```

Output:
```
5 5 5
5 99 5
5 5 5
```
