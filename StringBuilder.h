/*
 * @File: StringBuilder.h
 * @Author: Aldrin John O. Manalansan (ajom)
 * @Email: aldrinjohnolaermanalansan@gmail.com
 * @Brief: Dynamically construct strings without worrying about the allocated memory size
 * @LastUpdate: June 7, 2025
 * 
 * Copyright (C) 2025  Aldrin John O. Manalansan  <aldrinjohnolaermanalansan@gmail.com>
 * 
 * This Source Code is served under Open-Source AJOM License
 * You should have received a copy of License_OS-AJOM
 * along with this source code. If not, see:
 * <https://raw.githubusercontent.com/Aldrin-John-Olaer-Manalansan/AJOM_License/refs/heads/main/LICENSE_AJOM-OS>
 */

#pragma once

#include "BinaryBuilder.h"

#include <stdarg.h>

#define _STRINGBUILDER_INITIALCAPACITY 200
#define _STRINGBUILDER_BUFFEREXPANSIONRATE 0.5

typedef struct {
    size_t capacity;		// memory size of the buffer variable
    char* string;       	// memory region where this variable uses as its data content
    char* writePtr;
    char* endPtr;       	// buffer <= writePtr <= endPtr <= (buffer + capacity - 1)
	float expansionRate;	// how much memory is increased every memory expansion
} stringbuilder_t;

// assures the minimum size of the string buffer. Expanding its memory size if necessary
static inline bool StringBuilder_SetMinSize(stringbuilder_t* const _stringBuilder, const size_t _minCapacity) {
	return BinaryBuilder_SetMinSize((binarybuilder_t* const)_stringBuilder, _minCapacity);
}

// assures that the string buffer has enough unused characters + null terminator. Expanding its memory size if necessary
static inline uintptr_t StringBuilder_ReserveStringLength(stringbuilder_t* const _stringBuilder, const size_t _reservedStringLength) {
	return BinaryBuilder_ReserveSize((binarybuilder_t* const)_stringBuilder, _reservedStringLength + 1);
}

static inline bool StringBuilder_SetWriteOffset(stringbuilder_t* const _stringBuilder, const uintptr_t _offset) {
	return BinaryBuilder_SetWriteOffset((binarybuilder_t* const)_stringBuilder, _offset);
}

static inline void StringBuilder_FreeBuffer(stringbuilder_t* const _stringBuilder) {
	BinaryBuilder_FreeBuffer((binarybuilder_t* const)_stringBuilder);
}

static inline void StringBuilder_Free(stringbuilder_t* _stringBuilder) {
	BinaryBuilder_Free((binarybuilder_t*)_stringBuilder);
}

/* properly initializes the stringbuilder variable.
 * Allocates memory to the stringbuilder variable if its current value is NULL
 * Reallocates memory to the stringbuilder's buffer that satisfy the required minimum size
 */
static inline stringbuilder_t* StringBuilder_InitWithMinSize(stringbuilder_t* _stringBuilder, const size_t _minCapacity, const float _expansionRate) {
	return (stringbuilder_t*)BinaryBuilder_InitWithMinSize((binarybuilder_t*)_stringBuilder, _minCapacity, _expansionRate);
}

// Assures that the buffer of the stringbuilder is an auto-expanding one.
static inline stringbuilder_t* StringBuilder_SetAutoExpandWithMinSize(stringbuilder_t* const _stringBuilder, const size_t _minCapacity, const float _expansionRate) {
	return (stringbuilder_t*)BinaryBuilder_SetAutoExpandWithMinSize((binarybuilder_t* const)_stringBuilder, _minCapacity, _expansionRate);
}

// Initialize the stringbuilder to contain a fixed-sized non-expanding buffer
static inline void StringBuilder_InitUsingBuffer(stringbuilder_t* const _stringBuilder, char* const _buffer, const size_t _capacity) {
	BinaryBuilder_InitUsingBuffer((binarybuilder_t* const)_stringBuilder, _buffer, _capacity);
}

size_t StringBuilder_Delete(stringbuilder_t* const _stringBuilder, size_t _length);
bool StringBuilder_InsertCharacter(stringbuilder_t* const _stringBuilder, const char character);
bool StringBuilder_InsertCharacters(stringbuilder_t* const _stringBuilder, const char* restrict const _str, const size_t _length);
bool StringBuilder_InsertString(stringbuilder_t* const _stringBuilder, const char* restrict const _str);
bool StringBuilder_InsertFormattedString(stringbuilder_t* const _stringBuilder, const char* restrict const _format, ...);
void StringBuilder_Clear(stringbuilder_t* const _stringBuilder);

#define StringBuilder_Init(_stringBuilder) StringBuilder_InitWithMinSize(_stringBuilder, _STRINGBUILDER_INITIALCAPACITY, _STRINGBUILDER_BUFFEREXPANSIONRATE)
#define StringBuilder_SetAutoExpand(_stringBuilder) StringBuilder_SetAutoExpandWithMinSize(_stringBuilder, _STRINGBUILDER_INITIALCAPACITY, _STRINGBUILDER_BUFFEREXPANSIONRATE)
#define StringBuilder_GetExpansionRate(_stringBuilder) ((_stringBuilder)->expansionRate - 1.0)
#define StringBuilder_SetExpansionRate(_stringBuilder, _expansionRate) ((_stringBuilder)->expansionRate = (_expansionRate) + 1.0)