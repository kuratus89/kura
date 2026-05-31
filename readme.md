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

