/*
 * @File: BinaryBuilder.c
 * @Author: Aldrin John O. Manalansan (ajom)
 * @Email: aldrinjohnolaermanalansan@gmail.com
 * @Brief: Dynamically construct binaries without worrying about the allocated memory size
 * @LastUpdate: June 7, 2025
 * 
 * Copyright (C) 2025  Aldrin John O. Manalansan  <aldrinjohnolaermanalansan@gmail.com>
 * 
 * This Source Code is served under Open-Source AJOM License
 * You should have received a copy of License_OS-AJOM
 * along with this source code. If not, see:
 * <https://raw.githubusercontent.com/Aldrin-John-Olaer-Manalansan/AJOM_License/refs/heads/main/LICENSE_AJOM-OS>
 */

#include "BinaryBuilder.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// assures the minimum size of the binary buffer. Expanding its memory size if necessary
bool BinaryBuilder_SetMinSize(binarybuilder_t* const _binaryBuilder, size_t _minCapacity) {
	if (_binaryBuilder->capacity >= _minCapacity) { // buffer's current size already satisfied our size requirement
		return true;
	}
	if (_binaryBuilder->expansionRate <= 1.0) {
		return false; // automatic expansion isn't allowed, therefore initialization requirement wasn't met
	}
	_minCapacity *= _binaryBuilder->expansionRate;
	void* const expandedBuffer = realloc(_binaryBuilder->data, _minCapacity);
	if (!expandedBuffer) {
		return false; // failed expanding our buffer's size, therefore initialization requirement wasn't met
	}
	ptrdiff_t _offset = (ptrdiff_t)expandedBuffer - (ptrdiff_t)_binaryBuilder->data;
	_binaryBuilder->data = expandedBuffer;
	_binaryBuilder->capacity = _minCapacity;
	_binaryBuilder->writePtr = (uint8_t*)_binaryBuilder->writePtr + _offset;
	_binaryBuilder->endPtr = (uint8_t*)_binaryBuilder->endPtr + _offset;
	return true;
}

// assures that the binary buffer has enough unused size. Expanding its memory size if necessary
// returns the write offset if successful
// returns UINTPTR_MAX if memory expansion failed
uintptr_t BinaryBuilder_ReserveSize(binarybuilder_t* const _binaryBuilder, const size_t _reservedSize) {
	if ((((size_t)_binaryBuilder->data + _binaryBuilder->capacity) < ((size_t)_binaryBuilder->endPtr + _reservedSize)) // requires expansion
	&& ((_binaryBuilder->expansionRate <= 1.0) // doesn't allow expansion
	|| !BinaryBuilder_SetMinSize(_binaryBuilder, _binaryBuilder->capacity + _reservedSize))) {
		return UINTPTR_MAX; // failed expanding buffer
	}
	return BinaryBuilder_GetWriteOffset(_binaryBuilder);
}

bool BinaryBuilder_SetWriteOffset(binarybuilder_t* const _binaryBuilder, const uintptr_t _offset) {
	void* const newPtr = (uint8_t*)_binaryBuilder->data + _offset;
	if (newPtr > _binaryBuilder->endPtr) {
		return false; // offset is out of bounds
	}
	if (_binaryBuilder->writePtr != newPtr) {
		_binaryBuilder->writePtr = newPtr;
	}
	return true;
}

// Forcefully changes size of the data at the buffer.
// This does not change the size of the buffer.
// WARNING: Be careful on using this! Only use this function if you know what you're doing
// using a new end offset that is less than the current end offset is like using BinaryBuilder_Delete function
// using a new end offset that is greater than the current end offset will treat unmonitored bytes at the right side as part of the binary data
bool BinaryBuilder_SetUsedSize(binarybuilder_t* const _binaryBuilder, const uintptr_t _offset) {
	if (_offset >= _binaryBuilder->capacity) {
		return false; // offset is out of bounds
	}
	_binaryBuilder->endPtr = (uint8_t*)_binaryBuilder->data + _offset;
	if (_binaryBuilder->writePtr > _binaryBuilder->endPtr) {
		_binaryBuilder->writePtr = _binaryBuilder->endPtr;
	}
	return true;
}

