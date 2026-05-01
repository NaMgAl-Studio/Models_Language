# MSLA Beta 0.0.2 Syntax Documentation

**Version**: Beta 0.0.2
**Language Full Name**: Models Language
**File Extension**: `.msla`

***

## Table of Contents

1. [Language Overview](#1-language-overview)
2. [Basic Rules](#2-basic-rules)
   - 2.1 [Comments](#21-comments)
   - 2.2 [Indentation](#22-indentation)
   - 2.3 [Keywords](#23-keywords)
   - 2.4 [Statement Structure](#24-statement-structure)
   - 2.5 [Initialization Code and Module Code](#25-initialization-code-and-module-code)
   - 2.6 [Python Type Handling Function Compatibility](#26-python-type-handling-function-compatibility)
3. [Variable Definition and Assignment](#3-variable-definition-and-assignment)
   - 3.1 [Variable Definition (def var)](#31-variable-definition-def-var)
   - 3.2 [Variable Assignment (ass var)](#32-variable-assignment-ass-var)
   - 3.3 [Variable Matrices](#33-variable-matrices)
   - 3.4 [Inter-Matrix Assignment Rules](#34-inter-matrix-assignment-rules)
4. [Operators](#4-operators)
5. [Function Type Definition and Instantiation](#5-function-type-definition-and-instantiation)
   - 5.1 [Function Type Definition (def fun / defend)](#51-function-type-definition-def-fun--defend)
   - 5.2 [Function Instantiation (ins)](#52-function-instantiation-ins)
   - 5.3 [Function Instance Assignment (ass fun / assend)](#53-function-instance-assignment-ass-fun--assend)
   - 5.4 [Function Matrices](#54-function-matrices)
   - 5.5 [Function Values as Variable References](#55-function-values-as-variable-references)
6. [Control Flow](#6-control-flow)
   - 6.1 [if / else if / else / ifend Conditional Statements](#61-if--else-if--else--ifend-conditional-statements)
   - 6.2 [Multi-Condition Logic (Comma and Semicolon)](#62-multi-condition-logic-comma-and-semicolon)
   - 6.3 [while / whileend Loop Statements](#63-while--whileend-loop-statements)
7. [Modules](#7-modules)
   - 7.1 [Module Definition (def mod / modend)](#71-module-definition-def-mod--modend)
   - 7.2 [-main Parameter](#72--main-parameter)
   - 7.3 [jump Instruction](#73-jump-instruction)
   - 7.4 [call Command](#74-call-command)
   - 7.5 [kill Statement](#75-kill-statement)
8. [Other Commands](#8-other-commands)
   - 8.1 [run Command](#81-run-command)
   - 8.2 [print Statement](#82-print-statement)
   - 8.3 [Printing Variable Matrices](#83-printing-variable-matrices)
   - 8.4 [Printing Function Matrix Values](#84-printing-function-matrix-values)
   - 8.5 [exit Statement](#85-exit-statement)
   - 8.6 [input Command](#86-input-command)
   - 8.7 [wait Statement](#87-wait-statement)
   - 8.8 [check_tip Statement](#88-check_tip-statement)
9. [Package System](#9-package-system)
   - 9.1 [Package Import (import)](#91-package-import-import)
   - 9.2 [Package Usage](#92-package-usage)
   - 9.3 [MSLB Package Creation](#93-mslb-package-creation)
   - 9.4 [Complete Package Usage Example](#94-complete-package-usage-example)
10. [Complete Example Program](#10-complete-example-program)
11. [Error Types and Causes](#11-error-types-and-causes)
    - 11.1 [UndefinedVariableError](#111-undefinedvariableerror)
    - 11.2 [UndefinedFunTypeError](#112-undefinedfuntypeerror)
    - 11.3 [UninstantiatedFunError](#113-uninstantiatedfunerror)
    - 11.4 [TypeMismatchError](#114-typemismatcherror)
    - 11.5 [DimensionMismatchWarning](#115-dimensionmismatchwarning)
    - 11.6 [DuplicateDefinitionError](#116-duplicatedefinitionerror)
    - 11.7 [DuplicateInstantiationError](#117-duplicateinstantiationerror)
    - 11.8 [MissingParameterError](#118-missingparametererror)
    - 11.9 [SyntaxFormatError](#119-syntaxformaterror)
    - 11.10 [AssignmentDirectionError](#1110-assignmentdirectionerror)
    - 11.11 [ModuleNotFoundError](#1111-modulenotfounderror)
    - 11.12 [FileNotFoundError](#1112-filenotfounderror)
    - 11.13 [MixedTypeOperationError](#1113-mixedtypeoperationerror)
    - 11.14 [MatrixIndexOutOfBoundsError](#1114-matrixindexoutofboundserror)
    - 11.15 [MismatchedEndMarkerError](#1115-mismatchedendmarkererror)
    - 11.16 [MatrixPrintDimensionError](#1116-matrixprintdimensionerror)
    - 11.17 [MatrixPrintMixedContentError](#1117-matrixprintmixedcontenterror)
    - 11.18 [WhileNotInLoopError](#1118-whilenotinlooperror)
    - 11.19 [CheckpointNotFoundError](#1119-checkpointnotfounderror)
    - 11.20 [KillTargetNotFoundError](#1120-killtargetnotfounderror)
    - 11.21 [PackageGrammarFormatMismatchError](#1121-packagegrammarformatmismatcherror)

***

## 1. Language Overview

MSLA (Models Language) is a modularity-focused programming language featuring a convenient function creation mechanism that allows functions to be chained together. The core design principles include:

- **Variables as Pointers**: All variable names function similarly to pointers in C. When performing calculations on variables, the program first locates the corresponding data address in memory based on the variable name, then reads and computes the corresponding variable, rather than copying the variable for computation.
- **Separation of Function Types and Instances**: First define a function type (template), then instantiate it into a usable instance function. A function type can be instantiated multiple times.
- **Modular Execution**: Programs organize code through modules, supporting inter-module jumps and cross-process calls.
- **Python Compatibility**: String processing, print statement syntax, etc., are compatible with Python.

MSLA program files use the `.msla` extension. Create a new file in VSCode with the `.msla` extension to start.

***

## 2. Basic Rules

### 2.1 Comments

Content starting with `#` is a comment. Everything from `#` to the end of the line is ignored by the interpreter.

```msla
# This is a comment
def var in -n  # End-of-line comment
```

Multi-line comments use three consecutive `#` characters (i.e., `###`) to indicate the start of a comment, and three consecutive `#` to indicate the end. All content between the two `###` markers is ignored by the interpreter.

```msla
###
This is the first line of a multi-line comment
This is the second line of a multi-line comment
This is the third line of a multi-line comment
###
def var in -n  ### This is not a multi-line comment because ### is not on its own line ###
```

### 2.2 Indentation

Certain statements (such as inside function type definitions, assignment blocks, modules, and conditional statements) require indentation to represent code hierarchy. Indentation must be consistent.

### 2.3 Keywords

MSLA keywords include: `def`, `var`, `fun`, `mod`, `ass`, `ins`, `defend`, `assend`, `modend`, `ifend`, `if`, `else`, `while`, `whileend`, `wait`, `check_tip`, `kill`, `run`, `jump`, `call`, `exit`. Keywords cannot be used as variable names, function names, or module names.

### 2.4 Statement Structure

Each statement occupies its own line. There is no syntax for writing multiple statements on a single line.

### 2.5 Initialization Code and Module Code

- **Initialization Code**: Code outside modules, executed only once when the program starts.
- **Module Code**: Code inside modules, which can be run multiple times through jumps and calls.

A code file may have no modules, but initialization code always executes first when the program starts.

### 2.6 Python Type Handling Function Compatibility

MSLA supports Python built-in functions for processing strings, floats, and integers, including but not limited to:

| Function    | Description                  |
| ----------- | ---------------------------- |
| `str()`     | Converts a value to a string |
| `int()`     | Converts a value to an integer |
| `float()`   | Converts a value to a float  |
| `len()`     | Gets the length of a string  |

```msla
a = str(123)        # Converts the number 123 to the string "123"
b = int("456")      # Converts the string "456" to the integer 456
c = float("3.14")   # Converts the string "3.14" to the float 3.14
d = len("hello")    # Gets the length of the string "hello", result is 5
```

In addition, common-sense operation rules between strings and floats follow Python syntax, for example, strings and floats cannot be multiplied.

***

## 3. Variable Definition and Assignment

### 3.1 Variable Definition (def var)

**Syntax**:

```
def var <variable_name> <-n | -w>
```

**Parameter Description**:

| Parameter | Meaning                                        |
| --------- | ---------------------------------------------- |
| `-n`      | The variable is used for numeric calculations (integers and floats) |
| `-w`      | The variable is used for string processing     |

When defining a variable, **one** of `-n` or `-w` must be specified.

**Example**:

```msla
def var count -n     # Defines a numeric variable named count
def var name -w      # Defines a string variable named name
```

**Rules**:

- All variables must first be defined via `def var` before they can be used.
- Variable names must not conflict with keywords.
- Redefining a variable with the same name is not allowed.

### 3.2 Variable Assignment (ass var)

**Syntax**:

```
ass var <variable_name> = <value>
```

**Assignment Rules**:

- The `ass` statement follows the **master-slave relationship** of the equals sign: the left side must be made equal to the right side, i.e., the value on the right is assigned to the variable on the left.
- When assigning, data types must match: numeric variables can only be assigned numeric values, and string variables can only be assigned string values.

**Example**:

```msla
def var in -n
ass var in = 1           # Assigns the numeric value 1 to in

def var greeting -w
ass var greeting = "hi"  # Assigns the string value "hi" to greeting
```

**Operation Assignment**:

The right side of the equals sign in an `ass` statement can contain arithmetic expressions.

```msla
ass var in = 1 + out     # Assigns the result of 1 + out to in
ass var in = a * b       # Assigns the result of a * b to in
```

### 3.3 Variable Matrices

A variable matrix is a multi-dimensional collection of variables. Only the same data type can be stored within a matrix.

**Definition Syntax**:

```
def var <matrix_name>_<range1>_<range2>_... <-n | -w>
```

**Dimensions and Ranges**:

- Range format is `[start:end]`, indicating that the dimension extends from the start index to the end index, inclusive of both ends.
- Dimension size = end - start + 1.
- Dimensions are referred to sequentially as the first dimension, second dimension, third dimension, etc.

**Example**:

```msla
# Defines a 3x3 numeric matrix (two dimensions, each of size 3)
def var input1_[0:2]_[0:2] -n

# Defines a one-dimensional numeric matrix (3 elements)
def var input2_[0:2] -n

# Defines a 3x3x3 three-dimensional numeric matrix
def var input3_[0:2]_[0:2]_[0:2] -n
```

**Global Assignment**:

An `ass` statement can be used to assign a uniform value to all elements in the entire matrix at once.

```msla
ass var input1 = 1  # Assigns 1 to all 9 values in the input1 matrix
```

**Coordinate Assignment**:

A specific coordinate within the matrix can be assigned a value individually.

```msla
ass var input1_[0]_[2] = 1  # Assigns 1 to the value at first dimension index 0, second dimension index 2
```

**Range Assignment**:

A range on a specific dimension within the matrix can be assigned, or a specified range in one matrix can be made equal to another variable matrix.

```msla
ass var input1_[0]_[0:2] = input2  # Makes the three values of input1 at first dimension index 0, second dimension 0 to 2, equal to the three values in input2 respectively
```

**Dimension Variable Parameters**:

In function matrices and matrix variables, variables can be used as parameters when specifying dimensions (except in `def` statements). When using a variable as a dimension parameter, the variable must be of integer type.

```msla
# Using a variable as a dimension parameter during assignment
ass var input1_[0:2]_[0:in] = 1   # Second dimension range is 0 to the value of variable in (in must be an integer)
ass var input1_[0]_[in] = 1       # Second dimension index is the value of variable in (in must be an integer)
```

**Note**: Variables cannot be used as dimension parameters in `def var` statements; dimension ranges must be constants.

**Variable Indexing Rules**:

When using variables to index function matrices and matrix variables, it is sufficient to index to at least one valid value, even if some dimensions have gone out of range.

### 3.4 Inter-Matrix Assignment Rules

When a variable matrix is set to be the same as another matrix variable, the following rules apply:

- **Same Dimension Size**: Directly make each value in each dimension correspond one-to-one as equal.
- **Different Dimension Size**: Prioritize making values of smaller dimensions equal. For example: if `input1` is a 3x3 two-dimensional matrix (9 values) and `input2` is a one-dimensional matrix (3 values), then only the three values of `input1`'s first dimension are made equal to the three values in `input2` in ascending index order.

```msla
def var input1_[0:2]_[0:2] -n
def var input2_[0:2] -n

ass var input1 = input2  # In input1: [0][0]=[0], [0][1]=[1], [0][2]=[2]; remaining values are unaffected
```

This rule also applies to addition, subtraction, multiplication, and division operations between matrices.

***

## 4. Operators

MSLA supports the following arithmetic operators:

| Operator | Meaning      | Example      |
| -------- | ------------ | ------------ |
| `+`      | Addition     | `a = b + c` |
| `-`      | Subtraction  | `a = b - c` |
| `*`      | Multiplication | `a = b * c` |
| `/`      | Division     | `a = b / c` |

MSLA supports the following comparison operators:

| Operator | Meaning              | Example      |
| -------- | -------------------- | ------------ |
| `>`      | Greater than         | `a > b`     |
| `<`      | Less than            | `a < b`     |
| `>=`     | Greater than or equal to | `a >= b`    |
| `=<`     | Less than or equal to | `a =< b`    |
| `!=`     | Not equal to         | `a != b`    |

**Operation Rules**:

- When operators appear on the right side of the equals sign in an `ass` statement, the equals sign master-slave relationship must be followed (make the left equal to the right).
- Addition, subtraction, multiplication, and division can be performed between numbers.
- Comparison operators are used in condition judgments of `if` and `while` statements, returning boolean values (true or false). `>`, `<`, `>=`, `=<`, `!=` are all comparison operators.
- When a function type has no parameter (i.e., neither `-n` nor `-w`), mixed operations are supported:
  - `a = b + len(str1)` — mixing numbers with string function return values
  - `a = b * c` — if `c` is a string and `b` is an integer, then `a` is the string `c` repeated `b` times (consistent with Python syntax)
- Illegal operations (such as multiplication) between strings and floats cannot be performed, following Python syntax rules.

***

## 5. Function Type Definition and Instantiation

### 5.1 Function Type Definition (def fun / defend)

**Syntax**:

```
def fun <function_type_name> <-n | -w | no parameter>
    def <value_name1> = <expression1>
    def <value_name2> = <expression2>
    ...
defend
```

**Parameter Description**:

| Parameter     | Meaning                                                                 |
| ------------- | ----------------------------------------------------------------------- |
| `-n`          | The function type is used for numeric calculations                      |
| `-w`          | The function type is used for string processing (string processing syntax is the same as Python, e.g., `len(str1)`, etc.) |
| No parameter  | The function type can mix string and numeric processing                 |

**Example**:

```msla
# Defines a numeric function type
def fun player -n
    def a = b + c        # Defines a as b plus c
    def d = SUM(e)       # Defines d as the sum of all values in e (WPS spreadsheet function)
defend

# Defines an untyped mixed function
def fun mixed
    def a = b + len(str1)   # Mixing numbers with string function return values
    def c = d * e           # If e is a string and d is an integer, then c is the string e repeated d times
defend
```

**Rules**:

- A function type definition begins with `def fun` and ends with `defend`; the two must appear in pairs.
- Internal definitions use the `def <value_name> = <expression>` format.
- WPS spreadsheet functions (such as `SUM`, `AVERAGE`, etc.) can be used in definitions to facilitate calculations. When using WPS spreadsheet functions, function names should be written in all uppercase.
- A function type cannot be used directly after definition; it must first be instantiated.

### 5.2 Function Instantiation (ins)

**Syntax**:

```
ins <function_type_name> <instance_function_name>
```

**Example**:

```msla
ins player player1   # Instantiates the player type into an instance function named player1
```

**Rules**:

- A function type can be instantiated multiple times.
- Instance function names after instantiation must not be duplicated.
- The instance function name can be the same as the original function type name.
- Only instantiated instance functions can be used for calculation and computation in code.

### 5.3 Function Instance Assignment (ass fun / assend)

**Syntax**:

```
ass fun <instance_function_name>
    <value_name1> = <value1>
    <value_name2>, <value_name3> = <value2>
    <value_name4> = <variable_name>
    ...
assend
```

**Assignment Methods**:

- **Direct Assignment**: Set a specific value in the instance function to a specified constant.
- **Multi-Value Simultaneous Assignment**: Multiple values can be defined on one line, separated by commas.
- **Link Variables**: Values in the instance function can be linked to a previously defined variable. Once linked, the value changes as the variable changes. The `ass` statement can later be used to reassign other values or variables.

**Example**:

```msla
ins player player1

ass fun player1
    c = 1                 # Assigns 1 to c
    a, b = 1              # Assigns 1 to both a and b
    e = in                # Links e to variable in; e changes with in
    z = out               # Links z to variable out
assend
```

**Rules**:

- An assignment block begins with `ass fun` and ends with `assend`; the two must appear in pairs.
- Linked variables must already exist (i.e., previously defined via `def var`).
- Values can also be defined as strings, e.g., `a = "hello"`.

### 5.4 Function Matrices

A function matrix instantiates a function type into a multi-dimensional matrix, where each element in the matrix is an independent instance function.

**Instantiation Syntax**:

```
ins <function_type_name> <instance_function_name>_<range1>_<range2>_...
```

**Example**:

```msla
# Instantiates the player type into a 3x3 function matrix
ins player player3_[0:2]_[0:2]

# Three-dimensional function matrix
ins player player4_[0:2]_[0:2]_[0:2]
```

**Assignment Syntax**:

```msla
ass fun player3_[0]_[0:2]
    c = 1               # Assigns 1 to c in the three instance functions of player3 at first dimension 0, second dimension 0 to 2
    b = input1          # Links to a variable matrix
    a = 1 - in          # Operation assignment
assend
```

Range specification rules for function matrix assignment are the same as for variable matrices.

### 5.5 Function Values as Variable References

A specified value in an instance function can be used like an ordinary variable by using the colon `:` reference syntax.

**Syntax**:

```
<instance_function_name>:<value_name>
```

For function matrices, coordinates must be added:

```
<instance_function_name>_[coord1]_[coord2]:<value_name>
```

**Example**:

```msla
# Referencing a value from a single instance function
ass var x = player1:a        # Assigns the value of a in player1 to variable x

# Referencing a value at a specified position in a function matrix
ass var y = player3_[0]_[0]:a   # Assigns the value of a in player3[0][0] to variable y
```

**Range Reference**:

```msla
# Treats the a values in the specified range of the player3 matrix as a variable matrix of the same dimension size
ass var result = player3_[0:2]_[0:2]:a
```

***

## 6. Control Flow

### 6.1 if / else if / else / ifend Conditional Statements

**Syntax**:

```
if(<condition>)
    <statement block>
else if(<condition>)
    <statement block>
else
    <statement block>
ifend
```

**Execution Logic**:

1. First, check the condition in `if`. If satisfied, execute the statement block under `if`, then jump to after `ifend` to continue execution.
2. If the `if` condition is not satisfied, check the conditions in `else if` sequentially (if present). If a certain `else if` is satisfied, execute its statement block, then jump to after `ifend`.
3. If none of the `if` and `else if` conditions are satisfied, execute the statement block under `else` (if present).
4. `ifend` is the closing marker for the conditional statement and must appear paired with `if`.
5. `if` statements can be **nested** multiple levels, meaning an `if` statement can be written inside the statement block of another `if` statement. Each level of nesting must have a corresponding `ifend` closure.

**Example**:

```msla
if(out > 3)
    print("yes1")
ifend

if(startfun:z > 4)
    print("Condition 1 satisfied")
else if(out > 5)
    print("Condition 2 satisfied")
else
    print("No conditions satisfied")
ifend
```

**Nested if Example**:

```msla
if(a > 0)
    if(b > 0)
        print("Both a and b are greater than 0")
    else
        print("a is greater than 0 but b is not greater than 0")
    ifend
else
    print("a is not greater than 0")
ifend
```

### 6.2 Multi-Condition Logic (Comma and Semicolon)

Multiple conditions can exist within a single `if` parentheses, connected by commas and semicolons:

| Connector | Meaning                                                  |
| --------- | -------------------------------------------------------- |
| `,`       | Logical AND — conditions separated by commas must all be satisfied simultaneously |
| `;`       | Logical OR — conditions separated by semicolons; satisfying any one is sufficient |

**Precedence**: Comma (AND) has higher precedence than semicolon (OR), meaning comma-connected condition groups are first merged, then semicolons are used to perform logical OR between groups.

**Syntax**:

```
if(<conditionA>, <conditionB> ; <conditionC>)
```

The above means: **(ConditionA AND ConditionB) OR ConditionC**.

**Example**:

```msla
# startfun:z>4 AND in>4, OR in>6
if(startfun:z > 4 , in > 4 ; in > 6)
    print("yes2")
ifend
```

### 6.3 while / whileend Loop Statements

**Syntax**:

```
while
    <statement block>
whileend

while (<condition>)
    <statement block>
whileend
```

**Unconditional Loop**:

When `while` is not followed by a condition, the statements inside the loop body will execute infinitely.

```msla
while
    print("a")
whileend
```

**Conditional Loop**:

Adding parentheses after `while` means the loop body is entered only when the condition inside the parentheses is satisfied. Multiple conditions can exist inside the parentheses, connected by commas and semicolons, using the same method as in `if` statements.

```msla
while (a > 1)
    print("a")
whileend
```

**Using if Statements Inside Loops**:

`if` statements can be used inside `while` statements.

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

**exit(while) to Exit Loop**:

The `exit(while)` statement is used to exit the current loop and continue executing the content after `whileend`.

**Rules**:

- A while statement begins with `while` and ends with `whileend`; the two must appear in pairs.
- While statements can be used both inside and outside modules.
- While statements can be nested.

***

## 7. Modules

### 7.1 Module Definition (def mod / modend)

**Syntax**:

```
def mod <module_name> [-main]
    <statement block>
modend
```

**Rules**:

- A module begins with `def mod` and ends with `modend`; the two must appear in pairs.
- Code inside a module can be executed multiple times (via `jump` or `call` instructions).
- Code outside modules (initialization code) is executed only once when the program starts.
- A code file may have no modules.

**Example**:

```msla
def mod amod
    print("hi")
    ass var in = 1
modend
```

### 7.2 -main Parameter

The `-main` parameter can be added to the `def mod` statement:

```
def mod <module_name> -main
```

**Meaning**: The `-main` parameter indicates that the code inside this module should be executed first after the initialization code finishes running.

**Example**:

```msla
def mod amod -main
    print("Program start")
modend
```

Only one module in a file should use the `-main` parameter.

### 7.3 jump Instruction

**Syntax**:

```
jump @ <module_name>
```

Or cross-file jump:

```
jump <file_name> <module_name>
```

**Description**:

- `@` indicates searching within the current file for the specified module to jump to.
- If `@` is replaced with the name of another `.msla` file (without the `.msla` extension), it indicates searching within the folder containing this file and its subfolders for the specified module in the corresponding `.msla` file.
- When jumping to a specified module in another `.msla` file, the initialization code in that file is executed first, followed by the specified module.

**Example**:

```msla
jump @ bmod          # Jumps to the bmod module in this file
jump otherfile bmod  # Jumps to the bmod module in otherfile.msla
```

**Rules**:

- `jump` always maintains a single process; after jumping, the target module is executed within the same process.
- The target module must exist; otherwise, a module not found error will be triggered.

### 7.4 call Command

**Syntax**:

```
call <file_name> <module_name>
```

Or calling within the current file:

```
call @ <module_name>
```

**Description**:

- The syntax of the `call` command is the same as `jump`.
- The difference is: `call` creates a new process in the background to execute the target module, while `jump` always maintains a single process.

**Example**:

```msla
call @ bmod            # Executes the bmod module of this file in a new process
call sha256 input      # Executes the input module in sha256.msla in a new process
```

**Grouping Syntax**:

```
call @ <module_name> ; <group_name>
```

**Description**:

- `call @ <module_name> ; <group_name>` — executes the module in parallel while assigning it to a specified group
- `call @ <module_name>` — can also be ungrouped
- After grouping, processes can be terminated by group using the `kill` statement

**Example**:

```msla
call @ amod ; a    # Execute the amod module in parallel, assigning it to group a
call @ amod        # Execute the amod module in parallel, ungrouped
```

### 7.5 kill Statement

**Syntax**:

```
kill                    # Stops all other processes except the current process
kill <group_name>       # Stops all processes in the specified group
kill <group1>,<group2>,...  # Stops all processes in the specified groups
kill N/A                # Stops all ungrouped processes
```

**Description**:

- The `kill` statement is used to stop processes.
- `kill` without parameters stops all other processes except the current process.
- `kill` with a group name stops all processes in the specified group.
- Multiple group names are separated by commas.
- `kill N/A` stops all ungrouped processes.

**Example**:

```msla
kill            # Stops all processes except self
kill a          # Stops all processes in group a
kill a,b,c      # Stops all processes in groups a, b, and c
kill N/A        # Stops all ungrouped processes
```

**Rules**:

- The `kill` statement is used in conjunction with the grouping feature of `call`.
- `kill` can only stop processes started via `call` and cannot stop the current process itself.

***

## 8. Other Commands

### 8.1 run Command

**Syntax**:

```
run "<file_path>"
```

**Description**:

- The `run` command is used to run other executable files on the computer (such as `.exe`, `.bat`, `.py` scripts, etc.).
- The path is enclosed in double quotes.

**Relative Path Syntax**:

- If the double-quoted string begins with `\\`, it indicates searching within the folder containing this `.msla` script for the specified file.

**Example**:

```msla
run "C:\game\main.exe"        # Runs an executable file at an absolute path
run "\\player\main.exe"       # Searches within the folder containing this script for player\main.exe
```

### 8.2 print Statement

**Syntax**:

```
print(<output_content>)
```

**Description**:

- The syntax of the `print` statement is the same as in Python.
- Python-style string operations can be used within `print`, such as `print(len(str1))`.

**Example**:

```msla
print("hi")
print(len(str1))
print(str1 , " world")
```

### 8.3 Printing Variable Matrices

The `print` statement can print variable matrices, but the following rules must be observed:

**Printable Conditions**:

A variable matrix can be printed only when it has **exactly two dimensions**, or when the specified region has **only two varying dimensions**.

| Situation                      | Printable   | Description                        |
| ------------------------------ | ----------- | ---------------------------------- |
| `input1_[0:2]_[0:2]`          | Yes         | Exactly two dimensions             |
| `input2_[0:2]_[0:2]_[0:2]`    | No          | Three dimensions, more than two varying dimensions |
| `input2_[0:2]_[0]_[0:2]`      | Yes         | Second dimension fixed at 0, only two varying dimensions |
| `input2_[0:2]_[0]`            | Yes         | Only one dimension, treated as a degenerate case |

**Printing Restrictions**:

- A single `print` statement can contain **only one variable matrix**, and must not simultaneously print other strings or numbers.

**Printing Effect**:

- The **smaller dimension** goes from left to right as the **horizontal axis**, and the **larger dimension** goes from top to bottom as the **vertical axis**.
- The corresponding content (string or number) is printed at the corresponding coordinate.
- Content is separated by **spaces**, with **adaptive spacing** (automatically adjusted based on content length).

**Example**:

```msla
def var input1_[0:2]_[0:2] -n   # Two-dimensional variable matrix
ass var input1 = 1
print(input1)                     # Prints the 3x3 variable matrix

def var input2_[0:2]_[0:2]_[0:2] -n   # Three-dimensional variable matrix
ass var input2 = 1
print(input2_[0:2]_[0]_[0:2])          # Fix the second dimension; only two varying dimensions, printable
```

### 8.4 Printing Function Matrix Values

Function matrices **themselves cannot** be printed with `print`, but when specific values in a specified range of a function matrix are treated as a variable matrix, they **can** be printed. Printing rules are the same as for printing variable matrices.

```msla
ins player player3_[0:2]_[0:2]   # 3x3 function matrix

# The function matrix itself cannot be printed
# print(player3)                # Error

# But when specific values in a specified range are treated as a variable matrix, they can be printed
print(player3_[0:2]_[0:2]:a)    # Treats all a values in player3 as a 3x3 variable matrix and prints them
print(player3_[0]_[0:2]:a)      # Limits first dimension to 0, prints a values in the second dimension range
```

### 8.5 exit Statement

**Syntax**:

```
exit(0)          # Exits all programs (including processes launched via call)
exit(1)          # Stops only the current process
exit(while)      # Exits the current while loop
exit()           # Equivalent to exit(0), exits all programs
```

**Description**:

- `exit(0)` — Exits all programs, including all processes launched via the `call` command.
- `exit(1)` — Stops only the current process; other processes are unaffected.
- `exit(while)` — Exits the current while loop and continues executing the code after `whileend`. This statement can only be used inside a while loop.
- `exit()` — Without parameters, equivalent to `exit(0)`.

**Example**:

```msla
exit(0)          # Exits all programs
exit(1)          # Stops only the current process

while (a > 0)
    if (a > 4)
        ass var a = a - 1
    else
        exit(while)    # Exits the loop
    ifend
whileend
```

### 8.6 input Command

**Syntax**:

```
input <variable_name> ("<prompt_text>") <-n | -w | no parameter>
```

**Parameter Description**:

| Parameter     | Meaning                                                                                                  |
| ------------- | -------------------------------------------------------------------------------------------------------- |
| `-n`          | Restricts user input to numbers only; the variable must be of numeric type. Characters other than digits and the decimal point are not displayed during input. |
| `-w`          | Restricts user input to text only; the variable must be of text type. Non-text characters are not displayed during input. |
| No parameter  | Allows mixed input of numbers and text; all characters are displayed normally.                           |

**Description**:

- The `input` command is used to receive user input and assign the input value to the specified variable.
- When executed, it first prints the prompt text, then waits for user input on the same line. After the user confirms with Enter, the input content is assigned to the variable.

**Example**:

```msla
def var in -n
input in ("Please enter:") -n       # Prints "Please enter:", waits for user to input a number, assigns to in

def var name -w
input name ("Please enter name:") -w  # Prints "Please enter name:", waits for user to input text, assigns to name

def var data -n
input data ("Please enter data:")     # Prints "Please enter data:", allows mixed input of numbers and text
```

### 8.7 wait Statement

**Syntax**:

```
wait(<seconds>)         # Timed wait
wait(<checkpoint_name>) # Checkpoint wait
```

**Description**:

- The `wait` statement is used to pause program execution.
- Timed wait: `wait(20.5)` means wait 20.5 seconds before continuing execution. The parameter is a number, supporting decimals.
- Checkpoint wait: `wait(now)` means the program pauses, waiting for the checkpoint named `now` to broadcast before continuing execution. The parameter is the checkpoint name (a non-numeric string).
- Checkpoint wait must be used in conjunction with the `check_tip` statement.

**Example**:

```msla
wait(20.5)          # Waits 20.5 seconds
wait(now)           # Waits for a broadcast from checkpoint named now
```

### 8.8 check_tip Statement

**Syntax**:

```
check_tip <checkpoint_name>
```

**Description**:

- The `check_tip` statement is used to place a checkpoint. When program execution reaches this line, it broadcasts with the specified name.
- All `wait` statements waiting for that checkpoint will be awakened and continue executing subsequent code.
- The checkpoint name is user-defined.

**Example**:

```msla
check_tip now       # Places a checkpoint named now; broadcasts now when execution reaches this line
```

**Combined Usage Example (wait and check_tip)**:

```msla
# Process A
wait(now)           # Waits for checkpoint now broadcast

# Process B
check_tip now       # When execution reaches here, broadcasts now, awakening Process A
```

***

## 9. Package System

MSLA supports extending functionality through a Package mechanism. A package is a series of `.dll` files that can be read by the interpreter or compiled program.

**Basic Rules**:

- `.dll` packages must be in the same project folder as the `.msla` script or compiled `.exe` file.

### 9.1 Package Import (import)

**Syntax**:

```
import <package_name> <alias>
```

**Alias Rules**:

- An alias consists of one English period followed by one to three English characters, e.g., `.m`, `.ab`, `.abc`.
- Each package can be imported multiple times with different aliases; in this case, the packages are independent of each other (because some packages create variables upon import).
- Within an entire project folder, aliases used each time a package is imported must not be duplicated.

**Example**:

```msla
import printpro .m    # Imports the printpro package with alias .m
import base64 .a      # Imports the base64 package with alias .a
import base64 .b      # Imports the same base64 package again with alias .b, independent from .a
```

### 9.2 Package Usage

**Syntax**:

```
<alias> <package_supported_statement>
```

**Description**:

- To execute statements within a package, first write the specified package's alias, then write the statement supported by the package.

**Example**:

```msla
import printpro .m
.m printpro("aaa")    # Uses the .m alias to call the printpro statement of the printpro package
```

**Variable Assignment and Reference**:

When variable parameters are defined in a grammar (e.g., `grammar: base64 "<intext>"`), in an msla script, variables can be assigned via the following methods:

1. **Direct Assignment**: Assign constant values to variables defined in the grammar. Double quotes are part of the syntax format specified in the grammar statement and are not part of the actual input content.

```msla
import base64 .a
.a:base64 "test"             # Assigns the test string to the intext variable defined in the grammar
```

2. **Variable Reference**: Use the `<variable_name>` format to assign the value of a variable in the msla script to a variable defined in the grammar.

```msla
.a:base64 "<in>"             # Assigns the value of the in variable to the intext variable
```

3. **Format Error**: If the usage format does not match the format defined in the grammar statement, a SyntaxFormatError will be returned.

```msla
.a:base64 <in>               # Error: format does not match base64 "<intext>" defined in the grammar
```

### 9.3 MSLB Package Creation

In addition to directly creating packages in other languages and compiling them into `.dll` files, users can also write `.mslb` scripts and use a package creator to build packages.

**MSLB Script Syntax**:

```
bagname <package_name>

def var <variable_name> <-n | -w>     # Defines variables carried by the package, which can be used or referenced globally

code_type <language_identifier>       # Specifies the programming language used in the script: py for Python, cpp for C++, c for C, msla for MSLA

line <statement_name>                  # Begins defining a syntax in the package
    grammar: <grammar_rule>           # Defines the usage format of the syntax
    content                           # Begins defining the actual script corresponding to the syntax
        <script_content>
    contentend                        # End of script definition
lineend                                # End of syntax definition
```

**Syntax Description**:

| Keyword                    | Meaning                                                                                              |
| -------------------------- | ---------------------------------------------------------------------------------------------------- |
| `bagname`                  | Defines the package name                                                                             |
| `def var`                  | Defines variables in the package. These variables can be used or referenced globally and can also be used by statements within the package. Users can only access package-internal variables created this way in msla scripts. |
| `code_type`                | Specifies the programming language used in the script: `py` for Python, `cpp` for C++, `c` for C, `msla` for MSLA language |
| `line` / `lineend`         | Defines a syntax in the package; must appear in pairs                                                  |
| `grammar:`                 | Defines the usage format of the syntax. `<variable_name>` indicates that the variable's value is also defined in this statement. |
| `content` / `contentend`   | Defines the actual script content corresponding to the syntax; must appear in pairs                    |

**Complete Example**:

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

**Description**:

- After writing an `.mslb` script, the user must use a package creator to build it into a `.dll` file.
- Since `.mslb` scripts involve other programming languages, users must have the corresponding language compiler installed.

**Format Validation**:

When a user uses a package statement in an msla script, if the usage format does not match the format defined in the grammar statement, the interpreter will return a SyntaxFormatError.

For example, if the grammar is defined as `base64 "<intext>"`, but the user writes `.a:base64 <in>` (missing double quotes), a SyntaxFormatError is triggered.

### 9.4 Complete Package Usage Example

The following example demonstrates the complete flow from importing a package to calling package statements:

```msla
import base64 .a
.a base64_help
.a base64 "hello"
exit()
```

**Execution Output**:

```
base64 <text>
aGVsbG8=
```

Where `aGVsbG8=` is the Base64 encoded result of `hello`.

***

## 10. Complete Example Program

Below is a simple complete MSLA program demonstrating core syntax such as variable definition, function definition and instantiation, module definition, conditional judgment, and jumps:

```msla
# Defines the main function type
def fun main -n
    def z = a + b + c
defend

# Defines variables
def var in -n
def var out -n

# Instantiates the function
ins main startfun

# Assigns values to the instance function
ass fun startfun
    a, b = 1
    c = in
    z = out
assend

# Defines the main module (-main means this module is executed first after initialization code)
def mod amod -main
    print("hi")

    ass var in = 1

    if(out > 3)
        print("yes1")
    ifend

    jump @ bmod

    call sha256 input
modend

# Defines another module
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

## 11. Error Types and Causes

### 11.1 UndefinedVariableError

**Trigger Scenario**: Using a variable name that has not been defined via `def var`.

**Example**:

```msla
ass var x = unknown_var   # unknown_var has not been defined
```

**Cause**: All variables must first be defined via `def var` before they can be used. Add a `def var <variable_name> <-n | -w>` statement before use.

***

### 11.2 UndefinedFunTypeError

**Trigger Scenario**: Instantiating a function type that has not been defined via `def fun`.

**Example**:

```msla
ins unknown_fun f1   # unknown_fun has not been defined
```

**Cause**: A function type must first be defined via `def fun` before it can be instantiated. Define the corresponding function type first.

***

### 11.3 UninstantiatedFunError

**Trigger Scenario**: Using a function that has not been instantiated via `ins`.

**Example**:

```msla
ass fun player1      # player1 has not been instantiated
    a = 1
assend
```

**Cause**: A function type cannot be used directly after definition; it must first be instantiated into an instance function via `ins`.

***

### 11.4 TypeMismatchError

**Trigger Scenario**: Assigning a string value to a numeric variable, or assigning a numeric value to a string variable.

**Example**:

```msla
def var count -n
ass var count = "hello"   # Numeric variable cannot be assigned a string value

def var name -w
ass var name = 123        # String variable cannot be assigned a numeric value
```

**Cause**: The data type of a variable is determined by the `-n` or `-w` parameter in `def var`. The type must remain consistent during assignment.

***

### 11.5 DimensionMismatchWarning

**Trigger Scenario**: Inconsistent dimension sizes during inter-matrix assignment.

**Example**:

```msla
def var input1_[0:2]_[0:2] -n
def var input2_[0:2] -n
ass var input1 = input2   # input1 is a two-dimensional matrix, input2 is a one-dimensional matrix
```

**Cause**: When two matrices have different dimensions, the system prioritizes making values of smaller dimensions equal. Uncovered values remain unchanged. This is a warning; the program can still execute, but confirm whether it matches expectations.

***

### 11.6 DuplicateDefinitionError

**Trigger Scenario**: Redefining a variable or function type with the same name.

**Example**:

```msla
def var count -n
def var count -n      # Duplicate definition of a variable with the same name

def fun player -n
    def a = b + c
defend
def fun player -n     # Duplicate definition of a function type with the same name
    def a = b * c
defend
```

**Cause**: Variable names and function type names must be unique within the same scope; duplicate definitions are not allowed.

***

### 11.7 DuplicateInstantiationError

**Trigger Scenario**: Duplicate instantiation of an instance function with the same name.

**Example**:

```msla
ins player player1
ins player player1    # Duplicate instantiation of an instance function with the same name
```

**Cause**: Instance function names must be unique. Duplicate instantiation of an instance function with the same name is not allowed (even from different function types).

***

### 11.8 MissingParameterError

**Trigger Scenario**: The `def var` statement is missing the `-n` or `-w` parameter.

**Example**:

```msla
def var count         # Missing -n or -w parameter
```

**Cause**: When defining a variable, either the `-n` (numeric type) or `-w` (string type) parameter must be specified.

***

### 11.9 SyntaxFormatError

**Trigger Scenario**: Keyword misspelling, missing closing markers, etc.

**Example**:

```msla
# Keyword misspelling
df var count -n              # "df" should be "def"

# Missing defend closing marker
def fun player -n
    def a = b + c            # Missing defend

# Missing assend closing marker
ass fun player1
    c = 1                    # Missing assend

# Missing modend closing marker
def mod amod
    print("hi")              # Missing modend

# Missing ifend closing marker
if(out > 3)
    print("yes")             # Missing ifend
```

**Cause**: Every block structure in MSLA has a corresponding closing marker and must be used in pairs: `def fun`/`defend`, `ass fun`/`assend`, `def mod`/`modend`, `if`/`ifend`. Keywords must be spelled correctly.

***

### 11.10 AssignmentDirectionError

**Trigger Scenario**: Violating the equals sign master-slave relationship in an `ass` statement, i.e., attempting to make the right side equal to the left side.

**Description**: MSLA's `ass` statement strictly follows the master-slave relationship where **the left side is assigned the value of the right side**. The left side of the equals sign must be an assignable target (variable name or matrix coordinate), and the right side is an expression or value.

**Example**:

```msla
# Error: attempting to assign in to 1, rather than assigning 1 to in
ass var 1 = in
```

**Cause**: In an `ass` statement, the left side of the equals sign must be the assignment target, and the right side is the value or expression. The master-slave relationship cannot be reversed.

***

### 11.11 ModuleNotFoundError

**Trigger Scenario**: A `jump` or `call` instruction points to a non-existent module or file.

**Example**:

```msla
jump @ nonexistent_mod      # The nonexistent_mod module does not exist in this file
jump missingfile bmod       # The missingfile.msla file cannot be found
```

**Cause**: The target module name or file name does not exist. Check whether the module name is spelled correctly, or whether the target `.msla` file exists in the current folder and its subfolders.

***

### 11.12 FileNotFoundError

**Trigger Scenario**: The `run` command points to a non-existent executable file.

**Example**:

```msla
run "C:\nonexistent\program.exe"   # The file path does not exist
run "\\missing\script.py"          # The file does not exist within the folder containing this script
```

**Cause**: The file path specified in the `run` command does not exist. Check whether the file path is correct, and when using the `\\` relative path, whether the file is within the folder containing this script.

***

### 11.13 MixedTypeOperationError

**Trigger Scenario**: Performing illegal operations between different types, such as strings and floats.

**Example**:

```msla
def var num -n
def var str -w
ass var num = 3.14
ass var str = "hello"
ass var result = num + str   # Numbers and strings cannot be directly added
```

**Cause**: Arithmetic operations such as addition, subtraction, multiplication, and division cannot be performed between numbers and strings. String repetition (e.g., integer * string) is only supported in function types without the `-n` or `-w` parameter, following Python syntax rules.

***

### 11.14 MatrixIndexOutOfBoundsError

**Trigger Scenario**: Accessing a matrix with an index that is out of the defined range.

**Example**:

```msla
def var input_[0:2] -n         # Matrix index range is 0~2
ass var input_[3] = 1          # Index 3 is out of range
ass var input_[-1] = 1         # Index -1 is out of range
```

**Cause**: The index range of a matrix is determined by `[start:end]` in `def var`. When accessing, the index must be within the start-to-end range (inclusive of both ends).

***

### 11.15 MismatchedEndMarkerError

**Trigger Scenario**: A closing marker does not match the opening marker.

**Example**:

```msla
def fun player -n
    def a = b + c
assend                        # Should be defend, not assend

ass fun player1
    c = 1
defend                        # Should be assend, not defend
```

**Cause**: Each block structure has a corresponding closing marker: `def fun` corresponds to `defend`, `ass fun` corresponds to `assend`, `def mod` corresponds to `modend`, `if` corresponds to `ifend`. The correct closing marker must be used.

***

### 11.16 MatrixPrintDimensionError

**Trigger Scenario**: Attempting to use `print` to print a variable matrix or function matrix value with more than two varying dimensions.

**Example**:

```msla
def var input2_[0:2]_[0:2]_[0:2] -n   # Three-dimensional variable matrix
ass var input2 = 1
print(input2)                          # Error: three varying dimensions, cannot print

ins player player3_[0:2]_[0:2]_[0:2]   # Three-dimensional function matrix
print(player3_[0:2]_[0:2]_[0:2]:a)     # Error: three varying dimensions, cannot print
```

**Cause**: When printing a variable matrix with `print`, the matrix must have exactly two dimensions, or the specified region must have only two varying dimensions. Matrices with more than two varying dimensions cannot be displayed in two-dimensional table form.

***

### 11.17 MatrixPrintMixedContentError

**Trigger Scenario**: Including other strings or numbers alongside a variable matrix in a `print` statement.

**Example**:

```msla
def var input1_[0:2]_[0:2] -n
ass var input1 = 1
print(input1, "Result is as follows")    # Error: cannot include other content when printing a variable matrix
print("Value:", input1)                  # Error: cannot include other content when printing a variable matrix
```

**Cause**: When a `print` statement contains a variable matrix, the statement can contain **only one variable matrix** and must not simultaneously print other strings or numbers. Variable matrices are printed in two-dimensional table form and cannot be mixed with other content in layout.

***

### 11.18 WhileNotInLoopError

**Trigger Scenario**: Using the `exit(while)` statement outside of a while loop.

**Example**:

```msla
if(a > 0)
    exit(while)    # Error: not inside a while loop, cannot use exit(while)
ifend
```

**Cause**: `exit(while)` can only be used inside a while loop to exit the current loop. Using this statement outside a loop will trigger an error.

***

### 11.19 CheckpointNotFoundError

**Trigger Scenario**: The checkpoint name specified in a `wait` statement does not exist (no corresponding `check_tip` statement).

**Example**:

```msla
wait(unknown_tip)    # A checkpoint named unknown_tip does not exist
```

**Cause**: The checkpoint that a `wait` statement waits for must be defined via a `check_tip` statement. Check whether the checkpoint name is spelled correctly and whether the corresponding `check_tip` statement exists.

***

### 11.20 KillTargetNotFoundError

**Trigger Scenario**: The group name specified in a `kill` statement does not exist (the group was not created via a `call` statement).

**Example**:

```msla
kill nonexistent_group    # A group named nonexistent_group does not exist
```

**Cause**: The group name specified in a `kill` statement must have been created via a `call @ <module_name> ; <group_name>` statement. Check whether the group name is spelled correctly and whether the corresponding `call` grouping statement has been executed.

***

### 11.21 PackageGrammarFormatMismatchError

**Trigger Scenario**: When using a package statement in an msla script, the usage format does not match the format defined in the grammar statement.

**Example**:

```msla
import base64 .a
# grammar is defined as base64 "<intext>"
.a:base64 <in>    # Error: missing double quotes, does not match the format defined in the grammar
```

**Cause**: The usage format of a package statement must strictly match the format defined in the `grammar:` of the `.mslb` script. Format markers such as double quotes and angle brackets cannot be omitted or changed. This error is a specific manifestation of SyntaxFormatError in the context of package usage.

***

*This document is written based on the MSLA Beta 0.0.2 syntax specification. For situations not covered in this document, Python syntax rules are followed by default.*
