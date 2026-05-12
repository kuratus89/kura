#include "common.h"
#include "chunk.h"
#include "scanner.h"
#include "token.h"
#pragma once

void disassembleChunk(Chunk* chunk , const char* name);
void disassembleInstruction(Chunk* chunk , int offset);
void disassembleScan(scar* fs);
void disassembleToken(Token* token);
void disassembleTokens(Tokens* token);