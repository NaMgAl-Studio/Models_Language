# MSLA 解释器使用说明

**版本**: Beta 0.0.2  
**语言全称**: Models Language  
**文件后缀**: `.msla`

---

## 1. 功能简介

MSLA 解释器是一个可以直接执行 `.msla` 程序文件的命令行工具。它支持 MSLA Beta 0.0.2 的全部语法，包括变量定义与赋值、矩阵操作（含变量索引与越界容忍）、函数类型定义与实例化、模块化执行、条件判断、跳转调用、包系统（import导入DLL包）等。当程序存在问题时，解释器会输出详细的报错信息，包括报错类型、报错位置（文件名、行号、列号）和报错原因。

---

## 2. 使用方法

### 2.1 基本用法

```
msla_interpreter.exe <文件路径>.msla
```

### 2.2 示例

```
msla_interpreter.exe test.msla
```

### 2.3 无参数调用

如果不提供文件路径参数，解释器会输出使用说明：

```
MSLA Interpreter v0.0.2
Usage: msla_interpreter <file.msla>
```

### 2.4 文件不存在

如果指定的 `.msla` 文件不存在，解释器会输出错误信息并退出。

---

## 3. 支持的语法概要

### 3.1 变量定义与赋值

```msla
def var count -n          # 定义数字变量
def var name -w           # 定义字符串变量
ass var count = 10        # 赋值
ass var count = 1 + 2     # 运算赋值
```

### 3.2 变量矩阵

```msla
def var mat_[0:2]_[0:2] -n       # 定义 3x3 数字矩阵
ass var mat = 1                    # 整体赋值
ass var mat_[0]_[2] = 5           # 坐标赋值
ass var mat_[0]_[0:2] = other     # 范围赋值
```

**变量索引规则**：矩阵维度中可使用变量作为索引（如 `mat_[0]_[idx]`，idx 为变量），当变量索引导致部分维度越界时，只要至少有一个有效值则不会报错，越界部分被忽略。

### 3.3 函数类型定义与实例化

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

### 3.4 函数值引用

```msla
player1:a                 # 引用实例函数值
player3_[0]_[0]:a        # 引用函数矩阵值
```

### 3.5 模块

```msla
def mod amod -main        # -main 表示初始化后首先执行
    print("hello")
    jump @ bmod           # 跳转到本文件内的 bmod 模块
modend

def mod bmod
    call @ amod           # 在新进程中调用 amod 模块
modend
```

### 3.6 条件语句

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

### 3.7 print 语句

```msla
print("hello")                    # 打印字符串
print(len("hello"))               # 打印函数结果
print(mat)                        # 打印变量矩阵（二维表格）
print(mat_[0:2]_[0])             # 打印指定范围
print(player3_[0:2]_[0:2]:a)     # 打印函数矩阵值
```

### 3.8 input 语句

```msla
input in ("请输入：") -n    # 限制输入数字
input name ("姓名：") -w   # 限制输入文字
```

### 3.9 其他命令

```msla
run "C:\game\main.exe"     # 运行外部程序
run "\\script\test.py"     # 相对路径运行
exit()                      # 退出程序
```

### 3.10 Python 类型函数兼容

```msla
str(123)       # "123"
int("456")     # 456
float("3.14")  # 3.14
len("hello")   # 5
```

### 3.11 包系统

```msla
import base64 .a           # 导入 base64 包，别名 .a
.a encode "hello"          # 调用 .a 包的 encode 函数，传入参数 "hello"
.a decode "aGVsbG8="      # 调用 .a 包的 decode 函数
```

**包说明**：
- `import <包名> <别名>`：加载指定名称的 DLL 包并注册别名
- 别名格式为 `.x`、`.xx` 或 `.xxx`（1~3个字母）
- 包以 DLL 文件形式提供，需放置在脚本同级或子目录中
- DLL 需导出 `msla_get_info` 函数（返回可用函数列表）及各功能函数

---

## 4. 报错信息说明

解释器在程序出错时会输出格式化的报错信息：

```
[报错类型] 文件名:行号:列号: 报错原因
```

### 4.1 全部报错类型

| 报错类型 | 说明 |
|----------|------|
| UndefinedVariableError | 使用了未定义的变量 |
| UndefinedFunTypeError | 实例化了未定义的函数类型 |
| UninstantiatedFunError | 使用了未实例化的函数 |
| TypeMismatchError | 类型不匹配（如数字变量赋字符串） |
| DimensionMismatchWarning | 矩阵维度不匹配（警告，程序继续） |
| DuplicateDefinitionError | 重复定义同名变量或函数类型 |
| DuplicateInstantiationError | 重复实例化同名实例函数 |
| MissingParameterError | def var 缺少 -n 或 -w 参数 |
| SyntaxFormatError | 语法格式错误 |
| AssignmentDirectionError | 赋值主从关系错误 |
| ModuleNotFoundError | 跳转/调用不存在的模块 |
| FileNotFoundError | run 命令文件未找到 |
| MixedTypeOperationError | 不同类型间的非法运算 |
| MatrixIndexOutOfBoundsError | 矩阵索引越界 |
| MismatchedEndMarkerError | 闭合标记不匹配 |
| MatrixPrintDimensionError | 打印超过两个变化维度的矩阵 |
| MatrixPrintMixedContentError | 打印矩阵时混合其他内容 |
| PackageNotFoundError | 导入的包DLL文件未找到 |
| DuplicateAliasError | import使用了重复的别名 |
| PackageFunctionNotFoundError | 调用的包函数不存在 |

### 4.2 报错示例

```
[UndefinedVariableError] test.msla:5:13: Variable 'unknown' not defined
[TypeMismatchError] test.msla:3:9: Cannot assign string to number variable
[MissingParameterError] test.msla:1:14: Missing parameter -n or -w in def var
[DuplicateDefinitionError] test.msla:2:9: Variable 'x' already defined
```

---

## 5. 示例程序

### 5.1 简单计算

```msla
def var x -n
def var y -n
ass var x = 10
ass var y = 3
print(x + y)
print(x * y)
exit()
```

运行结果：
```
13
30
```

### 5.2 函数与模块

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

运行结果：
```
hi
yes4
```

### 5.3 矩阵操作

```msla
def var mat_[0:2]_[0:2] -n
ass var mat = 5
ass var mat_[1]_[1] = 99
print(mat)
exit()
```

运行结果：
```
5 5 5
5 99 5
5 5 5
```
