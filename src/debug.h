#pragma once
#include "common.h"
#include "chunk.h"
#include "scanner.h"
#include "token.h"
#include "vm.h"

void disassembleChunk(Chunk* chunk , const char* name);
void disassembleInstruction(Chunk* chunk , int offset);
void disassembleScan(scar* fs);
void disassembleToken(Token* token);
void disassembleTokens(Tokens* token);
char* disassembleTokenType(tokenType type);
void disassembleFuncToken(tokenFunctions* tf);
void disassembleStackVM(VM* vm);