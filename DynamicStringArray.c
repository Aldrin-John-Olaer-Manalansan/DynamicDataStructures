/*
 * @File: DynamicStringArray.c
 * @Author: Aldrin John O. Manalansan (ajom)
 * @Email: aldrinjohnolaermanalansan@gmail.com
 * @Brief: Dynamically construct String Arrays with arbitrary size
 * @LastUpdate: June 7, 2025
 * 
 * Copyright (C) 2025  Aldrin John O. Manalansan  <aldrinjohnolaermanalansan@gmail.com>
 * 
 * This Source Code is served under Open-Source AJOM License
 * You should have received a copy of License_OS-AJOM
 * along with this source code. If not, see:
 * <https://raw.githubusercontent.com/Aldrin-John-Olaer-Manalansan/AJOM_License/refs/heads/main/LICENSE_AJOM-OS>
 */

#include "DynamicStringArray.h"

// assures the minimum elements of the DynamicStringArray. Expanding its memory size if necessary
bool DynamicStringArray_SetMinElements(dynamicstringarray_t* const _object, size_t _minElementCount) {
	if (_object->maxElementCount >= _minElementCount) {
		return true; // buffer's current max element already satisfied our requirement
	}
	_minElementCount *= _object->expansionRate;
	void* const expanded = realloc(_object->array, _minElementCount * sizeof(_object->array));
	if (!expanded) {
		return false; // failed expanding our array's size, therefore initialization requirement wasn't met
	}
	_object->array = expanded;
	_object->maxElementCount = _minElementCount;
	return true;
}

// assures the minimum size of the DynamicStringArray's buffer. Expanding its memory size if necessary
bool DynamicStringArray_SetMinBufferSize(dynamicstringarray_t* const _object, size_t _minBufferSize) {
	if (_object->bufferSize >= _minBufferSize) {
		return true; // buffer's current max element already satisfied our requirement
	}
	_minBufferSize *= _object->expansionRate;
	void* const expanded = realloc(_object->buffer, _minBufferSize);
	if (!expanded) {
		return false; // failed expanding our buffer's size, therefore initialization requirement wasn't met
	}
    const ptrdiff_t relocateOffset = (ptrdiff_t)expanded - (ptrdiff_t)_object->buffer;
	_object->buffer = expanded;
	_object->bufferSize = _minBufferSize;
    // relocate array of pointers as well
    for (size_t i = 0; i < _object->elementCount; i++) {
        _object->array[i] += relocateOffset;
    }
	return true;
}

// assures that the DynamicStringArray has enough unused elements. Expanding its memory size if necessary
bool DynamicStringArray_ReserveElements(dynamicstringarray_t* const _object, const size_t _reservedElementCount) {
	if (((_object->maxElementCount - _object->elementCount) < _reservedElementCount) // requires expansion
	&& !DynamicStringArray_SetMinElements(_object, _object->maxElementCount + _reservedElementCount)) {
		return false; // failed expanding buffer
	}
	return true;
}

// assures that the DynamicStringArray has enough unused elements. Expanding its memory size if necessary
bool DynamicStringArray_ReserveBufferSize(dynamicstringarray_t* const _object, const size_t _reservedBufferSize) {
	if (((_object->bufferSize - _object->usedSize) < _reservedBufferSize) // requires expansion
	&& !DynamicStringArray_SetMinBufferSize(_object, _object->bufferSize + _reservedBufferSize)) {
		return false; // failed expanding buffer
	}
	return true;
}

// Frees the DynamicStringArray's Array and Buffer memories
void DynamicStringArray_FreeStorage(dynamicstringarray_t* const _object) {
	if (_object->array) { // has allocated array
		free(_object->array);
		_object->array = NULL;
	}
	if (_object->buffer) { // has allocated buffer
		free(_object->buffer);
		_object->buffer = NULL;
	}
}

/* Frees a DynamicStringArray object
 * CAUTION! Do not pass pointer to a permanent DynamicStringArray variable!
 */
