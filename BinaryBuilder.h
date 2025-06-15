/*
 * @File: BinaryBuilder.h
 * @Author: Aldrin John O. Manalansan (ajom)
 * @Email: aldrinjohnolaermanalansan@gmail.com
 * @Brief: Dynamically construct binaries without worrying about the allocated memory size
 * @LastUpdate: June 16, 2025
 * 
 * Copyright (C) 2025  Aldrin John O. Manalansan  <aldrinjohnolaermanalansan@gmail.com>
 * 
 * This Source Code is served under Open-Source AJOM License
 * You should have received a copy of License_OS-AJOM
 * along with this source code. If not, see:
 * <https://raw.githubusercontent.com/Aldrin-John-Olaer-Manalansan/AJOM_License/refs/heads/main/LICENSE_AJOM-OS>
 */

#ifndef BINARYBUILDER_H
#define BINARYBUILDER_H

#include <stdint.h>
#include <stdbool.h>

// linux compatibility
#include <stddef.h>
#include <limits.h>

#define _BINARYBUILDER_INITIALCAPACITY 200
#define _BINARYBUILDER_BUFFEREXPANSIONRATE 0.5

typedef struct {
    size_t capacity;      // memory size of the buffer variable
    void* data;        // memory region where this variable uses as its data content
    void* writePtr;
    void* endPtr;        // buffer <= writePtr <= endPtr <= (buffer + capacity - 1)
	float expansionRate;	// how much memory is increased every memory expansion
} binarybuilder_t;

typedef struct {
    size_t capacity;
    void* data;
} binarydata_t;

bool BinaryBuilder_SetMinSize(binarybuilder_t* const _binaryBuilder, size_t _minCapacity);
uintptr_t BinaryBuilder_ReserveSize(binarybuilder_t* const _binaryBuilder, const size_t _reservedSize);
bool BinaryBuilder_SetWriteOffset(binarybuilder_t* const _binaryBuilder, const uintptr_t _offset);
bool BinaryBuilder_SetUsedSize(binarybuilder_t* const _binaryBuilder, const uintptr_t _offset);
size_t BinaryBuilder_Delete(binarybuilder_t* const _binaryBuilder, size_t _length);
uintptr_t BinaryBuilder_SetByte(binarybuilder_t* const _binaryBuilder, const uint8_t byte);
uintptr_t BinaryBuilder_SetBytes(binarybuilder_t* const _binaryBuilder, const void* const _source, const size_t _length);
uintptr_t BinaryBuilder_InsertByte(binarybuilder_t* const _binaryBuilder, const uint8_t byte);
uintptr_t BinaryBuilder_InsertBytes(binarybuilder_t* const _binaryBuilder, const void* const _source, const size_t _length);
void BinaryBuilder_Clear(binarybuilder_t* const _binaryBuilder);
void BinaryBuilder_FreeBuffer(binarybuilder_t* const _binaryBuilder);
void BinaryBuilder_Free(binarybuilder_t* _binaryBuilder);
binarybuilder_t* BinaryBuilder_Clone(binarybuilder_t* restrict _destination, const binarybuilder_t* restrict const _source);
binarybuilder_t* BinaryBuilder_InitWithMinSize(binarybuilder_t* _binaryBuilder, const size_t _minCapacity, const float _expansionRate);
binarybuilder_t* BinaryBuilder_SetAutoExpandWithMinSize(binarybuilder_t* const _binaryBuilder, const size_t _minCapacity, const float _expansionRate);
void BinaryBuilder_InitUsingBuffer(binarybuilder_t* const _binaryBuilder, void* const _data, const size_t _capacity);

void BinaryData_FreeBuffer(binarydata_t* const _binaryData);
void BinaryData_Free(binarydata_t* _binaryData);
bool BinaryData_SetMinSize(binarydata_t* const _binaryData, const size_t _minCapacity);
binarydata_t* BinaryData_Clone(binarydata_t* restrict _destination, const binarydata_t* restrict const _source);
binarydata_t* BinaryData_InitWithMinSize(binarydata_t* _binaryData, const size_t _minCapacity);

#define BinaryBuilder_Init(_binaryBuilder) BinaryBuilder_InitWithMinSize(_binaryBuilder, _BINARYBUILDER_INITIALCAPACITY, _BINARYBUILDER_BUFFEREXPANSIONRATE)
#define BinaryBuilder_SetAutoExpand(_binaryBuilder) BinaryBuilder_SetAutoExpandWithMinSize(_binaryBuilder, _BINARYBUILDER_INITIALCAPACITY, _BINARYBUILDER_BUFFEREXPANSIONRATE)
#define BinaryBuilder_GetData(_binaryBuilder) (_binaryBuilder)->data
#define BinaryBuilder_GetMaxSize(_binaryBuilder) (_binaryBuilder)->capacity
#define BinaryBuilder_GetCurrentSize(_binaryBuilder) ((size_t)((_binaryBuilder)->endPtr) - (size_t)((_binaryBuilder)->data))
#define BinaryBuilder_GetWriteOffset(_binaryBuilder) ((uintptr_t)((_binaryBuilder)->writePtr) - (uintptr_t)((_binaryBuilder)->data))
#define BinaryBuilder_GetExpansionRate(_binaryBuilder) ((_binaryBuilder)->expansionRate - 1.0)
#define BinaryBuilder_SetExpansionRate(_binaryBuilder, _expansionRate) ((_binaryBuilder)->expansionRate = (_expansionRate) + 1.0)

#define BinaryData_Init(_binaryData) BinaryData_InitWithMinSize(_binaryData, _BINARYBUILDER_INITIALCAPACITY)

#endif
