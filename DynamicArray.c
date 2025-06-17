/*
 * @File: DynamicArray.c
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

#include "DynamicArray.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// assures the minimum size of the dynamicarray's buffer. Expanding its memory size if necessary
bool DynamicArray_SetMinElementsWithSize(dynamicarray_t* const _object, const size_t _minCount, const size_t _elementSize) {
	size_t requiredSize = _minCount * _elementSize;
	if ((_object->maxElementCount * _object->elementSize) >= requiredSize) { // buffer's current size already satisfied our requirement
		return true;
	}
	dynamicarray_t *expandedBuffer = realloc(_object->array, requiredSize);
	if (!expandedBuffer) {
		return false; // failed expanding our buffer's size, therefore initialization requirement wasn't met
	}
	_object->array = expandedBuffer;
	_object->maxElementCount = _minCount;
	_object->elementSize = _elementSize;
	return true;
}

// assures that the dynamicarray's buffer has enough unused elements. Expanding its memory size if necessary
bool DynamicArray_ReserveElements(dynamicarray_t* const _object, const size_t _reservedElementCount) {
	if ((_object->maxElementCount < (_object->elementCount + _reservedElementCount)) // requires expansion
	&& !DynamicArray_SetMinElements(_object, (_object->maxElementCount + _reservedElementCount) * _object->expansionRate)) {
		return false; // failed expanding buffer
	}
	return true;
}

// Frees the DynamicArray's Buffer
void DynamicArray_FreeBuffer(dynamicarray_t* const _object) {
	if (_object->array) { // has allocated buffer
		free(_object->array);
		_object->array = NULL;
	}
}

/* Frees a DynamicArray object
 * CAUTION! Do not pass pointer to a permanent DynamicArray variable!
 */
void DynamicArray_Free(dynamicarray_t* _object) {
	if (_object->array) { // has allocated buffer
		free((void*)_object->array);
	}
	free((void*)_object);
}

// Removes a specific element from the dynamicArray
bool DynamicArray_Delete(dynamicarray_t* const _object, const size_t _deletedElementIndex) {
	if (_deletedElementIndex >= _object->elementCount) {
		return false; // index out of bounds
	}
	_object->elementCount--; // decrease element count by 1
	size_t shiftedElementCount = _object->elementCount - _deletedElementIndex;
	if (shiftedElementCount) { // there are elements that needs to be shifted leftwards
		const uintptr_t basePtr = (uintptr_t)_object->array + (_deletedElementIndex * _object->elementSize);
		memmove(
			(void*)basePtr,
			(const void*)(basePtr + _object->elementSize),
			shiftedElementCount * _object->elementSize
		);
	}
	return true; // element has been deleted successfully
}

/* Inserts the data at the specified element index, shifting all the higher ordered elements by 1
 * The last element index is used if the specified element index is out of bounds.
 * Supports overlapping data
 */
bool DynamicArray_Insert(dynamicarray_t* restrict const _object, const size_t _index, const void* restrict const _value) {
	if (_index >= _object->elementCount) { // index out of bounds
		return DynamicArray_Push(_object, _value);
	}
	if (!DynamicArray_ReserveElements(_object, 2)) { // one slot for the inserted data, and another one slot for the temporary space
		return false; // insufficient memory
	}
	void* const _valuePtr = _object->array + ((_object->elementCount + 1) * _object->elementSize); // pointer to an unused element slot
	void* const _destination = _object->array + (_index * _object->elementSize);
	memcpy(_valuePtr, _value, _object->elementSize); // temporarily store value
	memmove(_destination + _object->elementSize, _destination, (_object->elementCount - _index) * _object->elementSize);
	memcpy(_destination, _valuePtr, _object->elementSize); // store temporary value at the index's value
	_object->elementCount++;
	return true;
}

// removes one element at the end of the stack
bool DynamicArray_Pop(dynamicarray_t* const _object) {
	if (!_object->elementCount) {
		return false; // array already has no elements
	}
	_object->elementCount--; // decrease element count by 1
	return true; // element has been deleted successfully
}

// Pushes a data at the end of the stack
bool DynamicArray_Push(dynamicarray_t* restrict const _object, const void* restrict const _value) {
	if (!DynamicArray_ReserveElements(_object, 1)) {
		return false; // insufficient memory
	}
	memcpy(_object->array + (_object->elementCount * _object->elementSize), _value, _object->elementSize);
	_object->elementCount++;
	return true;
}

// checks if the DynamicArray has an element with value
bool DynamicArray_HasValue(const dynamicarray_t* const _object, const void* const _value) {
	const void* endPtr = _object->array + (_object->elementCount * _object->elementSize);
	for (const void* _elementPtr = _object->array; _elementPtr < endPtr; _elementPtr += _object->elementSize) {
		if (!memcmp(_elementPtr, _value, _object->elementSize)) { // values matched
			return true;
		}
	}
	return false;
}

// checks if the DynamicArray has an element with value and returns the element number that contains that value
// Requirement: 1 <= _startingElementNumber <= _object->elementCount
size_t DynamicArray_GetElementNumberContainingValue(const dynamicarray_t* const _object, size_t _startingElementNumber, const void* const _value) {
	_startingElementNumber--; // from 0 to n-1
	if (_startingElementNumber >= _object->elementCount) {
		return 0; // element index out of bounds
	}
	size_t elementIndex = 1;
	const void* const endPtr = _object->array + (_object->elementCount * _object->elementSize);
	for (const void* _elementPtr = _object->array + (_startingElementNumber * _object->elementSize);
		_elementPtr < endPtr;
		_elementPtr += _object->elementSize
	) {
		if (!memcmp(_elementPtr, _value, _object->elementSize)) { // values matched
			return elementIndex;
		}
		elementIndex++;
	}
	return 0;
}

/* Properly initializes the DynamicArray variable.
 * Allocates memory to the DynamicArray variable if its current value is NULL
 * Reallocates memory to the DynamicArray's buffer that satisfy the required minimum size
 */
dynamicarray_t* DynamicArray_InitAll(dynamicarray_t* _object, const size_t _elementSize, const size_t _minCount, const float _expansionRate) {
	bool _mallocVar;
	if (!_object) { // if we are requesting to initialize it
		_object = malloc(sizeof(dynamicarray_t));
		if (!_object) {
			return NULL; // failed allocating dynamicArray variable
		}
		_object->array = NULL; // indicate buffer requires initialization later
		_mallocVar = true;
	} else {
		_mallocVar = false;
	}
	if (!_object->array) { // buffer isn't initialized yet
		_object->array = malloc(_minCount * _elementSize);
		if (!_object->array) {
			if (_mallocVar) {
				free(_object);
			}
			return NULL; // failed allocating buffer to our dynamicArray variable
		}
		_object->maxElementCount = _minCount;
	} else if (!DynamicArray_SetMinElementsWithSize(_object, _minCount, _elementSize)) {
		if (_mallocVar) {
			free(_object);
		}
		return NULL; // minimum size requrement didn't met
	}
	_object->expansionRate = _expansionRate + 1.0;
	_object->elementSize = _elementSize;
	_object->elementCount = 0;
	return _object; // initialization sucessful
}