void DynamicStringArray_Free(dynamicstringarray_t* _object) {
	if (_object->array) { // has allocated array
		free((void*)_object->array);
	}
	if (_object->array) { // has allocated buffer
		free((void*)_object->buffer);
	}
	free((void*)_object);
}

/*
 * Clears all the elements of the DynamicStringArray Object, making it look empty
 * The allocated elements are not actually freed from memory but are instead reused later by new elements
 */
void DynamicStringArray_Clear(dynamicstringarray_t* _object) {
	_object->elementCount = 0;
	_object->usedSize = 0;
}

// appends one element containing the string at the end of the stack
bool DynamicStringArray_PushSubString(dynamicstringarray_t* restrict const _object, const char* restrict const _string, size_t _length) {
	if (!_length || !_string[0] // first character is not a null terminator
	|| !DynamicStringArray_ReserveElements(_object, 1) // insufficient memory
	) {
		return false;
	}
	ptrdiff_t _actualLength = (ptrdiff_t)memchr(_string, 0, _length);
	if (_actualLength) { // null terminator is present between the entire substring's length
		_actualLength -= (ptrdiff_t)_string;
		if (_length > (size_t)_actualLength) {
			_length = _actualLength;
		}
	}
    size_t _size = _length + 1;
	if (!DynamicStringArray_ReserveBufferSize(_object, _size)) {
		return false; // insufficient memory
	}

    char* const insertedString = _object->buffer + _object->usedSize;
	memcpy(insertedString, _string, _length);
	insertedString[_length] = 0; // null terminator
	_object->usedSize += _size;
    _object->array[_object->elementCount] = insertedString;
	_object->elementCount++;
	return true;
}

// removes one element and its string content at the end of the stack
bool DynamicStringArray_Pop(dynamicstringarray_t* const _object) {
	if (!_object->elementCount) {
		return false; // array already has no elements
	}
	_object->elementCount--; // decrease element count by 1
	_object->usedSize -= strlen(_object->array[_object->elementCount]) + 1; // decrease used size at buffer
	return true; // element has been deleted successfully
}

// Inserts the data at the specified element index, shifting all the higher ordered elements by 1
// The passed index is required be in between the existing first element and the last element.
// Use DynamicStringArray_PushSubString if you wish to insert at the end of the array
// Does not support overlapping data
bool DynamicStringArray_InsertSubString(
	dynamicstringarray_t* restrict const _object, const size_t _index,
	const char* restrict const _string, size_t _length
) {
	if (!_length || !_string[0] // first character is not a null terminator
	|| (_index >= _object->elementCount) // index out of bounds
	|| !DynamicStringArray_ReserveElements(_object, 1) // insufficient memory
	) {
		return false;
	}
	ptrdiff_t _actualLength = (ptrdiff_t)memchr(_string, 0, _length);
	if (_actualLength) { // null terminator is present between the entire substring's length
		_actualLength -= (ptrdiff_t)_string;
		if (_length > (size_t)_actualLength) {
			_length = _actualLength;
		}
	}
    const size_t _size = _length + 1;
	if (!DynamicStringArray_ReserveBufferSize(_object, _size)) {
		return false; // insufficient memory
	}
    
    char* const insertedString = (char*)_object->array[_index];
    size_t shiftedSize = _object->usedSize + (size_t)_object->buffer - (size_t)insertedString;
    if (shiftedSize) { // there are parts that needs to be shifted rightwards
        memmove(insertedString + _size, insertedString, shiftedSize);
    }
	memcpy(insertedString, _string, _length);
	insertedString[_length] = 0; // null terminator
    _object->usedSize += _size;
    
    shiftedSize = _object->elementCount - _index;
    _object->elementCount++;
    if (shiftedSize) { // there are parts that needs to be shifted rightwards
        memmove(&_object->array[_index + 1], &_object->array[_index], shiftedSize * sizeof(_object->array));

        // relocate contents of array of pointers as well
        for (size_t i = _index + 1; i < _object->elementCount; i++) {
            _object->array[i] += _size;
        }
    }
	_object->array[_index] = insertedString;

	return true;
}

