/*
 * @File: StringBuilder.c
 * @Author: Aldrin John O. Manalansan (ajom)
 * @Email: aldrinjohnolaermanalansan@gmail.com
 * @Brief: Dynamically construct strings without worrying about the allocated memory size
 * @LastUpdate: June 16, 2025
 * 
 * Copyright (C) 2025  Aldrin John O. Manalansan  <aldrinjohnolaermanalansan@gmail.com>
 * 
 * This Source Code is served under Open-Source AJOM License
 * You should have received a copy of License_OS-AJOM
 * along with this source code. If not, see:
 * <https://raw.githubusercontent.com/Aldrin-John-Olaer-Manalansan/AJOM_License/refs/heads/main/LICENSE_AJOM-OS>
 */

#include "StringBuilder.h"

#include <stdio.h>
#include <string.h>

/* Returns a pointer to stringbuilder's string content with offset
 * WARNING: This is accessing a pointer that could change when the stringbuilder expands
 * Only use this function if you know what you're doing
 */
char* StringBuilder_GetStringWithOffset(const stringbuilder_t* const _stringBuilder, const uintptr_t offset) {
	return (UINTPTR_MAX == offset) ? NULL : _stringBuilder->string + offset;
}

// strlen() version of the stringbuilder's contents
size_t StringBuilder_GetUsedLength(stringbuilder_t* const _stringBuilder) {
	return _stringBuilder->endPtr - _stringBuilder->string;
}

// Removes number of characters at the left of the write offset
// Returns the number of characters deleted
size_t StringBuilder_Delete(stringbuilder_t* const _stringBuilder, size_t _length) {
	_length = BinaryBuilder_Delete((binarybuilder_t* const)_stringBuilder, _length); // get number of characters deleted
	if (_length) {
		*_stringBuilder->endPtr = 0; // null terminator
	}
	return _length;
}

/* Inserts a single character at the write offset without deleting any characters at the current string
 * Returns an offset to where the byte got written
 * Returns -1 if the byte wasn't written
 */
uintptr_t StringBuilder_InsertCharacter(stringbuilder_t* const _stringBuilder, const char character) {
	if (UINTPTR_MAX == StringBuilder_ReserveStringLength(_stringBuilder, 1)) {
		return UINTPTR_MAX; // insufficient memory
	}
	const uintptr_t initialOffset = _stringBuilder->writePtr - _stringBuilder->string;
	if (_stringBuilder->writePtr < _stringBuilder->endPtr) { // write pointer is in between the string content of the buffer
		memmove(_stringBuilder->writePtr + 1, _stringBuilder->writePtr, _stringBuilder->endPtr - _stringBuilder->writePtr);
	}
	*_stringBuilder->writePtr = character; // change the character at the write pointer to our desired character
	_stringBuilder->writePtr++;
	_stringBuilder->endPtr++;
	*_stringBuilder->endPtr = 0; // null terminator
	return initialOffset;
}

/* Inserts a number of characters at the write offset without deleting any characters at the current string
 * if _source = 0x0 to 0xFF , fills the field with this character, for example:
 * _str = 0x61('a') will set each character at the field as 'a'
 * else, set _str with a valid pointer where the copied data is located
 * Returns an offset to where the bytes got written
 * Returns -1 if the bytes wasn't written
 */
uintptr_t StringBuilder_InsertCharacters(stringbuilder_t* const _stringBuilder, const char* restrict const _str, const size_t _length) {
	const uintptr_t initialOffset = _stringBuilder->writePtr - _stringBuilder->string;
	// make sure we have space for the inserted string + backup string + null terminator
	if ((UINTPTR_MAX == StringBuilder_ReserveStringLength(_stringBuilder, _length * 2))
	|| (UINTPTR_MAX == BinaryBuilder_InsertBytes((binarybuilder_t* const)_stringBuilder, _str, _length))) {
		return UINTPTR_MAX; // insufficient memory
	}
	*_stringBuilder->endPtr = 0; // null terminator
	return initialOffset;
}

/* Inserts a normal string at the write offset without deleting any characters at the current string
 * if _source = 0x0 to 0xFF , fills the field with this character, for example:
 * _str = 0x61('a') will set each character at the field as 'a'
 * else, set _str with a valid pointer where the copied data is located
 * Returns an offset to where the bytes got written
 * Returns -1 if the bytes wasn't written
 */
uintptr_t StringBuilder_InsertString(stringbuilder_t* const _stringBuilder, const char* restrict const _str) {
	return StringBuilder_InsertCharacters(_stringBuilder, _str, strlen(_str));
}

/* Inserts a formatted string (with or without args) at the write offset without deleting any characters at the current string
 * Returns an offset to where the bytes got written
 * Returns -1 if the bytes wasn't written
 */
uintptr_t StringBuilder_InsertFormattedString(stringbuilder_t* const _stringBuilder, const char* _format, ...) {
	va_list _args;
    va_start(_args, _format); // this can only be used once

	// Use a copy for the length calculation
	va_list _args_copy;
	va_copy(_args_copy, _args); // we create a backup so that the original va_list will not be consued
	const size_t _length = vsnprintf(NULL, 0, _format, _args_copy); // get length
	va_end(_args_copy); // since _args_copy is now consumed, this va_list clone is now useless, so we now free it
	// va_list _args can still be consumed at this point

	const size_t _formatSize = strlen(_format) + 1;
	if (UINTPTR_MAX == BinaryBuilder_ReserveSize((binarybuilder_t* const)_stringBuilder, _length + 1 + _formatSize)) {
		return UINTPTR_MAX; // insufficient memory
	}

	// clone the string so that it won't be messed up incase its stored inside the shifted region
	_format = memmove(_stringBuilder->endPtr + _length + 1, _format, _formatSize);

	if (_stringBuilder->writePtr < _stringBuilder->endPtr) { // write pointer is in between the string content of the buffer
		// shift the righthand substring to the right
		memmove(_stringBuilder->writePtr + _length, _stringBuilder->writePtr, _stringBuilder->endPtr - _stringBuilder->writePtr);
	}
	char savedChar = _stringBuilder->writePtr[_length]; // temporarily save the character before it becomes a null terminator later
	vsprintf(_stringBuilder->writePtr, _format, _args);
    va_end(_args);
	const uintptr_t initialOffset = _stringBuilder->writePtr - _stringBuilder->string;
	_stringBuilder->writePtr += _length;
	_stringBuilder->endPtr += _length;
	if (_stringBuilder->writePtr < _stringBuilder->endPtr) { // write pointer is in between the string content of the buffer
		*_stringBuilder->writePtr = savedChar; // recover back the original character
	}
	*_stringBuilder->endPtr = 0; // null terminator
	return initialOffset;
}

// Clears the String making it look blank/empty
void StringBuilder_Clear(stringbuilder_t* const _stringBuilder) {
	_stringBuilder->writePtr = _stringBuilder->string;
	_stringBuilder->endPtr = _stringBuilder->string;
	*_stringBuilder->endPtr = 0; // null terminator
}
