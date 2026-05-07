#include "common.h"
#include "chunk.h"
#pragma once

void disassembleChunk(Chunk* chunk , const char* name);
void disassembleInstruction(Chunk* chunk , int offset);