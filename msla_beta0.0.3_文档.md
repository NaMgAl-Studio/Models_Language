# MSLA Beta 0.0.2 语法文档

**版本**: Beta 0.0.2
**语言全称**: Models Language
**文件后缀**: `.msla`

***

## 目录

1. [语言概述](#1-语言概述)
2. [基本规则](#2-基本规则)
   - 2.1 [注释](#21-注释)
   - 2.2 [缩进](#22-缩进)
   - 2.3 [关键字](#23-关键字)
   - 2.4 [语句结构](#24-语句结构)
   - 2.5 [初始化代码与模块代码](#25-初始化代码与模块代码)
   - 2.6 [Python 类型处理函数兼容](#26-python-类型处理函数兼容)
3. [变量定义与赋值](#3-变量定义与赋值)
   - 3.1 [变量定义 (def var)](#31-变量定义-def-var)
   - 3.2 [变量赋值 (ass var)](#32-变量赋值-ass-var)
   - 3.3 [变量矩阵](#33-变量矩阵)
   - 3.4 [矩阵间赋值规则](#34-矩阵间赋值规则)
4. [运算符](#4-运算符)
5. [函数类型定义与实例化](#5-函数类型定义与实例化)
   - 5.1 [函数类型定义 (def fun / defend)](#51-函数类型定义-def-fun--defend)
   - 5.2 [函数实例化 (ins)](#52-函数实例化-ins)
   - 5.3 [函数实例赋值 (ass fun / assend)](#53-函数实例赋值-ass-fun--assend)
   - 5.4 [函数矩阵](#54-函数矩阵)
   - 5.5 [函数值作为变量引用](#55-函数值作为变量引用)
6. [控制流](#6-控制流)
   - 6.1 [if / else if / else / ifend 条件语句](#61-if--else-if--else--ifend-条件语句)
   - 6.2 [多条件逻辑（逗号与分号）](#62-多条件逻辑逗号与分号)
   - 6.3 [while / whileend 循环语句](#63-while--whileend-循环语句)
7. [模块](#7-模块)
   - 7.1 [模块定义 (def mod / modend)](#71-模块定义-def-mod--modend)
   - 7.2 [-main 参数](#72--main-参数)
   - 7.3 [jump 跳转指令](#73-jump-跳转指令)
   - 7.4 [call 命令](#74-call-命令)
   - 7.5 [kill 语句](#75-kill-语句)
8. [其他命令](#8-其他命令)
   - 8.1 [run 命令](#81-run-命令)
   - 8.2 [print 语句](#82-print-语句)
   - 8.3 [打印变量矩阵](#83-打印变量矩阵)
   - 8.4 [打印函数矩阵值](#84-打印函数矩阵值)
   - 8.5 [exit 退出语句](#85-exit-退出语句)
   - 8.6 [input 命令](#86-input-命令)
   - 8.7 [wait 语句](#87-wait-语句)
   - 8.8 [check_tip 语句](#88-check_tip-语句)
9. [包系统](#9-包系统)
   - 9.1 [包导入 (import)](#91-包导入-import)
   - 9.2 [包使用](#92-包使用)
   - 9.3 [MSLB 包制作](#93-mslb-包制作)
   - 9.4 [包使用完整示例](#94-包使用完整示例)
10. [完整示例程序](#10-完整示例程序)
11. [报错类型与原因](#11-报错类型与原因)
   - 11.1 [未定义变量错误 (UndefinedVariableError)](#111-未定义变量错误-undefinedvariableerror)
   - 11.2 [未定义函数类型错误 (UndefinedFunTypeError)](#112-未定义函数类型错误-undefinedfuntypeerror)
   - 11.3 [未实例化函数错误 (UninstantiatedFunError)](#113-未实例化函数错误-uninstantiatedfunerror)
   - 11.4 [类型不匹配错误 (TypeMismatchError)](#114-类型不匹配错误-typemismatcherror)
   - 11.5 [矩阵维度不匹配警告 (DimensionMismatchWarning)](#115-矩阵维度不匹配警告-dimensionmismatchwarning)
   - 11.6 [重复定义错误 (DuplicateDefinitionError)](#116-重复定义错误-duplicatedefinitionerror)
   - 11.7 [重复实例化错误 (DuplicateInstantiationError)](#117-重复实例化错误-duplicateinstantiationerror)
   - 11.8 [参数缺失错误 (MissingParameterError)](#118-参数缺失错误-missingparametererror)
   - 11.9 [语法格式错误 (SyntaxFormatError)](#119-语法格式错误-syntaxformaterror)
   - 11.10 [赋值主从关系错误 (AssignmentDirectionError)](#1110-赋值主从关系错误-assignmentdirectionerror)
   - 11.11 [模块未找到错误 (ModuleNotFoundError)](#1111-模块未找到错误-modulenotfounderror)
   - 11.12 [文件未找到错误 (FileNotFoundError)](#1112-文件未找到错误-filenotfounderror)
   - 11.13 [混合类型运算错误 (MixedTypeOperationError)](#1113-混合类型运算错误-mixedtypeoperationerror)
   - 11.14 [矩阵索引越界错误 (MatrixIndexOutOfBoundsError)](#1114-矩阵索引越界错误-matrixindexoutofboundserror)
   - 11.15 [闭合标记不匹配错误 (MismatchedEndMarkerError)](#1115-闭合标记不匹配错误-mismatchedendmarkererror)
   - 11.16 [矩阵打印维度错误 (MatrixPrintDimensionError)](#1116-矩阵打印维度错误-matrixprintdimensionerror)
   - 11.17 [矩阵打印混合内容错误 (MatrixPrintMixedContentError)](#1117-矩阵打印混合内容错误-matrixprintmixedcontenterror)
   - 11.18 [循环外退出错误 (WhileNotInLoopError)](#1118-循环外退出错误-whilenotinlooperror)
   - 11.19 [检查点未找到错误 (CheckpointNotFoundError)](#1119-检查点未找到错误-checkpointnotfounderror)
   - 11.20 [kill 目标未找到错误 (KillTargetNotFoundError)](#1120-kill-目标未找到错误-killtargetnotfounderror)
   - 11.21 [包语法格式不匹配错误 (PackageGrammarFormatMismatchError)](#1121-包语法格式不匹配错误-packagegrammarformatmismatcherror)

***

## 1. 语言概述

MSLA（Models Language）是一门注重模块化的编程语言，具有方便的函数创建机制，函数可以串成函数链。该语言的核心设计理念包括：

- **变量即指针**：所有变量名类似 C 语言中的指针，程序进行变量计算时，会先根据变量名找到对应数据在内存中的地址，再读取对应变量进行计算，而不是将变量复制一次再进行计算。
- **函数类型与实例分离**：先定义函数类型（模板），再将其实例化为可使用的实例函数。一个函数类型可以被多次实例化。
- **模块化执行**：程序通过模块组织代码，支持模块间跳转和跨进程调用。
- **Python 兼容**：字符串处理、print 语句等语法与 Python 兼容。

MSLA 程序文件后缀为 `.msla`，使用 VSCode 在新建文件时输入 `.msla` 后缀即可新建。

***

## 2. 基本规则

### 2.1 注释

以 `#` 开头的内容为注释，从 `#` 到行末的内容会被解释器忽略。

```msla
# 这是一行注释
def var in -n  # 行末注释
```

多行注释用连续三个 `#`（即 `###`）表示注释开始，也用连续三个 `#` 表示注释结束。两个 `###` 之间的所有内容被解释器忽略。

```msla
###
这是多行注释的第一行
这是多行注释的第二行
这是多行注释的第三行
###
def var in -n  ### 这不是多行注释，因为 ### 未单独成行 ###
```

### 2.2 缩进

部分语句（如函数类型定义内部、赋值块内部、模块内部、条件语句内部）需要使用缩进来表示代码层级关系。缩进必须保持一致。

### 2.3 关键字

MSLA 的关键字包括：`def`、`var`、`fun`、`mod`、`ass`、`ins`、`defend`、`assend`、`modend`、`ifend`、`if`、`else`、`while`、`whileend`、`wait`、`check_tip`、`kill`、`run`、`jump`、`call`、`exit`。关键字不可用作变量名、函数名或模块名。

### 2.4 语句结构

每条语句独占一行。不存在一行内写多条语句的语法。

### 2.5 初始化代码与模块代码

- **初始化代码**：模块外的代码，仅在程序刚运行时执行一次。
- **模块代码**：模块内的代码，可通过跳转和调用多次运行。

一个代码文件可以没有模块，但初始化代码始终会在程序启动时首先执行。

### 2.6 Python 类型处理函数兼容

MSLA 支持 Python 中处理字符串、浮点数和整数的内置函数，包括但不限于：

| 函数        | 说明       |
| --------- | -------- |
| `str()`   | 将值转换为字符串 |
| `int()`   | 将值转换为整数  |
| `float()` | 将值转换为浮点数 |
| `len()`   | 获取字符串长度  |

```msla
a = str(123)        # 将数字 123 转换为字符串 "123"
b = int("456")      # 将字符串 "456" 转换为整数 456
c = float("3.14")   # 将字符串 "3.14" 转换为浮点数 3.14
d = len("hello")    # 获取字符串 "hello" 的长度，结果为 5
```

此外，字符串与浮点数等常识性运算规则均遵循 Python 语法，例如字符串与浮点数不能相乘等。

***

## 3. 变量定义与赋值

### 3.1 变量定义 (def var)

**语法**：

```
def var <变量名> <-n | -w>
```

**参数说明**：

| 参数   | 含义                 |
| ---- | ------------------ |
| `-n` | 该变量用于数字的计算（整数和浮点数） |
| `-w` | 该变量用于字符串的处理        |

定义变量时**必须**指定 `-n` 或 `-w` 参数，二者缺一不可。

**示例**：

```msla
def var count -n     # 定义一个名为 count 的数字变量
def var name -w      # 定义一个名为 name 的字符串变量
```

**规则**：

- 所有变量必须先通过 `def var` 定义后才能被使用。
- 变量名不可与关键字重复。
- 不允许重复定义同名变量。

### 3.2 变量赋值 (ass var)

**语法**：

```
ass var <变量名> = <值>
```

**赋值规则**：

- `ass` 语句遵循等号的**主从关系**：必须是让等号的左边等于等号的右边，即将右边的值赋予左边的变量。
- 赋值时必须注意数据类型匹配：数字变量只能赋数字值，字符串变量只能赋字符串值。

**示例**：

```msla
def var in -n
ass var in = 1           # 给 in 赋值为数字 1

def var greeting -w
ass var greeting = "hi"  # 给 greeting 赋值为字符串 "hi"
```

**运算赋值**：

`ass` 语句的等号右侧可以包含运算表达式。

```msla
ass var in = 1 + out     # 将 1 + out 的结果赋给 in
ass var in = a * b       # 将 a * b 的结果赋给 in
```

### 3.3 变量矩阵

变量矩阵是多维的变量集合，矩阵内只能存储同一种数据类型。

**定义语法**：

```
def var <矩阵名>_<范围1>_<范围2>_... <-n | -w>
```

**维度与范围**：

- 范围格式为 `[起始:结束]`，表示该维度从起始索引到结束索引，包含两端。
- 维度大小 = 结束 - 起始 + 1。
- 维度从前往后依次称为第一维、第二维、第三维等。

**示例**：

```msla
# 定义一个 3×3 的数字矩阵（两个维度，每维大小为3）
def var input1_[0:2]_[0:2] -n

# 定义一个一维的数字矩阵（3个元素）
def var input2_[0:2] -n

# 定义一个 3×3×3 的三维数字矩阵
def var input3_[0:2]_[0:2]_[0:2] -n
```

**整体赋值**：

可以用 `ass` 语句一次性给整个矩阵内的所有值赋予统一的值。

```msla
ass var input1 = 1  # 将 input1 矩阵中所有 9 个值都赋为 1
```

**坐标赋值**：

可以给矩阵内某一坐标单独赋值。

```msla
ass var input1_[0]_[2] = 1  # 将第一维索引为0、第二维索引为2的值赋为1
```

**范围赋值**：

可以给矩阵内某一维度上的范围赋值，也可以让一个矩阵中的指定范围与另一个变量矩阵相等。

```msla
ass var input1_[0]_[0:2] = input2  # 将 input1 的第一维索引为0、第二维从0到2的三个值分别等于 input2 中的三个值
```

**维度变量参数**：

在函数矩阵和矩阵变量中，定义维度时可以加入变量作为参数（`def` 语句中除外）。使用变量作为维度参数时，该变量必须为整数形式。

```msla
# 赋值时使用变量作为维度参数
ass var input1_[0:2]_[0:in] = 1   # 第二维的范围为 0 到变量 in 的值（in 必须为整数）
ass var input1_[0]_[in] = 1       # 第二维的索引为变量 in 的值（in 必须为整数）
```

**注意**：`def var` 语句中不能使用变量作为维度参数，维度范围必须是常量。

**变量索引规则**：

在用变量索引函数矩阵和矩阵变量时，仅需索引到至少一个有效值即可，即使有些维度已超出范围。

### 3.4 矩阵间赋值规则

当一个变量矩阵被设定为与另一个矩阵变量相同时，遵循以下规则：

- **维度大小相同**：直接让两者每维的值一一对应相等。
- **维度大小不同**：优先使小维度的值相等。例如：`input1` 为 3×3 二维矩阵（9个值），`input2` 为一维矩阵（3个值），则只有 `input1` 的第一维的三个值按位数从小到大分别等于 `input2` 中的三个值。

```msla
def var input1_[0:2]_[0:2] -n
def var input2_[0:2] -n

ass var input1 = input2  # input1 中 [0][0]=[0], [0][1]=[1], [0][2]=[2]，其余值不受影响
```

此规则同样适用于矩阵间的加减乘除等运算。

***

## 4. 运算符

MSLA 支持以下算术运算符：

| 运算符 | 含义 | 示例          |
| --- | -- | ----------- |
| `+` | 加法 | `a = b + c` |
| `-` | 减法 | `a = b - c` |
| `*` | 乘法 | `a = b * c` |
| `/` | 除法 | `a = b / c` |

MSLA 支持以下比较运算符：

| 运算符 | 含义   | 示例          |
| --- | ---- | ----------- |
| `>`  | 大于   | `a > b`     |
| `<`  | 小于   | `a < b`     |
| `>=` | 大于等于 | `a >= b`    |
| `=<` | 小于等于 | `a =< b`    |
| `!=` | 不等于  | `a != b`    |

**运算规则**：

- 运算符出现在 `ass` 语句等号右侧，必须遵循等号主从关系（让左边等于右边）。
- 数字与数字之间可以进行加减乘除运算。
- 比较运算符用于 `if` 和 `while` 语句的条件判断中，返回布尔值（真或假）。`>`、`<`、`>=`、`=<`、`!=` 均属于比较运算符。
- 当函数类型无参数（即既非 `-n` 也非 `-w`）时，支持混合运算：
  - `a = b + len(str1)` — 数字与字符串函数返回值混合
  - `a = b * c` — 若 `c` 是字符串，`b` 是整数，则 `a` 为整数 `b` 倍的字符串 `c`（与 Python 语法通用）
- 字符串与浮点数之间不能进行非法运算（如相乘），遵循 Python 语法规则。

***

## 5. 函数类型定义与实例化

### 5.1 函数类型定义 (def fun / defend)

**语法**：

```
def fun <函数类型名> <-n | -w | 无参数>
    def <值名1> = <表达式1>
    def <值名2> = <表达式2>
    ...
defend
```

**参数说明**：

| 参数   | 含义                                                |
| ---- | ------------------------------------------------- |
| `-n` | 该函数类型用于数字的计算                                      |
| `-w` | 该函数类型用于字符串的处理（字符串处理语法与 Python 相同，如 `len(str1)` 等） |
| 无参数  | 该函数类型可以混合处理字符串和数字                                 |

**示例**：

```msla
# 定义一个数字类型的函数
def fun player -n
    def a = b + c        # 定义 a 等于 b 加 c
    def d = SUM(e)       # 定义 d 等于 e 中所有值的总和（WPS 表格函数）
defend

# 定义一个无类型的混合函数
def fun mixed
    def a = b + len(str1)   # 数字与字符串函数返回值混合
    def c = d * e           # 若 e 是字符串、d 是整数，则 c 为 d 倍的字符串 e
defend
```

**规则**：

- 函数类型定义以 `def fun` 开头，以 `defend` 结尾，二者必须成对出现。
- 内部的定义式使用 `def <值名> = <表达式>` 格式。
- 定义式中可以使用 WPS 表格中的函数（如 `SUM`、`AVERAGE` 等）以方便计算。使用 WPS 表格函数时，函数名应全部大写。
- 函数类型定义后不能直接使用，必须先实例化。

### 5.2 函数实例化 (ins)

**语法**：

```
ins <函数类型名> <实例函数名>
```

**示例**：

```msla
ins player player1   # 将 player 类型实例化为名为 player1 的实例函数
```

**规则**：

- 一个函数类型可以被多次实例化。
- 实例化后的实例函数名字不能重复。
- 实例函数的名字可以与原来的函数类型名相同。
- 只有实例化后的实例函数才能在代码中进行计算和使用。

### 5.3 函数实例赋值 (ass fun / assend)

**语法**：

```
ass fun <实例函数名>
    <值名1> = <值1>
    <值名2>, <值名3> = <值2>
    <值名4> = <变量名>
    ...
assend
```

**赋值方式**：

- **直接赋值**：将实例函数中的某个值设定为指定常量。
- **多值同时赋值**：一行内可以定义多个值，值与值之间用逗号隔开。
- **链接变量**：实例函数中的值可以链接到某个已定义的变量。链接后，该值随变量的变化而变化。之后可以用 `ass` 语句重新赋予其他值或变量。

**示例**：

```msla
ins player player1

ass fun player1
    c = 1                 # 将 c 赋为 1
    a, b = 1              # 将 a 和 b 都赋为 1
    e = in                # 将 e 链接到变量 in，e 随 in 变化
    z = out               # 将 z 链接到变量 out
assend
```

**规则**：

- 赋值块以 `ass fun` 开头，以 `assend` 结尾，二者必须成对出现。
- 链接的变量必须已存在（即前文已通过 `def var` 定义）。
- 值也可以定义为字符串，如 `a = "hello"`。

### 5.4 函数矩阵

函数矩阵是将函数类型实例化为一个多维矩阵，矩阵中的每个元素都是互相独立的实例函数。

**实例化语法**：

```
ins <函数类型名> <实例函数名>_<范围1>_<范围2>_...
```

**示例**：

```msla
# 将 player 类型实例化为 3×3 的函数矩阵
ins player player3_[0:2]_[0:2]

# 三维函数矩阵
ins player player4_[0:2]_[0:2]_[0:2]
```

**赋值语法**：

```msla
ass fun player3_[0]_[0:2]
    c = 1               # 将 player3 中第一维为0、第二维为0至2的三个实例函数中的 c 都赋为1
    b = input1          # 链接到变量矩阵
    a = 1 - in          # 运算赋值
assend
```

函数矩阵赋值时的范围指定规则与变量矩阵相同。

### 5.5 函数值作为变量引用

实例函数中的指定值可以当作普通变量来使用，使用冒号 `:` 语法引用。

**语法**：

```
<实例函数名>:<值名>
```

对于函数矩阵，需要加上坐标：

```
<实例函数名>_[坐标1]_[坐标2]:<值名>
```

**示例**：

```msla
# 引用单个实例函数的值
ass var x = player1:a        # 将 player1 的 a 值赋给变量 x

# 引用函数矩阵中指定位置的值
ass var y = player3_[0]_[0]:a   # 将 player3[0][0] 的 a 值赋给变量 y
```

**范围引用**：

```msla
# 将 player3 矩阵中指定范围的 a 当做维数大小相同的变量矩阵
ass var result = player3_[0:2]_[0:2]:a
```

***

## 6. 控制流

### 6.1 if / else if / else / ifend 条件语句

**语法**：

```
if(<条件>)
    <语句块>
else if(<条件>)
    <语句块>
else
    <语句块>
ifend
```

**执行逻辑**：

1. 首先检测 `if` 中的条件，若满足则执行 `if` 下的语句块，然后跳到 `ifend` 之后继续执行。
2. 若 `if` 条件不满足，则依次检测 `else if` 中的条件（如果存在），若某个 `else if` 满足则执行其下的语句块，然后跳到 `ifend` 之后。
3. 若所有 `if` 和 `else if` 的条件都不满足，则执行 `else` 下的语句块（如果存在）。
4. `ifend` 为条件语句的闭合标记，必须与 `if` 成对出现。
5. `if` 语句可以**嵌套多层**使用，即在一个 `if` 的语句块内部可以再写 `if` 语句，每层嵌套必须有对应的 `ifend` 闭合。

**示例**：

```msla
if(out > 3)
    print("yes1")
ifend

if(startfun:z > 4)
    print("条件1满足")
else if(out > 5)
    print("条件2满足")
else
    print("所有条件都不满足")
ifend
```

**if 嵌套示例**：

```msla
if(a > 0)
    if(b > 0)
        print("a和b都大于0")
    else
        print("a大于0但b不大于0")
    ifend
else
    print("a不大于0")
ifend
```

### 6.2 多条件逻辑（逗号与分号）

一个 `if` 括号内可以有多个条件，通过逗号和分号连接：

| 连接符 | 含义                      |
| --- | ----------------------- |
| `,` | 逻辑与（AND）—— 逗号分隔的条件需同时满足 |
| `;` | 逻辑或（OR）—— 分号分隔的条件满足其一即可 |

**优先级**：逗号（AND）的优先级高于分号（OR），即先合并逗号连接的条件组，再用分号在各组之间做逻辑或。

**语法**：

```
if(<条件A>, <条件B> ; <条件C>)
```

上述含义为：**（条件A 且 条件B）或 条件C**。

**示例**：

```msla
# startfun:z>4 且 in>4，或者 in>6
if(startfun:z > 4 , in > 4 ; in > 6)
    print("yes2")
ifend
```

### 6.3 while / whileend 循环语句

**语法**：

```
while
    <语句块>
whileend

while (<条件>)
    <语句块>
whileend
```

**无条件循环**：

`while` 后不跟条件时，循环体内的语句将无限循环执行。

```msla
while
    print("a")
whileend
```

**条件循环**：

`while` 后加括号意为仅当满足括号内的条件时才进入循环。括号内可以有多个条件，通过逗号和分号连接，这部分的使用方法与 if 语句中相同。

```msla
while (a > 1)
    print("a")
whileend
```

**循环内使用 if 语句**：

while 语句里可以使用 if 语句。

```msla
while
    if (a > 4)
        ass var a = a - 1
        print("test")
    else
        exit(while)
    ifend
whileend
```

**exit(while) 退出循环**：

`exit(while)` 语句用于退出当前循环，继续执行 whileend 之后的内容。

**规则**：

- while 语句以 `while` 开头，以 `whileend` 结尾，二者必须成对出现。
- while 语句既可以用于模块内，也可以用于模块外。
- while 语句可以嵌套使用。

***

## 7. 模块

### 7.1 模块定义 (def mod / modend)

**语法**：

```
def mod <模块名> [-main]
    <语句块>
modend
```

**规则**：

- 模块以 `def mod` 开头，以 `modend` 结尾，二者必须成对出现。
- 模块内的代码允许多次运行（通过 jump 或 call 指令）。
- 模块外的代码（初始化代码）只在程序刚运行时执行一次。
- 一个代码文件可以没有模块。

**示例**：

```msla
def mod amod
    print("hi")
    ass var in = 1
modend
```

### 7.2 -main 参数

在 `def mod` 语句中可以添加 `-main` 参数：

```
def mod <模块名> -main
```

**含义**：`-main` 参数表示在运行完初始化代码后，首先执行该模块内的代码。

**示例**：

```msla
def mod amod -main
    print("程序开始")
modend
```

一个文件中应只有一个模块使用 `-main` 参数。

### 7.3 jump 跳转指令

**语法**：

```
jump @ <模块名>
```

或跨文件跳转：

```
jump <文件名> <模块名>
```

**说明**：

- `@` 表示在本文件内寻找指定模块进行跳转。
- 若将 `@` 替换为另一个 `.msla` 文件的文件名（不包含 `.msla` 后缀），则表示寻找本文件所处文件夹内及其子文件夹内的对应 `.msla` 文件中的指定模块。
- 跳转到另一个 `.msla` 文件的指定模块时，先执行该文件中的初始化代码，再执行指定模块。

**示例**：

```msla
jump @ bmod          # 跳转到本文件中的 bmod 模块
jump otherfile bmod  # 跳转到 otherfile.msla 中的 bmod 模块
```

**规则**：

- `jump` 始终保持一个进程，跳转后在同一进程内执行目标模块。
- 目标模块必须存在，否则会触发模块未找到错误。

### 7.4 call 命令

**语法**：

```
call <文件名> <模块名>
```

或在本文件内调用：

```
call @ <模块名>
```

**说明**：

- `call` 命令的语法与 `jump` 相同。
- 区别在于：`call` 在后台新建一个进程去执行目标模块，而 `jump` 始终保持一个进程。

**示例**：

```msla
call @ bmod            # 在新进程中执行本文件的 bmod 模块
call sha256 input      # 在新进程中执行 sha256.msla 中的 input 模块
```

**分组语法**：

```
call @ <模块名> ; <组名>
```

**说明**：

- `call @ <模块名> ; <组名>` — 并行执行模块的同时将其分到指定组
- `call @ <模块名>` — 也可不分组
- 分组后可通过 `kill` 语句按组终止进程

**示例**：

```msla
call @ amod ; a    # 并行执行 amod 模块，同时将其分到 a 组
call @ amod        # 并行执行 amod 模块，不分组
```

### 7.5 kill 语句

**语法**：

```
kill                    # 停止除自己这个进程以外的所有其他进程
kill <组名>             # 停止指定组的所有进程
kill <组1>,<组2>,...    # 停止多个组的所有进程
kill N/A                # 停止所有未被分组的进程
```

**说明**：

- `kill` 语句用于停止进程。
- 不带参数的 `kill` 停止除当前进程以外的所有其他进程。
- 带组名的 `kill` 停止指定组的所有进程。
- 多个组名之间用逗号分隔。
- `kill N/A` 停止所有未被分组的进程。

**示例**：

```msla
kill            # 停止除自己以外的所有进程
kill a          # 停止 a 组的所有进程
kill a,b,c      # 停止 a、b、c 组的所有进程
kill N/A        # 停止所有未被分组的进程
```

**规则**：

- `kill` 语句与 `call` 的分组功能配合使用。
- `kill` 只能停止通过 `call` 启动的进程，不能停止当前进程自身。

***

## 8. 其他命令

### 8.1 run 命令

**语法**：

```
run "<文件路径>"
```

**说明**：

- `run` 命令用于运行电脑里的其他可执行文件（如 `.exe`、`.bat`、`.py` 脚本等）。
- 路径以双引号包裹。

**相对路径语法**：

- 若双引号内以 `\\` 开头，则表示在本 `.msla` 脚本所处文件夹内寻找指定文件。

**示例**：

```msla
run "C:\game\main.exe"        # 运行绝对路径的可执行文件
run "\\player\main.exe"       # 在本脚本所处文件夹内寻找 player\main.exe
```

### 8.2 print 语句

**语法**：

```
print(<输出内容>)
```

**说明**：

- `print` 语句的语法与 Python 相同。
- 可以在 `print` 中使用 Python 风格的字符串操作，如 `print(len(str1))`。

**示例**：

```msla
print("hi")
print(len(str1))
print(str1 , " world")
```

### 8.3 打印变量矩阵

`print` 语句可以打印变量矩阵，但需遵守以下规则：

**可打印条件**：

变量矩阵**只有两个维度**，或所限定区域内**只有两个变化的维度**时才能被打印。

| 情况                         | 能否打印  | 说明               |
| -------------------------- | ----- | ---------------- |
| `input1_[0:2]_[0:2]`       | ✅ 可以  | 恰好两个维度           |
| `input2_[0:2]_[0:2]_[0:2]` | ❌ 不可以 | 三个维度，超过两个变化维度    |
| `input2_[0:2]_[0]_[0:2]`   | ✅ 可以  | 第二维固定为0，只有两个变化维度 |
| `input2_[0:2]_[0]`         | ✅ 可以  | 只有一个维度，视为退化情况    |

**打印限制**：

- 同一 `print` 语句中**只能有一个变量矩阵**，不得同时打印其他字符串或数字。

**打印效果**：

- **小维度**从左至右为**横轴**，**较大维度**从上到下为**纵轴**。
- 对应坐标打印对应的内容（字符串或数字）。
- 内容之间用**空格分隔**，间距**自适应**（根据内容长度自动调整）。

**示例**：

```msla
def var input1_[0:2]_[0:2] -n   # 二维变量矩阵
ass var input1 = 1
print(input1)                     # 打印 3x3 变量矩阵

def var input2_[0:2]_[0:2]_[0:2] -n   # 三维变量矩阵
ass var input2 = 1
print(input2_[0:2]_[0]_[0:2])          # 固定第二维，只有两个变化维度，可以打印
```

### 8.4 打印函数矩阵值

函数矩阵**本身不能**被 `print` 打印，但将函数矩阵中指定范围的指定值当做变量矩阵时，**可以**打印，打印规则与打印变量矩阵相同。

```msla
ins player player3_[0:2]_[0:2]   # 3x3 函数矩阵

# 函数矩阵本身不能打印
# print(player3)                # ❌ 错误

# 但将指定范围的指定值当做变量矩阵时可以打印
print(player3_[0:2]_[0:2]:a)    # ✅ 将 player3 中所有 a 值当做 3x3 变量矩阵打印
print(player3_[0]_[0:2]:a)      # ✅ 限定第一维为0，打印第二维范围的 a 值
```

### 8.5 exit 退出语句

**语法**：

```
exit(0)          # 退出所有程序（包括用 call 拉起的进程）
exit(1)          # 仅停止当前进程
exit(while)      # 退出当前 while 循环
exit()           # 等同于 exit(0)，退出所有程序
```

**说明**：

- `exit(0)` — 退出所有程序，包括通过 `call` 命令拉起的所有进程。
- `exit(1)` — 仅停止当前进程，其他进程不受影响。
- `exit(while)` — 退出当前所在的 while 循环，继续执行 whileend 之后的代码。此语句只能在 while 循环内部使用。
- `exit()` — 不带参数时等同于 `exit(0)`。

**示例**：

```msla
exit(0)          # 退出所有程序
exit(1)          # 仅停止当前进程

while (a > 0)
    if (a > 4)
        ass var a = a - 1
    else
        exit(while)    # 退出循环
    ifend
whileend
```

### 8.6 input 命令

**语法**：

```
input <变量名> (<提示文字>) <-n | -w | 无参数>
```

**参数说明**：

| 参数   | 含义                                                                                          |
| ---- | ------------------------------------------------------------------------------------------- |
| `-n` | 限制用户只能输入数字，此时变量必须为数字类型。输入时除数字和小数点外的字符不显示                                                      |
| `-w` | 限制用户只能输入文字，此时变量只能为文字类型。输入时非文字字符不显示                                                            |
| 无参数  | 可以混合输入数字和文字，所有字符正常显示                                                                         |

**说明**：

- `input` 命令用于接收用户输入，并将输入的值赋到指定变量中。
- 执行时会先打印提示文字，再在同一行等待用户的输入，用户用回车确认后将输入内容赋给变量。

**示例**：

```msla
def var in -n
input in ("请输入：") -n       # 打印"请输入："，等待用户输入数字，赋值给 in

def var name -w
input name ("请输入姓名：") -w  # 打印"请输入姓名："，等待用户输入文字，赋值给 name

def var data -n
input data ("请输入数据：")     # 打印"请输入数据："，可混合输入数字和文字
```

### 8.7 wait 语句

**语法**：

```
wait(<秒数>)        # 定时等待
wait(<检查点名>)    # 检查点等待
```

**说明**：

- `wait` 语句用于暂停程序执行。
- 定时等待：`wait(20.5)` 表示等待 20.5 秒后继续执行。参数为数字，支持小数。
- 检查点等待：`wait(now)` 表示程序暂停，等待名为 now 的检查点发出广播后再继续执行。参数为检查点名称（非数字字符串）。
- 检查点等待需与 `check_tip` 语句配合使用。

**示例**：

```msla
wait(20.5)          # 等待 20.5 秒
wait(now)           # 等待名为 now 的检查点广播
```

### 8.8 check_tip 语句

**语法**：

```
check_tip <检查点名>
```

**说明**：

- `check_tip` 语句用于放置一个检查点，当程序执行到该行时，进行名为指定名称的广播。
- 所有等待该检查点的 `wait` 语句将被唤醒，继续执行后续代码。
- 检查点名称由用户自定义。

**示例**：

```msla
check_tip now       # 放置名为 now 的检查点，执行到该行时进行 now 广播
```

**配合使用示例（wait 与 check_tip）**：

```msla
# 进程 A
wait(now)           # 等待 now 检查点广播

# 进程 B
check_tip now       # 执行到此时发出 now 广播，唤醒进程 A
```

***

## 9. 包系统

MSLA 支持通过包（Package）机制扩展功能。包是一系列 `.dll` 文件，可被解释器读取或编译好的程序所读取。

**基本规则**：

- `.dll` 包必须和 `.msla` 脚本或编译好的 `.exe` 文件在一个工程文件夹内。

### 9.1 包导入 (import)

**语法**：

```
import <包名> <简化名>
```

**简化名规则**：

- 简化名由一个英文句号和一到三个英文字符构成，如 `.m`、`.ab`、`.abc`。
- 每种包可以被多次导入并被指定不同的简化名，此时包与包之间互相独立（因为有些包在导入时会顺带创建一些变量）。
- 一整个工程文件夹内，每次导入包时的简化名不能相同。

**示例**：

```msla
import printpro .m    # 导入 printpro 包，简化名为 .m
import base64 .a      # 导入 base64 包，简化名为 .a
import base64 .b      # 同一个 base64 包再次导入，简化名为 .b，与 .a 互相独立
```

### 9.2 包使用

**语法**：

```
<简化名> <包支持的语句>
```

**说明**：

- 要执行包里面的语句，先打出指定包的简化名，再打出包所支持的语句。

**示例**：

```msla
import printpro .m
.m printpro("aaa")    # 使用 .m 简化名调用 printpro 包的 printpro 语句
```

**变量赋值与引用**：

当 grammar 中定义了变量参数时（如 `grammar: base64 "<intext>"`），在 msla 脚本中可以通过以下方式为变量赋值：

1. **直接赋值**：将常量值赋给 grammar 中定义的变量。双引号是 grammar 语句中规定的语法格式，不是实际输入内容的一部分。

```msla
import base64 .a
.a:base64 "test"             # 将 test 字符串赋给 grammar 中定义的 intext 变量
```

2. **变量引用**：用 `<变量名>` 的形式将 msla 脚本中的变量值赋给 grammar 中定义的变量。

```msla
.a:base64 "<in>"             # 将 in 变量的值赋给 intext 变量
```

3. **格式错误**：如果使用时的格式与 grammar 语句中定义的格式不匹配，将返回 SyntaxFormatError 错误。

```msla
.a:base64 <in>               # ❌ 错误：格式与 grammar 中定义的 base64 "<intext>" 不匹配
```

### 9.3 MSLB 包制作

用户除了可以直接用其他语言制作包并编译为 `.dll` 文件外，也可以编写 `.mslb` 脚本并用包制作器制作包。

**MSLB 脚本语法**：

```
bagname <包名>

def var <变量名> <-n | -w>     # 定义包中所带的变量，可被全局使用或引用

code_type <语言标识>            # 定义脚本中使用的编程语言，py 表示 Python，cpp 表示 C++，c 表示 C 语言，msla 表示 MSLA 语言

line <语句名>                   # 开始定义包中的一个语法
    grammar: <语法规则>         # 定义语法的使用格式
    content                     # 开始定义语法实际对应的脚本
        <脚本内容>
    contentend                  # 定义脚本结束
lineend                         # 定义语法结束
```

**语法说明**：

| 关键字                      | 含义                                                              |
| ------------------------ | --------------------------------------------------------------- |
| `bagname`                | 定义包名                                                            |
| `def var`                | 定义包中的变量，这些变量可被全局使用或引用，也可由包内的语句所使用。用户在 msla 脚本中仅可以访问通过此方式创建的包内变量 |
| `code_type`              | 指定脚本使用的编程语言，`py` 表示 Python，`cpp` 表示 C++，`c` 表示 C 语言，`msla` 表示 MSLA 语言 |
| `line` / `lineend`       | 定义包中的一个语法，需成对出现                                                 |
| `grammar:`               | 定义语法的使用格式，`<变量名>` 表示在该句中顺便定义了变量的值                               |
| `content` / `contentend` | 定义语法实际对应的脚本内容，需成对出现                                             |

**完整示例**：

```mslb
bagname base64

def var intext -w

code_type py

line a
    grammar: base64 "<intext>"
    content
        import base64
        original_text = intext
        bytes_data = original_text.encode('utf-8')
        base64_bytes = base64.b64encode(bytes_data)
        encoded_text = base64_bytes.decode('utf-8')
        print(encoded_text)
    contentend
lineend

line b
    grammar: base64_help
    content
        print("base64 <text>")
    contentend
lineend
```

**说明**：

- 用户编写好 `.mslb` 脚本后，需使用包制作器将其制作为 `.dll` 文件。
- `.mslb` 脚本涉及其他编程语言，因此要求用户必须拥有对应语言的编译器。

**格式校验**：

当用户在 msla 脚本中使用包语句时，如果使用格式与 grammar 语句中定义的格式不匹配，解释器将返回 SyntaxFormatError 错误。

例如，grammar 定义为 `base64 "<intext>"`，但用户写成了 `.a:base64 <in>`（缺少双引号），则触发 SyntaxFormatError。

### 9.4 包使用完整示例

以下示例展示了从导入包到调用包语句的完整流程：

```msla
import base64 .a
.a base64_help
.a base64 "hello"
exit()
```

**运行输出**：

```
base64 <text>
aGVsbG8=
```

其中 `aGVsbG8=` 是 `hello` 进行 Base64 编码后的结果。

***

## 10. 完整示例程序

以下是一段简单的完整 MSLA 程序，展示了变量定义、函数定义与实例化、模块定义、条件判断、跳转等核心语法：

```msla
# 定义 main 函数类型
def fun main -n
    def z = a + b + c
defend

# 定义变量
def var in -n
def var out -n

# 实例化函数
ins main startfun

# 为实例函数赋值
ass fun startfun
    a, b = 1
    c = in
    z = out
assend

# 定义主模块，-main 表示初始化代码后首先执行此模块
def mod amod -main
    print("hi")

    ass var in = 1

    if(out > 3)
        print("yes1")
    ifend

    jump @ bmod

    call sha256 input
modend

# 定义另一个模块
def mod bmod
    print("hi")

    if(startfun:z > 4 , in > 4 ; in > 6)
        print("yes2")
    else if(out > 5)
        print("yes3")
    else
        print("yes4")
    ifend

    exit()
modend
```

***

## 11. 报错类型与原因

### 11.1 未定义变量错误 (UndefinedVariableError)

**触发场景**：使用了一个未通过 `def var` 定义的变量名。

**示例**：

```msla
ass var x = unknown_var   # unknown_var 未被定义
```

**原因**：所有变量必须先通过 `def var` 定义后才能被使用。请在使用前添加 `def var <变量名> <-n | -w>` 语句。

***

### 11.2 未定义函数类型错误 (UndefinedFunTypeError)

**触发场景**：实例化了一个未通过 `def fun` 定义的函数类型。

**示例**：

```msla
ins unknown_fun f1   # unknown_fun 未被定义
```

**原因**：函数类型必须先通过 `def fun` 定义后才能被实例化。请先定义对应的函数类型。

***

### 11.3 未实例化函数错误 (UninstantiatedFunError)

**触发场景**：使用了一个未通过 `ins` 实例化的函数。

**示例**：

```msla
ass fun player1      # player1 未被实例化
    a = 1
assend
```

**原因**：函数类型定义后不能直接使用，必须先通过 `ins` 实例化为实例函数。

***

### 11.4 类型不匹配错误 (TypeMismatchError)

**触发场景**：对数字变量赋字符串值，或对字符串变量赋数字值。

**示例**：

```msla
def var count -n
ass var count = "hello"   # 数字变量不能赋字符串值

def var name -w
ass var name = 123        # 字符串变量不能赋数字值
```

**原因**：变量的数据类型由 `def var` 中的 `-n` 或 `-w` 参数决定，赋值时必须保持类型一致。

***

### 11.5 矩阵维度不匹配警告 (DimensionMismatchWarning)

**触发场景**：矩阵间赋值时维度大小不一致。

**示例**：

```msla
def var input1_[0:2]_[0:2] -n
def var input2_[0:2] -n
ass var input1 = input2   # input1 为二维矩阵，input2 为一维矩阵
```

**原因**：当两个矩阵维度不同时，系统会优先使小维度的值相等，未被覆盖的值保持不变。此为警告，程序仍可执行，但请确认是否符合预期。

***

### 11.6 重复定义错误 (DuplicateDefinitionError)

**触发场景**：重复定义同名变量或同名函数类型。

**示例**：

```msla
def var count -n
def var count -n      # 重复定义同名变量

def fun player -n
    def a = b + c
defend
def fun player -n     # 重复定义同名函数类型
    def a = b * c
defend
```

**原因**：变量名和函数类型名在同一个作用域内必须唯一，不允许重复定义。

***

### 11.7 重复实例化错误 (DuplicateInstantiationError)

**触发场景**：重复实例化同名实例函数。

**示例**：

```msla
ins player player1
ins player player1    # 重复实例化同名实例函数
```

**原因**：实例函数的名字必须唯一，不允许重复实例化相同名称的实例函数（即使来自不同的函数类型）。

***

### 11.8 参数缺失错误 (MissingParameterError)

**触发场景**：`def var` 语句缺少 `-n` 或 `-w` 参数。

**示例**：

```msla
def var count         # 缺少 -n 或 -w 参数
```

**原因**：定义变量时必须指定 `-n`（数字类型）或 `-w`（字符串类型）参数。

***

### 11.9 语法格式错误 (SyntaxFormatError)

**触发场景**：关键字拼写错误、缺少闭合标记等。

**示例**：

```msla
# 关键字拼写错误
df var count -n              # "df" 应为 "def"

# 缺少 defend 闭合标记
def fun player -n
    def a = b + c            # 缺少 defend

# 缺少 assend 闭合标记
ass fun player1
    c = 1                    # 缺少 assend

# 缺少 modend 闭合标记
def mod amod
    print("hi")              # 缺少 modend

# 缺少 ifend 闭合标记
if(out > 3)
    print("yes")             # 缺少 ifend
```

**原因**：MSLA 的每种块结构都有对应的闭合标记，必须成对使用：`def fun`/`defend`、`ass fun`/`assend`、`def mod`/`modend`、`if`/`ifend`。关键字拼写必须准确。

***

### 11.10 赋值主从关系错误 (AssignmentDirectionError)

**触发场景**：`ass` 语句中违反等号主从关系，即试图让等号右边等于左边。

**说明**：MSLA 的 `ass` 语句严格遵循**左边被赋值为右边的值**的主从关系。等号左侧必须是一个可赋值的目标（变量名或矩阵坐标），等号右侧是表达式或值。

**示例**：

```msla
# 错误：试图将 in 赋给 1，而非将 1 赋给 in
ass var 1 = in
```

**原因**：`ass` 语句中，等号左侧必须是赋值目标，右侧是值或表达式。主从关系不可颠倒。

***

### 11.11 模块未找到错误 (ModuleNotFoundError)

**触发场景**：`jump` 或 `call` 指令指向一个不存在的模块或文件。

**示例**：

```msla
jump @ nonexistent_mod      # 本文件中不存在 nonexistent_mod 模块
jump missingfile bmod       # 找不到 missingfile.msla 文件
```

**原因**：目标模块名或文件名不存在。请检查模块名是否拼写正确，或目标 `.msla` 文件是否存在于当前文件夹及其子文件夹中。

***

### 11.12 文件未找到错误 (FileNotFoundError)

**触发场景**：`run` 命令指向一个不存在的可执行文件。

**示例**：

```msla
run "C:\nonexistent\program.exe"   # 文件路径不存在
run "\\missing\script.py"          # 脚本所处文件夹内不存在该文件
```

**原因**：`run` 命令中指定的文件路径不存在。请检查文件路径是否正确，以及使用 `\\` 相对路径时文件是否在本脚本所处文件夹内。

***

### 11.13 混合类型运算错误 (MixedTypeOperationError)

**触发场景**：字符串与浮点数等不同类型之间进行非法运算。

**示例**：

```msla
def var num -n
def var str -w
ass var num = 3.14
ass var str = "hello"
ass var result = num + str   # 数字与字符串不能直接相加
```

**原因**：数字与字符串之间不能进行加减乘除等算术运算。对于字符串的重复（如整数 \* 字符串），仅在没有指定 `-n` 或 `-w` 参数的函数类型中支持，遵循 Python 语法规则。

***

### 11.14 矩阵索引越界错误 (MatrixIndexOutOfBoundsError)

**触发场景**：访问矩阵时索引超出定义范围。

**示例**：

```msla
def var input_[0:2] -n         # 矩阵索引范围为 0~2
ass var input_[3] = 1          # 索引 3 超出范围
ass var input_[-1] = 1         # 索引 -1 超出范围
```

**原因**：矩阵的索引范围由 `def var` 中的 `[起始:结束]` 决定，访问时索引必须在起始到结束的范围内（包含两端）。

***

### 11.15 闭合标记不匹配错误 (MismatchedEndMarkerError)

**触发场景**：闭合标记与开头标记不匹配。

**示例**：

```msla
def fun player -n
    def a = b + c
assend                        # 应为 defend，而非 assend

ass fun player1
    c = 1
defend                        # 应为 assend，而非 defend
```

**原因**：每种块结构都有对应的闭合标记：`def fun` 对应 `defend`、`ass fun` 对应 `assend`、`def mod` 对应 `modend`、`if` 对应 `ifend`。必须使用正确的闭合标记。

***

### 11.16 矩阵打印维度错误 (MatrixPrintDimensionError)

**触发场景**：尝试用 `print` 打印超过两个变化维度的变量矩阵或函数矩阵值。

**示例**：

```msla
def var input2_[0:2]_[0:2]_[0:2] -n   # 三维变量矩阵
ass var input2 = 1
print(input2)                          # ❌ 三个变化维度，不能打印

ins player player3_[0:2]_[0:2]_[0:2]   # 三维函数矩阵
print(player3_[0:2]_[0:2]_[0:2]:a)     # ❌ 三个变化维度，不能打印
```

**原因**：`print` 打印变量矩阵时，矩阵必须恰好有两个维度，或所限定区域内只有两个变化的维度。超过两个变化维度的矩阵无法以二维表格形式展示。

***

### 11.17 矩阵打印混合内容错误 (MatrixPrintMixedContentError)

**触发场景**：在打印变量矩阵的 `print` 语句中同时包含其他字符串或数字。

**示例**：

```msla
def var input1_[0:2]_[0:2] -n
ass var input1 = 1
print(input1, "结果如下")    # ❌ 打印变量矩阵时不能同时包含其他内容
print("值:", input1)         # ❌ 打印变量矩阵时不能同时包含其他内容
```

**原因**：`print` 语句中包含变量矩阵时，该语句中**只能有一个变量矩阵**，不得同时打印其他字符串或数字。变量矩阵的打印会以二维表格形式输出，无法与其他内容混合排版。

***

### 11.18 循环外退出错误 (WhileNotInLoopError)

**触发场景**：在 while 循环外部使用 `exit(while)` 语句。

**示例**：

```msla
if(a > 0)
    exit(while)    # ❌ 不在 while 循环内，不能使用 exit(while)
ifend
```

**原因**：`exit(while)` 只能在 while 循环内部使用，用于退出当前循环。在循环外部使用此语句将触发错误。

***

### 11.19 检查点未找到错误 (CheckpointNotFoundError)

**触发场景**：`wait` 语句中指定的检查点名称不存在（没有对应的 `check_tip` 语句）。

**示例**：

```msla
wait(unknown_tip)    # 名为 unknown_tip 的检查点不存在
```

**原因**：`wait` 语句等待的检查点必须通过 `check_tip` 语句定义。请检查检查点名称是否拼写正确，以及对应的 `check_tip` 语句是否已存在。

***

### 11.20 kill 目标未找到错误 (KillTargetNotFoundError)

**触发场景**：`kill` 语句中指定的组名不存在（没有通过 `call` 语句创建过该组）。

**示例**：

```msla
kill nonexistent_group    # 名为 nonexistent_group 的组不存在
```

**原因**：`kill` 语句中指定的组名必须是通过 `call @ <模块名> ; <组名>` 语句创建的组。请检查组名是否拼写正确，以及对应的 `call` 分组语句是否已执行。

***

### 11.21 包语法格式不匹配错误 (PackageGrammarFormatMismatchError)

**触发场景**：在 msla 脚本中使用包语句时，使用格式与 grammar 语句中定义的格式不匹配。

**示例**：

```msla
import base64 .a
# grammar 定义为 base64 "<intext>"
.a:base64 <in>    # ❌ 缺少双引号，与 grammar 定义的格式不匹配
```

**原因**：包语句的使用格式必须与 `.mslb` 脚本中 `grammar:` 定义的格式严格一致。双引号、尖括号等格式标记不可省略或更改。此错误属于 SyntaxFormatError 在包使用场景下的具体表现。

***

*本文档基于 MSLA Beta 0.0.2 语法规范编写，如遇本文档未涵盖的情况，默认遵循 Python 语法规则。*
