/*
 * @File: DynamicArray.h
 * @Author: Aldrin John O. Manalansan (ajom)
 * @Email: aldrinjohnolaermanalansan@gmail.com
 * @Brief: Dynamically construct Arrays with arbitrary size
 * @LastUpdate: June 17, 2025
 * 
 * Copyright (C) 2025  Aldrin John O. Manalansan  <aldrinjohnolaermanalansan@gmail.com>
 * 
 * This Source Code is served under Open-Source AJOM License
 * You should have received a copy of License_OS-AJOM
 * along with this source code. If not, see:
 * <https://raw.githubusercontent.com/Aldrin-John-Olaer-Manalansan/AJOM_License/refs/heads/main/LICENSE_AJOM-OS>
 */

#ifndef DYNAMICARRAY_H
#define DYNAMICARRAY_H

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

#define _object_DEFAULT_INITIALCOUNT 30
#define _object_DEFAULT_EXPANSIONRATE 1.5

typedef struct {
    void* array;            // array buffer
    size_t elementCount;    // how much elements is currently valid
    size_t maxElementCount; // max number of elements
    size_t elementSize;     // size per element
	float expansionRate;    // how much elements is additionally added everytime we expand
} dynamicarray_t;

bool DynamicArray_SetMinElementsWithSize(dynamicarray_t* const _object, const size_t _minCount, const size_t _elementSize);
bool DynamicArray_ReserveElements(dynamicarray_t* const _object, const size_t _reservedElementCount);
void DynamicArray_FreeBuffer(dynamicarray_t* const _object);
void DynamicArray_Free(dynamicarray_t* _object);
bool DynamicArray_Delete(dynamicarray_t* const _object, const size_t _deletedElementIndex);
bool DynamicArray_Insert(dynamicarray_t* restrict const _object, const size_t _index, const void* restrict const _value);
bool DynamicArray_Pop(dynamicarray_t* const _object);
bool DynamicArray_Push(dynamicarray_t* restrict const _object, const void* restrict const _value);
bool DynamicArray_HasValue(const dynamicarray_t* const _object, const void* const _value);
size_t DynamicArray_GetElementNumberContainingValue(const dynamicarray_t* const _object, size_t _startingElementNumber, const void* const _value);
dynamicarray_t* DynamicArray_InitAll(dynamicarray_t* _object, const size_t _elementSize, const size_t _minCount, const float _expansionRate);

#define DynamicArray_SetMinElements(_object, _minCount) DynamicArray_SetMinElementsWithSize(_object, _minCount, (_object)->elementSize)
#define DynamicArray_Clear(_object) ((_object)->elementCount = 0)
#define DynamicArray_Init(_object, _elementSize) DynamicArray_InitAll(_object, _elementSize, _object_DEFAULT_INITIALCOUNT, _object_DEFAULT_EXPANSIONRATE)

#endif
