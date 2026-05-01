# MSLA Packages Maker User Guide

**Version**: Beta 0.0.3
**Tool Name**: MSLA Packages Maker\
**File Extension**: `.mslb`

***

## 1. Function Overview

MSLA Packages Maker is a command-line tool that compiles `.mslb` script files into `.dll` dynamic link libraries, which can then be loaded and invoked by the MSLA interpreter through `import` statements. The package maker reads the package definition in an MSLB script, automatically generates C++ source code conforming to the MSLA interpreter's DLL interface, and then invokes the g++ compiler to compile it into a DLL file.

***

## 2. Usage

### 2.1 Basic Usage

```
msla_packages_maker.exe <文件路径>.mslb
```

After successful compilation, a `.dll` file with the same name will be generated in the directory where the `.mslb` file resides.

### 2.2 Specify Output Directory

```
msla_packages_maker.exe <文件路径>.mslb -o <输出目录>
```

The `-o` parameter can be used to output the generated DLL file to a specified directory.

### 2.3 Examples

```
msla_packages_maker.exe mypkg.mslb
msla_packages_maker.exe mypkg.mslb -o C:\project\libs
```

### 2.4 Calling Without Arguments

If no arguments are provided, the package maker will display usage instructions:

```
MSLA Packages Maker v0.0.3
Usage: packages_maker <file.mslb> [-o <output_dir>]
```

***

## 3. MSLB Script Syntax

### 3.1 Basic Structure

A `.mslb` script consists of the following parts: package name definition, variable definition, code type declaration, and function line definitions.

```
packagename <包名>

def var <变量名> <-n | -w>

code_type <py | cpp | c | msla>

line <语句名>
    grammar: <语法规则>
    content
        <脚本内容>
    contentend
lineend
```

### 3.2 bagname / packagename — Package Name Definition

Defines the name of the package. The compiled DLL file name will be `<package_name>.dll`. Both `bagname` and `packagename` keywords are accepted; `packagename` is recommended.

```mslb
packagename mymath
```

The old keyword is also supported:

```mslb
packagename mymath
```

### 3.3 def var — Package Variable Definition

Defines global variables within the package. These variables can be used by functions inside the package and can also be referenced in MSLA scripts via `<alias>:<variable_name>`.

| Parameter | Meaning          |
| --------- | ---------------- |
| `-n`      | Numeric variable |
| `-w`      | String variable  |

```mslb
def var text -w     # 定义字符串变量
def var count -n    # 定义数字变量
```

### 3.4 code_type — Code Type Declaration

Declares the scripting language used by the functions in the package.

| Type   | Description                                                     |
| ------ | --------------------------------------------------------------- |
| `cpp`  | C++ code, directly embedded in the DLL function body, compiled with g++ |
| `c`    | C language code, directly embedded in the DLL function body, compiled with gcc |
| `msla` | MSLA language code, currently compiled into a DLL in the style of C++ |
| `py`   | Python code, executed at runtime via the Python interpreter     |

```mslb
code_type cpp
```

### 3.5 line / lineend — Function Line Definition

Defines a function in the package. Each `line` block contains two parts: `grammar` (syntax rule) and `content` (script content).

```mslb
line a
    grammar: greet "<name>"
    content
        std::string msg = "Hello, " + name + "!";
        print(msg)
    contentend
lineend
```

### 3.6 grammar — Syntax Rule

Defines the syntax format that users use when calling the function in an MSLA script.

- The first token of the syntax rule is the **function name**, i.e., the name used to call it in an MSLA script
- `<variable_name>` represents a parameter placeholder; the values passed when calling are mapped to the corresponding variables in order

```mslb
grammar: greet "<name>"           # 函数名为 greet，参数为 name
grammar: add "<a>" "<b>"          # 函数名为 add，参数为 a 和 b
grammar: help                     # 函数名为 help，无参数
```

### 3.7 content / contentend — Script Content

