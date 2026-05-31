# Kura

Kura is a statically typed , bytecode compiled scripting language written in C. It has hand written scanner , tokenizer , a custom made binary tree expression compiler and a stack based virtual machine.

---

## Table of Contents
- [Overview](#overview)
---

## Overview

Kura source files are processed through four stage pipeline:

```
Source (.kura) -> Scanner -> Tokenizer -> Compiler -> Virtual Machine
```
The scanner produce raw lexemes , tokenizer classifies them , compiler convert them to bytecode chunks and the VM interprets that chunk on a value stack.

---

## Pipeline

### Scanner ('scanner.c')
it iterate the raw source string character by character and cuts it into 'scanPtr' . each one holds starts pointer , end pointer , a byte length and a line number . It does not copy the source text which saves a lot of memory .It handles whitespace , string litrals  , interget/float , alphabetic identifiers , single and double character signs , and comments (both single line and multi line).

### Tokenizer ('token.c')
It iterates the scanPtr array and classifies each span into a 'tokenType'.It classify scans in priority order:

1) Single character signs ('(' , ')', '{' , '}' , ';' , '=',...)
2) Two character signs ('==' , '!=' , '<=' , '>=', '&&' , '||' , '+=' , '-=')
3) String litrals (spans surrounded by '"')
4) Integer/float numeric litrals
5) Data type keywords ('int' , 'float' , 'bool' , 'string')
6) General keywords ('if' , 'else' , 'while' , 'for', 'func' , 'return' , 'print' , 'break' , 'continue' , 'true' , 'false' , 'NULL' , 'exit')
7) Identifier (anything else)

Then these tokens are passed through 'functinize' , which seprates token streams into 'Tokens' which is a collection of tokens , each function has its own 'Tokens'.

### Compiler ('compiler.c')
Iterates Tokens of every function and output bytecode into 'Chunks' objects . it convert arithmetic expressions into a custom made binary expression tree , then the tree is traversed post order to produce stack machhine instructions. It track variable scopes with a trie based map. Functions are declared in a first pass so they can be forward called and compiled in a second pass.

### Virtual Machine ('vm.c')
Its a stack based virtual machine . It maintains:
- A value stack
- A call stack
- A variable arry


## Language Refrence

### Data types

| Keyword | Size |
|---------|------|
| 'int'   | 4 B  |
| 'float' | 4 B  |
| 'bool'  | 1 B  |
| 'char'  | 1 B  |
| 'string'| -    |
| 'vector'| -    |

### Variables

Variables should be declared with an explict type and should be assigned immediately.

```
int x = 45;
float y = 9.12;
bool flag = 1;
```

To updatet variables , we can use '=' , '+=' or '-=' operators:

```
x = x+1;
x += 10;
x -= 3;
```

### Operators

**Arithmetic**

| Operators  | Meaning        |
|------------|----------------|
| '+'        | Addition       |
| '-'        | Subtraction    |
| '*'        | Multiplication |
| '/'        | Division       |
| '%'        | Modulo         |
| '-'(unarry)| Negation       |

**Comparision**

| Operator | Meaning               |
|----------|-----------------------|
| '=='     | Equal                 |
| '!='     | Not equal             |
| '<'      | Less than             |
| '>'      | Greater then          |
| '<='     | Less than or equal    |
| '>='     | Greater than or equal |

**Logical**

| Operator | Meaning |
|----------|---------|
| '&&'     | And     |
| '||'     | Or      |
| '!'      | Not     |

Operator precedence : low -> high

```
|| -> && -> == , != , < , > , <= , >= -> + , - -> * , /
```

### Function

Functions should be declared with the 'func' keyword with their return data type , name and parameter list

```
func int add(int a , int b){
    return (a+b);
}
```

### Control Flow

It supports 'if'/'else' , 'while' , and 'for' loops.
We can also use 'break' and 'continue' to control loops.
And 'return' to exits a function with a return value.

```
if(x==9){
    x=5;
}
else {
    x=10;
}
while(x<=10){
    print x;
    x-=1;
}
```

### Print
'print' is a built in statement that stdout only integers.

```
int x = 10 +5;
print x;
```

### Comments

kura use two comment types:
``` 
# this is single line comment

// 
this is 
multi line 
comment
//

```

---

# Build

## Prequisites

1) Cmake
2) C compiler
3) Make/Ninja

On Linux:
```
sudo apt install cmake gcc make
```

On macos:
```
brew install cmake
xcode-select --install
```

On windows , install [CMake](https://cmake.org/download) and either MinGW-w64 or Visual Studio (with the Desktop development with c++ workload).
---

## Release build
It compiles without debug outputs

```
cmake -S . -B build/release -DCMAKE_BUILD_TYPE=Release
cmake --build build/release
```

The binary is written to:
```
bin/Release/kura.exe
```

Run it:

linux:
```
bin/Release/Kura path/to/source/kura
```
windows:
```
./bin/Release/Kura.exe path/to/source.kura
```

## Debug build

This build defines the DEBUG macro, which enables diagnostic output.

```
cmake -S . -B build/debug -DCMAKE_BUILD_TYPE=Debug
cmake --build build/debug
```

the binary is written to:
```
bin/Debug/Kura.exe
```

Run it:

Linux:
```
bin/Debug/Kura path/to/source.kura
```

Windows:
```
./bin/Debug/Kura.exe path/to/source.kura
```

---

### Switching between Release and Debug

You can keep both files at same time and rebuild either one independently:
```
cmake --build build/release
cmake --build build/debug
```