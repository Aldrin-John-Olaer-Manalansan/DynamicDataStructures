/*
 * @File: StringBuilder.c
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

#include "StringBuilder.h"

// Removes number of characters at the left of the write offset
size_t StringBuilder_Delete(stringbuilder_t* const _stringBuilder, size_t _length) {
	_length = BinaryBuilder_Delete((binarybuilder_t* const)_stringBuilder, _length); // get number of characters deleted
	if (_length) {
		*_stringBuilder->endPtr = 0; // null terminator
	}
	return _length;
}

// Inserts a single character at the write offset without deleting any characters at the current string
bool StringBuilder_InsertCharacter(stringbuilder_t* const _stringBuilder, const char character) {
	if (UINTPTR_MAX == StringBuilder_ReserveStringLength(_stringBuilder, 1)) {
		return false; // insufficient memory
	}
	if (_stringBuilder->writePtr < _stringBuilder->endPtr) { // write pointer is in between the string content of the buffer
		memmove(_stringBuilder->writePtr + 1, _stringBuilder->writePtr, _stringBuilder->endPtr - _stringBuilder->writePtr);
	}
	*_stringBuilder->writePtr = character; // change the character at the write pointer to our desired character
	_stringBuilder->writePtr++;
	_stringBuilder->endPtr++;
	*_stringBuilder->endPtr = 0; // null terminator
	return true;
}

// Inserts a normal string at the write offset without deleting any characters at the current string
bool StringBuilder_InsertCharacters(stringbuilder_t* const _stringBuilder, const char* restrict const _str, const size_t _length) {
	if (UINTPTR_MAX == StringBuilder_ReserveStringLength(_stringBuilder, _length)) {
		return false; // insufficient memory
	}
	if (_stringBuilder->writePtr < _stringBuilder->endPtr) { // write pointer is in between the string content of the buffer
		memmove(_stringBuilder->writePtr + _length, _stringBuilder->writePtr, _stringBuilder->endPtr - _stringBuilder->writePtr);
	}
	memcpy(_stringBuilder->writePtr, _str, _length);
	_stringBuilder->writePtr += _length;
	_stringBuilder->endPtr += _length;
	*_stringBuilder->endPtr = 0; // null terminator
	return true;
}

bool StringBuilder_InsertString(stringbuilder_t* const _stringBuilder, const char* restrict const _str) {
	return StringBuilder_InsertCharacters(_stringBuilder, _str, strlen(_str));
}

// Inserts a formatted string (with or without args) at the write offset without deleting any characters at the current string
bool StringBuilder_InsertFormattedString(stringbuilder_t* const _stringBuilder, const char* restrict const _format, ...) {
	va_list _args;
    va_start(_args, _format);
	size_t _length = vsnprintf(NULL, 0, _format, _args);
	if (UINTPTR_MAX == StringBuilder_ReserveStringLength(_stringBuilder, _length)) {
		return false; // insufficient memory
	}
	if (_stringBuilder->writePtr < _stringBuilder->endPtr) { // write pointer is in between the string content of the buffer
		memmove(_stringBuilder->writePtr + _length, _stringBuilder->writePtr, _stringBuilder->endPtr - _stringBuilder->writePtr);
	}
	char savedChar = _stringBuilder->writePtr[_length]; // temporarily save the character before it becomes a null terminator later
	vsprintf(_stringBuilder->writePtr, _format, _args);
    va_end(_args);
	_stringBuilder->writePtr += _length;
	_stringBuilder->endPtr += _length;
	if (_stringBuilder->writePtr < _stringBuilder->endPtr) { // write pointer is in between the string content of the buffer
		*_stringBuilder->writePtr = savedChar; // recover back the original character
	}
	*_stringBuilder->endPtr = 0; // null terminator
	return true;
}

// Clears the String making it look blank/empty
void StringBuilder_Clear(stringbuilder_t* const _stringBuilder) {
	_stringBuilder->writePtr = _stringBuilder->string;
	_stringBuilder->endPtr = _stringBuilder->string;
	*_stringBuilder->endPtr = 0; // null terminator
}