// Removes a specific element from the dynamicStringArray
bool DynamicStringArray_Delete(dynamicstringarray_t* const _object, const size_t _deletedElementIndex) {
	if (_deletedElementIndex >= _object->elementCount) {
		return false; // index out of bounds
	}
    const size_t bytesDeleted = strlen(_object->array[_deletedElementIndex]) + 1;
	_object->elementCount--; // decrease element count by 1
	size_t shiftedElementCount = _object->elementCount - _deletedElementIndex;
	if (shiftedElementCount) { // there are elements that needs to be shifted leftwards
		memmove(
            _object->array[_deletedElementIndex],
            _object->array[_deletedElementIndex + 1],
            _object->buffer + _object->usedSize - _object->array[_deletedElementIndex + 1]
        ); // shift the contents of the buffer
		memmove(
            &_object->array[_deletedElementIndex],
            &_object->array[_deletedElementIndex + 1],
            shiftedElementCount * sizeof(_object->array)
        ); // shift the array
        // relocate contents of array of pointers as well
        for (size_t i = _deletedElementIndex; i < _object->elementCount; i++) {
            _object->array[i] -= bytesDeleted;
        }
	}
	_object->usedSize -= bytesDeleted; // decrease used size at buffer
	return true; // element has been deleted successfully
}

/* Searches a string inside the DynamicStringArray variable
 * Returns the index of the matching element if the string was found
 * Returns -1 if the string was not found
 */
size_t DynamicStringArray_Search(const dynamicstringarray_t* restrict const _object, const char* restrict const _seachedString, const bool _isCaseSensitive) {
    for (size_t i = 0; i < _object->elementCount; i++) {
        if (_isCaseSensitive
            ? !strcmp(_seachedString, _object->array[i])
            : !stricmp(_seachedString, _object->array[i])
        ) {
            return i;
        }
    }
    return -1;
}

/* Properly initializes the DynamicStringArray variable.
 * Allocates memory to the DynamicStringArray variable if its current value is NULL
 * Reallocates memory to the DynamicStringArray's buffer that satisfy the required minimum size
 */
dynamicstringarray_t* DynamicStringArray_InitAll(dynamicstringarray_t* _object, const size_t _minElementCount, const size_t _minBufferSize, const float _expansionRate) {
	bool _mallocVar;
	if (!_object) { // if we are requesting to initialize it
		_object = malloc(sizeof(dynamicstringarray_t));
		if (!_object) {
			return NULL; // failed allocating memory to our object
		}
		_object->array = NULL; // indicate array requires initialization later
		_object->buffer = NULL; // indicate buffer requires initialization later
		_mallocVar = true;
	} else {
		_mallocVar = false;
	}
	_object->expansionRate = _expansionRate + 1.0;

	if (!_object->array) { // array isn't initialized yet
		_object->array = malloc(_minElementCount * sizeof(_object->array));
		if (!_object->array) {
			if (_mallocVar) {
				free(_object);
			}
			return NULL; // failed allocating array to our object
		}
		_object->maxElementCount = _minElementCount;
	} else if (!DynamicStringArray_SetMinElements(_object, _minElementCount)) {
		if (_mallocVar) {
			free(_object);
		}
		return NULL; // minimum size requrement didn't met
	}

	if (!_object->buffer) { // buffer isn't initialized yet
		_object->buffer = malloc(_minBufferSize);
		if (!_object->buffer) {
			if (_mallocVar) {
				free(_object);
			}
			return NULL; // failed allocating buffer to our object
		}
		_object->bufferSize = _minBufferSize;
	} else if (!DynamicStringArray_SetMinBufferSize(_object, _minBufferSize)) {
		if (_mallocVar) {
			free(_object);
		}
		return NULL; // minimum size requrement didn't met
	}

	_object->elementCount = 0;
	_object->usedSize = 0;
	return _object; // initialization sucessful
}