// Removes number of bytes at the left of the write offset
// Returns the number of bytes deleted
size_t BinaryBuilder_Delete(binarybuilder_t* const _binaryBuilder, size_t _length) {
	size_t _leftHandLength = (size_t)_binaryBuilder->writePtr - (size_t)_binaryBuilder->data;
	if (_length > _leftHandLength) { // byte length is more than what is allowed at the lefthand side of our writer
		_length = _leftHandLength;
	}
	if (_length) { // there are still some character left to be deleted
		if (_binaryBuilder->writePtr < _binaryBuilder->endPtr) { // write pointer is in between the binary content of the buffer
			memmove(
				(uint8_t*)_binaryBuilder->writePtr - _length,
				_binaryBuilder->writePtr,
				(size_t)_binaryBuilder->endPtr - (size_t)_binaryBuilder->writePtr
			);
		}
		_binaryBuilder->writePtr = (uint8_t*)_binaryBuilder->writePtr - _length;
		_binaryBuilder->endPtr = (uint8_t*)_binaryBuilder->endPtr - _length;
	}
	return _length;
}

// Sets a single byte at the write offset without deleting any bytes at the current binary
bool BinaryBuilder_SetByte(binarybuilder_t* const _binaryBuilder, const uint8_t byte) {
	if (UINTPTR_MAX == BinaryBuilder_ReserveSize(_binaryBuilder, 1)) {
		return false; // insufficient memory
	}
	*(uint8_t*)_binaryBuilder->writePtr = byte; // change the byte at the write pointer to our desired byte
	void* const newWritePtr = (uint8_t*)_binaryBuilder->writePtr + 1;
	_binaryBuilder->writePtr = newWritePtr;
	if (_binaryBuilder->endPtr < newWritePtr) { // written bytes is beyond the endPtr
		_binaryBuilder->endPtr = newWritePtr; // update the endPtr
	}
	return true;
}

/* Writes a number of bytes at the write offset without deleting any bytes at the current binary
 * if _source = 0x0 to 0xFF , fills the field with this bytes, for example:
 * _source = 0x11 will set each byte at the field as 0x11
 * else, set _source with a valid pointer where the copied data is located
 */
bool BinaryBuilder_SetBytes(binarybuilder_t* const _binaryBuilder, const void* const _source, const size_t _length) {
	if (_length <= 0) {
		return false; // invalid length
	}
	if (UINTPTR_MAX == BinaryBuilder_ReserveSize(_binaryBuilder, _length)) {
		return false; // insufficient memory
	}
	void* const newWritePtr = (uint8_t*)_binaryBuilder->writePtr + _length;
	if ((uintptr_t)_source > 0xFF) {
		memmove(_binaryBuilder->writePtr, _source, _length);
	} else {
		memset(_binaryBuilder->writePtr, (int)((uintptr_t)_source & UINT_MAX), _length);
	}
	_binaryBuilder->writePtr = newWritePtr;
	if (_binaryBuilder->endPtr < newWritePtr) { // written bytes is beyond the endPtr
		_binaryBuilder->endPtr = newWritePtr; // update the endPtr
	}
	return true;
}

// Inserts a single byte at the write offset without deleting any bytes at the current binary
bool BinaryBuilder_InsertByte(binarybuilder_t* const _binaryBuilder, const uint8_t byte) {
	if (UINTPTR_MAX == BinaryBuilder_ReserveSize(_binaryBuilder, 1)) {
		return false; // insufficient memory
	}
	if (_binaryBuilder->writePtr < _binaryBuilder->endPtr) { // write pointer is in between the binary content of the buffer
		memmove((uint8_t*)_binaryBuilder->writePtr + 1, _binaryBuilder->writePtr, (size_t)_binaryBuilder->endPtr - (size_t)_binaryBuilder->writePtr);
	}
	*(uint8_t*)_binaryBuilder->writePtr = byte; // change the byte at the write pointer to our desired byte
	_binaryBuilder->writePtr = (uint8_t*)_binaryBuilder->writePtr + 1;
	_binaryBuilder->endPtr = (uint8_t*)_binaryBuilder->endPtr + 1;
	return true;
}

/* Inserts a number of bytes at the write offset without deleting any bytes at the current binary
 * if _source = 0x0 to 0xFF , fills the field with this bytes, for example:
 * else, set _source with a valid pointer where the copied data is located
 */
