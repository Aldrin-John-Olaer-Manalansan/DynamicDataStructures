/*
 * @File: DynamicStringArray.h
 * @Author: Aldrin John O. Manalansan (ajom)
 * @Email: aldrinjohnolaermanalansan@gmail.com
 * @Brief: Dynamically construct String Arrays with arbitrary size
 * @LastUpdate: June 17, 2025
 * 
 * Copyright (C) 2025  Aldrin John O. Manalansan  <aldrinjohnolaermanalansan@gmail.com>
 * 
 * This Source Code is served under Open-Source AJOM License
 * You should have received a copy of License_OS-AJOM
 * along with this source code. If not, see:
 * <https://raw.githubusercontent.com/Aldrin-John-Olaer-Manalansan/AJOM_License/refs/heads/main/LICENSE_AJOM-OS>
 */

#pragma once

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

#define _DYNAMICARRAY_DEFAULT_INITIALARRAYCOUNT 10
#define _DYNAMICARRAY_DEFAULT_INITIALBUFFERSIZE 100
#define _DYNAMICARRAY_DEFAULT_EXPANSIONRATE 0.5f

typedef struct {
    char **array;            // array of string pointers are stored here
    char *buffer;           // contents are stored here
    size_t elementCount;    // how much elements is currently valid
    size_t usedSize;        // how much buffer's size has been used
    size_t maxElementCount; // current memory size of the pointer array
    size_t bufferSize;      // current memory size of the buffer
	float expansionRate;    // how fast will the memory will expand
} dynamicstringarray_t;

bool DynamicStringArray_SetMinElements(dynamicstringarray_t* const _object, const size_t _minElementCount);
bool DynamicStringArray_SetMinBufferSize(dynamicstringarray_t* const _object, const size_t _minBufferSize);
bool DynamicStringArray_ReserveElements(dynamicstringarray_t* const _object, const size_t _reservedElementCount);
bool DynamicStringArray_ReserveBufferSize(dynamicstringarray_t* const _object, const size_t _reservedBufferSize);
void DynamicStringArray_FreeStorage(dynamicstringarray_t* const _object);
void DynamicStringArray_Free(dynamicstringarray_t* _object);
void DynamicStringArray_Clear(dynamicstringarray_t* _object);
bool DynamicStringArray_PushSubString(dynamicstringarray_t* restrict const _object, const char* restrict const _string, size_t _length);
bool DynamicStringArray_Pop(dynamicstringarray_t* const _object);
bool DynamicStringArray_InsertSubString(dynamicstringarray_t* restrict const _object, const size_t _index, const char* restrict const _string, size_t _length);
bool DynamicStringArray_Delete(dynamicstringarray_t* const _object, const size_t _deletedElementIndex);
size_t DynamicStringArray_Search(const dynamicstringarray_t* restrict const _object, const char* restrict const _seachedString, const bool _isCaseSensitive);
dynamicstringarray_t* DynamicStringArray_InitAll(dynamicstringarray_t* _object, const size_t _minElementCount, const size_t _minBufferSize, const float _expansionRate);

#define DynamicStringArray_Init(_object) DynamicStringArray_InitAll(_object, _DYNAMICARRAY_DEFAULT_INITIALARRAYCOUNT, _DYNAMICARRAY_DEFAULT_INITIALBUFFERSIZE, _DYNAMICARRAY_DEFAULT_EXPANSIONRATE)
#define DynamicStringArray_HasString(_object, _seachedString, _isCaseSensitive) (DynamicStringArray_Search(_object, _seachedString, _isCaseSensitive) != (size_t)-1)
#define DynamicStringArray_Push(_object, _string) DynamicStringArray_PushSubString(_object, _string, strlen(_string))
#define DynamicStringArray_Insert(_object, _index, _string) DynamicStringArray_InsertSubString(_object, _index, _string, strlen(_string))
