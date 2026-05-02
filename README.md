# MSLA — Models Language

> 面向模块化思维、原生支持矩阵与模块化、具备极强扩展性的命令式编程语言

> A model-oriented, matrix-native, modular, and highly extensible imperative programming language.

---

[📖 中文语法文档](msla_beta0.0.3_文档.md) | [📖 English Syntax Doc](msla_beta0.0.3_doc_en.md)

---

## 语言定位 / Language Overview

MSLA（Models Language）是一门以**"模块化思维"**为核心设计理念的编程语言。它将现实世界中的对象抽象为**变量矩阵**与**函数矩阵**，将程序的执行流程组织为**模块**，让开发者能够像搭积木一样构建程序。

MSLA (Models Language) is a programming language with **"model-oriented thinking"** at its core. It abstracts real-world objects into **variable matrices** and **function matrices**, organizes execution flow into **modules**, and allows developers to build programs like building blocks.

---

## 五大核心特色 / Five Core Features

### 1. 面向用户，变量函数操作友好简单 / User-Friendly Variables & Functions

MSLA 的语法设计遵循"所见即所得"原则。变量必须先定义后使用，类型明确（`-n` 数字 / `-w` 字符串），赋值语句遵循清晰的主从关系。

MSLA follows a "what you see is what you get" design principle. Variables must be defined before use, with explicit types (`-n` for numbers / `-w` for strings). Assignment follows a clear master-slave relationship.

```msla
def var name -w           # Define a string variable
ass var name = "MSLA"     # Assign a value
print("Hello, " , name)   # Output
```

### 2. 原生矩阵支持 / Native Matrix Support

MSLA 原生支持**多维矩阵**——不仅是数据，连函数也能矩阵化。3×3 的函数矩阵中，每个单元格都是独立运行的函数实例。

MSLA natively supports **multi-dimensional matrices** — not just for data, but functions too. In a 3×3 function matrix, each cell is an independently running function instance.

```msla
# 3x3 number matrix
def var grid_[0:2]_[0:2] -n
ass var grid = 0
ass var grid_[1]_[1] = 99
print(grid)

# 3x3 function matrix
def fun node -n
    def output = input * weight
    defend

ins node network_[0:2]_[0:2]   # Instantiate as 3x3 function matrix
```

**Output / 输出**:
```
0 0 0
0 99 0
0 0 0
```

### 3. 模块化编程 / Modular Programming

代码以**模块（mod）**组织。通过 `jump`（同进程跳转）和 `call`（新进程调用）实现模块协作，完美映射思维中的"步骤"与"分支"。

Code is organized in **modules (mod)**. Use `jump` (same-process) and `call` (new-process) for inter-module collaboration, naturally mapping to the "steps" and "branches" of human thinking.

```msla
def mod setup -main
    print("System started")
    jump @ process
modend

def mod process
    print("Processing...")
    if(count > 10)
        jump @ finish
    ifend
modend

def mod finish
    print("Done!")
    exit()
modend
```

### 4. 极强扩展性 / Extremely Extensible

完善的**包系统（Package System）**：用 C++/Python/C/MSLA 编写功能，通过 `.mslb` 脚本定义接口，包制作器一键编译为 `.dll`，一行 `import` 即可调用。

Powerful **Package System**: Write features in C++/Python/C/MSLA, define interfaces via `.mslb` scripts, compile to `.dll` with one click, and use with a single `import`.

```mslb
# mymath.mslb — Package definition
bagname mymath

def var result -n

code_type cpp

line add
    grammar: add "<a>" "<b>"
    content
        int sum = std::stoi(a) + std::stoi(b);
        print(std::to_string(sum));
    contentend
lineend
```

```msla
# main.msla — Using the package
import mymath .m
.m add "10" "20"    # Output: 30
```

### 5. 原生多线程 / Native Multithreading

通过 `call` 命令在后台新建进程执行模块。一行代码开启多线程。

Create parallel processes with `call`. One line for multithreading.

```msla
call @ heavy_computation   # Run in background, main continues
```

---

## 核心语法 / Core Syntax Highlights