bool BinaryBuilder_InsertBytes(binarybuilder_t* const _binaryBuilder, const void* const _source, const size_t _length) {
	if (_binaryBuilder->writePtr >= _binaryBuilder->endPtr) { // write pointer is NOT in between the binary content of the buffer
		return BinaryBuilder_SetBytes(_binaryBuilder, _source, _length);
	}
	if (_length <= 0) {
		return false; // invalid length
	}

	if ((uintptr_t)_source > 0xFF) {
		if (UINTPTR_MAX == BinaryBuilder_ReserveSize(_binaryBuilder, _length * 2)) { // we will use a temporary storage to store itself
			return false; // insufficient memory
		}
		void* const storagePtr = (uint8_t*)_binaryBuilder->endPtr + _length;
		memmove(storagePtr, _source, _length); // temporarily store the source contents at the unused region, necessary to assure overlapping memories are transfered correctly
		memmove((uint8_t*)_binaryBuilder->writePtr + _length, _binaryBuilder->writePtr, (size_t)_binaryBuilder->endPtr - (size_t)_binaryBuilder->writePtr);
		memcpy(_binaryBuilder->writePtr, storagePtr, _length);
	} else {
		if (UINTPTR_MAX == BinaryBuilder_ReserveSize(_binaryBuilder, _length)) {
			return false; // insufficient memory
		}
		memmove((uint8_t*)_binaryBuilder->writePtr + _length, _binaryBuilder->writePtr, (size_t)_binaryBuilder->endPtr - (size_t)_binaryBuilder->writePtr);
		memset(_binaryBuilder->writePtr, (int)((uintptr_t)_source & UINT_MAX), _length);
	}

	_binaryBuilder->writePtr = (uint8_t*)_binaryBuilder->writePtr + _length;
	_binaryBuilder->endPtr = (uint8_t*)_binaryBuilder->endPtr + _length;
	return true;
}

// Clears the binary making it look blank/empty
void BinaryBuilder_Clear(binarybuilder_t* const _binaryBuilder) {
	_binaryBuilder->writePtr = _binaryBuilder->data;
	_binaryBuilder->endPtr = _binaryBuilder->data;
}

// Only frees the BinaryBuilder's buffer
void BinaryBuilder_FreeBuffer(binarybuilder_t* const _binaryBuilder) {
	if ((_binaryBuilder->expansionRate >= 1.0) && _binaryBuilder->data) { // has allocated auto-expanding buffer
		free(_binaryBuilder->data);
		_binaryBuilder->data = NULL;
	}
}

/* Frees a BinaryBuilder object
 * CAUTION! Do not pass pointer to a permanent BinaryBuilder variable!
 */
void BinaryBuilder_Free(binarybuilder_t* _binaryBuilder) {
	if ((_binaryBuilder->expansionRate >= 1.0) && _binaryBuilder->data) { // has allocated auto-expanding buffer
		free(_binaryBuilder->data);
	}
	free((void*)_binaryBuilder);
}

/* Properly initializes the binarybuilder variable.
 * Allocates memory to the binarybuilder variable if its current value is NULL
 * Reallocates memory to the binarybuilder's buffer that satisfy the required minimum size
 */
binarybuilder_t* BinaryBuilder_InitWithMinSize(binarybuilder_t* _binaryBuilder, const size_t _minCapacity, const float _expansionRate) {
	bool _mallocVar;
	if (!_binaryBuilder) { // if we are requesting to initialize it
		_binaryBuilder = malloc(sizeof(binarybuilder_t));
		if (!_binaryBuilder) { 
			return NULL; // failed allocating binarybuilder variable
		}
		_binaryBuilder->data = NULL; // indicate buffer requires initialization later
		_mallocVar = true;
	} else {
		_mallocVar = false;
	}
	_binaryBuilder->expansionRate = _expansionRate + 1.0;
	if (!_binaryBuilder->data) { // buffer isn't initialized yet
		_binaryBuilder->data = malloc(_minCapacity);
		if (!_binaryBuilder->data) {
			if (_mallocVar) {
				free(_binaryBuilder);
			}
			return NULL; // failed allocating buffer to our binarybuilder variable
		}
		_binaryBuilder->capacity = _minCapacity;
	} else if (!BinaryBuilder_SetMinSize(_binaryBuilder, _minCapacity)) {
		if (_mallocVar) {
			free(_binaryBuilder);
		}
		return NULL; // minimum size requrement didn't met
	}
	_binaryBuilder->writePtr = _binaryBuilder->data;
	_binaryBuilder->endPtr = _binaryBuilder->data;
	return _binaryBuilder; // initialization sucessful
}

