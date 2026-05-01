# MSLA 包制作器使用说明

**版本**: Beta 0.0.3
**工具名**: MSLA Packages Maker  
**文件后缀**: `.mslb`

---

## 1. 功能简介

MSLA 包制作器是一个命令行工具，用于将 `.mslb` 脚本文件编译为 `.dll` 动态链接库，供 MSLA 解释器通过 `import` 语句加载和调用。包制作器读取 MSLB 脚本中的包定义，自动生成符合 MSLA 解释器 DLL 接口的 C++ 源码，然后调用 g++ 编译器将其编译为 DLL 文件。

---

## 2. 使用方法

### 2.1 基本用法

```
msla_packages_maker.exe <文件路径>.mslb
```

编译成功后，将在 `.mslb` 文件所在目录生成同名的 `.dll` 文件。

### 2.2 指定输出目录

```
msla_packages_maker.exe <文件路径>.mslb -o <输出目录>
```

使用 `-o` 参数可将生成的 DLL 文件输出到指定目录。

### 2.3 示例

```
msla_packages_maker.exe mypkg.mslb
msla_packages_maker.exe mypkg.mslb -o C:\project\libs
```

### 2.4 无参数调用

如果不提供任何参数，包制作器会输出使用说明：

```
MSLA Packages Maker v0.0.3
Usage: packages_maker <file.mslb> [-o <output_dir>]
```

---

## 3. MSLB 脚本语法

### 3.1 基本结构

一个 `.mslb` 脚本由以下部分组成：包名定义、变量定义、代码类型声明、函数行定义。

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

### 3.2 bagname / packagename — 包名定义

定义包的名称，编译后生成的 DLL 文件名即为 `<包名>.dll`。`bagname` 和 `packagename` 两种关键字均可使用，推荐使用 `packagename`。

```mslb
packagename mymath
```

也可使用旧关键字：

```mslb
packagename mymath
```

### 3.3 def var — 包变量定义

定义包中的全局变量，这些变量可被包内的函数使用，也可在 MSLA 脚本中通过 `<简化名>:<变量名>` 引用。

| 参数 | 含义 |
|------|------|
| `-n` | 数字类型变量 |
| `-w` | 字符串类型变量 |

```mslb
def var text -w     # 定义字符串变量
def var count -n    # 定义数字变量
```

### 3.4 code_type — 代码类型声明

声明包中函数使用的脚本语言。

| 类型 | 说明 |
|------|------|
| `cpp` | C++ 代码，直接嵌入 DLL 函数体，使用 g++ 编译 |
| `c` | C 语言代码，直接嵌入 DLL 函数体，使用 gcc 编译 |
| `msla` | MSLA 语言代码，暂以 C++ 方式编译为 DLL |
| `py` | Python 代码，运行时通过 Python 解释器执行 |

```mslb
code_type cpp
```

### 3.5 line / lineend — 函数行定义

定义包中的一个函数。每个 `line` 块包含 `grammar`（语法规则）和 `content`（脚本内容）两部分。

```mslb
line a
    grammar: greet "<name>"
    content
        std::string msg = "Hello, " + name + "!";
        print(msg)
    contentend
lineend
```

### 3.6 grammar — 语法规则

定义用户在 MSLA 脚本中调用该函数时使用的语法格式。

- 语法规则的第一个词为**函数名**，即 MSLA 脚本中调用的名称
- 使用 `<变量名>` 表示参数占位符，调用时传入的值会按顺序映射到对应变量

```mslb
grammar: greet "<name>"           # 函数名为 greet，参数为 name
grammar: add "<a>" "<b>"          # 函数名为 add，参数为 a 和 b
grammar: help                     # 函数名为 help，无参数
```

### 3.7 content / contentend — 脚本内容

定义函数的实际执行脚本，支持 `cpp`、`c`、`msla` 和 `py` 四种类型。

**C++ 类型（code_type cpp）**：

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

- grammar 中的 `<变量名>` 会映射为函数内的 `std::string` 局部变量
- 可使用 `print()` 输出内容（包制作器会自动转换为输出流操作）
- 其余 C++ 代码按原样嵌入

**C 语言类型（code_type c）**：

```mslb
code_type c

line a
    grammar: greet "<name>"
    content
        print(name)
    contentend
lineend
```

- grammar 中的 `<变量名>` 会映射为函数内的 `const char*` 参数
- 可使用 `print()` 输出内容
- 使用 C 标准库函数（`string.h`、`stdlib.h`、`stdio.h`）
- C 语言源码使用 gcc 编译，导出函数无 `extern "C"` 包裹

**MSLA 语言类型（code_type msla）**：

```mslb
code_type msla

line a
    grammar: greet "<name>"
    content
        print(name)
    contentend
lineend
```

- MSLA 语言类型暂以 C++ 方式编译为 DLL
- 在 `msla_get_info` 中通过 `lang:msla` 标记语言类型

**Python 类型（code_type py）**：

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

- grammar 中的 `<变量名>` 会在运行时作为 Python 变量传入
- Python 脚本在运行时写入临时文件并通过 Python 解释器执行
- 需要 Python 已安装并在系统 PATH 中

---

## 4. 完整示例

### 4.1 示例 MSLB 脚本

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

### 4.2 编译为 DLL

```
msla_packages_maker.exe mymath.mslb
```

输出：
```
Package: mymath
Code type: cpp
Functions: 2
Generated source: mymath_dll_src.cpp
Successfully built: mymath.dll
```

### 4.3 在 MSLA 脚本中使用

```msla
import mymath .m
.m show_info
.m greet "MSLA"
exit()
```

运行结果：
```
mymath v1.0
Hello, MSLA!
```

---

## 5. DLL 接口说明

包制作器生成的 DLL 遵循 MSLA 解释器的包接口规范：

| 导出函数 | 签名 | 说明 |
|----------|------|------|
| `msla_get_info` | `const char* msla_get_info()` | 返回包信息，格式为 `lang:<类型>|<函数名> grammar:base64 "<语法规则>"`，各部分以 `|` 分隔 |
| `msla_<函数名>` | `const char* msla_<函数名>(const char** args, int argc)` | 执行指定函数，`args` 为参数数组，`argc` 为参数数量 |

如果需要手动编写 DLL 而不使用包制作器，只需实现上述接口即可。

---

## 6. 注意事项

1. **编译器依赖**：包制作器需要系统中已安装 MinGW 并在 PATH 中可用。C++ 类型使用 g++ 编译，C 语言类型使用 gcc 编译
2. **DLL 位置**：生成的 DLL 文件需与 `.msla` 脚本在同一目录或其子目录中，解释器才能找到
3. **Python 依赖**：`code_type py` 的包运行时需要 Python 解释器
4. **临时文件**：包制作器在编译过程中会生成临时 C++ 源码文件，编译成功后自动删除
5. **注释**：MSLB 脚本中以 `#` 开头到行末的内容为注释