| Feature / 特性 | Description / 说明 | Example / 示例 |
|:---|:---|:---|
| **Variable Definition / 变量定义** | Explicit types with `-n`/`-w` | `def var age -n` |
| **Matrix Operation / 矩阵操作** | Multi-dim, full/coord/range assignment | `ass var mat_[0:2]_[0:2] = 1` |
| **Function Type / 函数类型** | Define type first, then instantiate | `def fun calc -n` → `ins calc c1` |
| **Function Matrix / 函数矩阵** | Functions as matrices | `ins calc grid_[0:2]_[0:2]` |
| **Module System / 模块系统** | Jump and call across modules | `def mod main -main` |
| **Condition / 条件判断** | Multi-condition AND/OR | `if(a > 0, b > 0 ; c > 0)` |
| **I/O / 输入输出** | Python-like syntax, matrix printing | `input in ("Prompt:") -n` |
| **Package System / 包系统** | Import DLL extensions with aliases | `import base64 .a` |

---

## 包系统工作流 / Package System Workflow

1. **编写 `.mslb` 脚本** — 定义包名、变量、语法规则和执行代码（支持 C++/Python/C/MSLA）
2. **编译** — `msla_packages_maker.exe mypkg.mslb` → 生成 `mypkg.dll`
3. **导入使用** — `import mypkg .m`

> 1. **Write `.mslb` script** — define package name, variables, grammar rules, and code
> 2. **Compile** — `msla_packages_maker.exe mypkg.mslb` → produces `mypkg.dll`
> 3. **Import & use** — `import mypkg .m`

---

## 工具链 / Toolchain

| Tool / 工具 | Function / 功能 | Command / 命令 |
|:---|:---|:---|
| **MSLA Interpreter / 解释器** | Execute `.msla` scripts | `msla_interpreter.exe main.msla` |
| **MSLA Compiler / 编译器** | Compile `.msla` to `.exe` | *In development / 待制作* |
| **Packages Maker / 包制作器** | Compile `.mslb` to `.dll` | `msla_packages_maker.exe mypkg.mslb` |

---

## 设计理念 / Design Philosophy

MSLA 的设计始终围绕一个核心：**让程序结构贴近人的思维结构**。

MSLA revolves around one core idea: **make program structure match human thought structure**.

| 人类思维 / Human Thought | MSLA 设计 / MSLA Design |
|:---|:---|
| 网格化数据 / Grid Data | **矩阵 / Matrix** |
| 步骤与阶段 / Steps & Stages | **模块 / Module** |
| 可复用组件 / Reusable Components | **包 / Package** |
| 同时多任务 / Simultaneous Tasks | **多线程 / Multithreading** |

> 你不需要花费精力去适应计算机的思维方式，而是让计算机理解你的思维方式。
>
> You don't adapt to the computer's way of thinking — the computer adapts to yours.

---

## 快速入门 / Quick Start

### 示例 1 / Example 1：矩阵运算 / Matrix Operation

```msla
def var a_[0:2]_[0:2] -n
ass var a = 1
ass var a_[0]_[1] = 5
ass var a_[1]_[1] = 9
print(a)
exit()
```

**Output / 输出**:
```
1 5 1
1 9 1
1 1 1
```

### 示例 2 / Example 2：函数与模块 / Functions & Modules

```msla
def fun main -n
    def z = a + b + c
    defend

def var in -n
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
    if(startfun:z > 4, in > 4 ; in > 6)
        print("yes2")
    else
        print("yes4")
    ifend
    exit()
modend
```

**Output / 输出**:
```
hi
yes4
```

### 示例 3 / Example 3：使用扩展包 / Using Packages

```msla
import base64 .a
.a base64_help
.a base64 "hello"
exit()
```

**Output / 输出**:
```
base64 <text>
aGVsbG8=
```

---

> **开始你的 MSLA 之旅 / Start Your MSLA Journey**：创建 `.msla` 文件，运行 `msla_interpreter.exe`，体验模块化编程的魅力。
>
> Create a `.msla` file, run `msla_interpreter.exe`, and experience the power of modular programming.

---

📖 [完整中文语法文档 / Full Chinese Syntax Documentation](msla_beta0.0.3_文档.md)
📖 [Full English Syntax Documentation](msla_beta0.0.3_doc_en.md)