// Assures that the buffer of the binarybuilder is an auto-expanding one.
binarybuilder_t* BinaryBuilder_SetAutoExpandWithMinSize(binarybuilder_t* const _binaryBuilder, const size_t _minCapacity, const float _expansionRate) {
	if (_binaryBuilder) { // binarybuilder variable was initialized
		if (_binaryBuilder->expansionRate >= 1.0) { // no changes required
			return _binaryBuilder;
		}
		_binaryBuilder->data = NULL; // indicate buffer requires initialization later
	}
	return BinaryBuilder_InitWithMinSize(_binaryBuilder, _minCapacity, _expansionRate); // reinitialize
}

// Initialize the binarybuilder to contain a fixed-sized non-expanding buffer
void BinaryBuilder_InitUsingBuffer(binarybuilder_t* const _binaryBuilder, void* const _data, const size_t _capacity) {
	if (_binaryBuilder->data && (_binaryBuilder->expansionRate >= 1.0))  { // allocated memory is an autoexpanding type of buffer
		free(_binaryBuilder->data);
	}
	_binaryBuilder->capacity = _capacity;
	_binaryBuilder->data = _data;
	_binaryBuilder->writePtr = _data;
	_binaryBuilder->endPtr = _data;
	_binaryBuilder->expansionRate = 0; // It's a manual buffer pointer. So we need to restrict autoexpansion
}

// Only frees the BinaryData's buffer
void BinaryData_FreeBuffer(binarydata_t* const _binaryData) {
	if (_binaryData->data) { // has allocated auto-expanding buffer
		free(_binaryData->data);
		_binaryData->data = NULL;
	}
}

/* Frees a BinaryData object
 * CAUTION! Do not pass pointer to a permanent BinaryData variable!
 */
void BinaryData_Free(binarydata_t* _binaryData) {
	if (_binaryData->data) { // has allocated auto-expanding buffer
		free(_binaryData->data);
	}
	free((void*)_binaryData);
}

// assures the minimum size of the binary buffer. Expanding its memory size if necessary
bool BinaryData_SetMinSize(binarydata_t* const _binaryData, const size_t _minCapacity) {
	if (_binaryData->capacity >= _minCapacity) { // buffer's current size already satisfied our size requirement
		return true;
	}
	void* expandedBuffer = realloc(_binaryData->data, _minCapacity);
	if (!expandedBuffer) {
		return false; // failed expanding our buffer's size, therefore initialization requirement wasn't met
	}
	_binaryData->data = expandedBuffer;
	_binaryData->capacity = _minCapacity;
	return true;
}

/* properly initializes the binarydata variable.
 * Allocates memory to the binarydata variable if its current value is NULL
 * Reallocates memory to the binarydata's buffer that satisfy the required minimum size
 */
binarydata_t* BinaryData_InitWithMinSize(binarydata_t* _binaryData, const size_t _minCapacity) {
	bool _mallocVar;
	if (!_binaryData) { // if we are requesting to initialize it
		_binaryData = malloc(sizeof(binarydata_t));
		if (!_binaryData) { 
			return NULL; // failed allocating BinaryData variable
		}
		_binaryData->data = NULL; // indicate buffer requires initialization later
		_mallocVar = true;
	} else {
		_mallocVar = false;
	}
	if (!_binaryData->data) { // buffer isn't initialized yet
		_binaryData->data = malloc(_minCapacity);
		if (!_binaryData->data) {
			if (_mallocVar) {
				free(_binaryData);
			}
			return NULL; // failed allocating buffer to our BinaryData variable
		}
		_binaryData->capacity = _minCapacity;
	} else if (!BinaryData_SetMinSize(_binaryData, _minCapacity)) {
		if (_mallocVar) {
			free(_binaryData);
		}
		return NULL; // minimum size requrement didn't met
	}
	return _binaryData; // initialization sucessful
}