Defines the actual execution script for the function, supporting `cpp`, `c`, `msla`, and `py` types.

**C++ Type (code_type cpp)**:

```mslb
code_type cpp

line a
    grammar: greet "<name>"
    content
        std::string msg = "Hello, " + name + "!";
        print(msg)
    contentend
lineend
```

- The `<variable_name>` in grammar is mapped to a `std::string` local variable within the function
- `print()` can be used to output content (the package maker automatically converts it to output stream operations)
- Other C++ code is embedded as-is

**C Language Type (code_type c)**:

```mslb
code_type c

line a
    grammar: greet "<name>"
    content
        print(name)
    contentend
lineend
```

- The `<variable_name>` in grammar is mapped to a `const char*` parameter within the function
- `print()` can be used to output content
- Uses C standard library functions (`string.h`, `stdlib.h`, `stdio.h`)
- C source code is compiled with gcc; exported functions are not wrapped with `extern "C"`

**MSLA Language Type (code_type msla)**:

```mslb
code_type msla

line a
    grammar: greet "<name>"
    content
        print(name)
    contentend
lineend
```

- The MSLA language type is currently compiled into a DLL in the style of C++
- The language type is marked as `lang:msla` in `msla_get_info`

**Python Type (code_type py)**:

```mslb
code_type py

line a
    grammar: encode "<text>"
    content
        import base64
        encoded = base64.b64encode(text.encode('utf-8')).decode('utf-8')
        print(encoded)
    contentend
lineend
```

- The `<variable_name>` in grammar is passed in as a Python variable at runtime
- Python scripts are written to a temporary file at runtime and executed via the Python interpreter
- Python must be installed and available in the system PATH

***

## 4. Complete Example

### 4.1 Example MSLB Script

```mslb
bagname mymath

def var text -w

code_type cpp

line a
    grammar: greet "<text>"
    content
        std::string greeting = "Hello, " + text + "!";
        print(greeting)
    contentend
lineend

line b
    grammar: show_info
    content
        print("mymath v1.0")
    contentend
lineend
```

### 4.2 Compiling to a DLL

```
msla_packages_maker.exe mymath.mslb
```

Output:

```
Package: mymath
Code type: cpp
Functions: 2
Generated source: mymath_dll_src.cpp
Successfully built: mymath.dll
```

### 4.3 Using in an MSLA Script

```msla
import mymath .m
.m show_info
.m greet "MSLA"
exit()
```

Run result:

```
mymath v1.0
Hello, MSLA!
```

***

## 5. DLL Interface Specification

The DLL generated by the package maker conforms to the MSLA interpreter's package interface specification:

| Exported Function | Signature                                                          | Description                                                                                                                                                              |
| ----------------- | ------------------------------------------------------------------ | ------------------------------------------------------------------------------------------------------------------------------------------------------------------------ |
| `msla_get_info`   | `const char* msla_get_info()`                                      | Returns package information in the format `lang:<type>\|<function_name> grammar:base64 "<syntax_rule>"`, with each part separated by `\|` |
| `msla_<func_name>`| `const char* msla_<func_name>(const char** args, int argc)`        | Executes the specified function; `args` is the argument array, `argc` is the argument count                                                                              |

If you need to manually write a DLL without using the package maker, you only need to implement the above interface.

***

## 6. Notes

1. **Compiler Dependency**: The package maker requires MinGW to be installed on the system and available in PATH. The C++ type uses g++ for compilation, while the C language type uses gcc.
2. **DLL Location**: The generated DLL file must be in the same directory as the `.msla` script or in a subdirectory for the interpreter to find it.
3. **Python Dependency**: Packages with `code_type py` require the Python interpreter at runtime.
4. **Temporary Files**: The package maker generates temporary C++ source files during the compilation process, which are automatically deleted after a successful build.
5. **Comments**: In MSLB scripts, content from `#` to the end of the line is treated as a comment.
