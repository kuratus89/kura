#include "common.h"
#include "chunk.h"
#include "scanner.h"
#pragma once

void disassembleChunk(Chunk* chunk , const char* name);
void disassembleInstruction(Chunk* chunk , int offset);
void disassembleScan(scar* fs);