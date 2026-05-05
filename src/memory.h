#include "common.h"
#pragma once
#include <stdint.h>
void* growCapacity(int* oldCap);

#define growArray(type , pointer , oldCount , newCount)\
(type*)reallocate(pointer , sizeof(type)* (oldCount) ,\
sizeof(type) * (newCount))

#define freeArray(type , pointer , oldCount)\
reallocate(pointer , sizeof(type)* (oldCount) , 0)