/*
 * @File: Dictionary.c
 * @Author: Aldrin John O. Manalansan (ajom)
 * @Email: aldrinjohnolaermanalansan@gmail.com
 * @Brief: Dynamically construct a dictionary of key-value pairs
 * @LastUpdate: June 16, 2025
 * 
 * Copyright (C) 2025  Aldrin John O. Manalansan  <aldrinjohnolaermanalansan@gmail.com>
 * 
 * This Source Code is served under Open-Source AJOM License
 * You should have received a copy of License_OS-AJOM
 * along with this source code. If not, see:
 * <https://raw.githubusercontent.com/Aldrin-John-Olaer-Manalansan/AJOM_License/refs/heads/main/LICENSE_AJOM-OS>
 */

#include "Dictionary.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* checks which memory block has higher value
 * the blocks are treated as little endian big numbers
 * where block_N[0] is the least significant byte
 * where block_N[n - 1] is the most significant byte
 * returns -1 if block_A <  block_B
 * returns  0 if block_A == block_B
 * returns  1 if block_A >  block_B
 */
static int8_t CompareMemoryBlocks(
    const uint8_t* const _block_A, const size_t _blockSize_A,
    const uint8_t* const _block_B, const size_t _blockSize_B
) {
	for (size_t _i = (_blockSize_A > _blockSize_B) ? _blockSize_A : _blockSize_B; _i--;) { 
		uint8_t _value_A = (_blockSize_A <= _i) ? 0 : _block_A[_i];
		uint8_t _value_B = (_blockSize_B <= _i) ? 0 : _block_B[_i];
		if (_value_A < _value_B) return -1;
		if (_value_A > _value_B) return  1;
	}
	return 0;
}

/* performs a binarysearch algorithm to traverse the dictionary
 * returns 0 if the key is found, pickedIndex = this key's index
 * if the key was not found, pickedIndex is in between the two organized key values
 * returns -1 if _key < _object->entries[*pickedIndex].key
 * returns  1 if _key > _object->entries[*pickedIndex].key
 */
static int8_t Dictionary_PickEntryIndex(
	const dictionary_t* restrict const _object,
	const void* restrict const _key, const size_t _keySize,
	size_t* restrict const _pickedIndex
) {
    size_t _right = _object->elementCount;
	if (!_right) { // no elements to traverse
		*_pickedIndex = 0;
		return -1;
	}
	size_t _left = 0;
	int8_t compareResult = -1;
    while (_left < _right) {
        *_pickedIndex = _left + ((_right - _left) >> 1); // avoid overflow
		compareResult = CompareMemoryBlocks(_key, _keySize, _object->entries[*_pickedIndex].key, _object->entries[*_pickedIndex].keySize);
        if (compareResult < 0) { // key is at the left side
            _right = *_pickedIndex;
        } else if (compareResult > 0) { // key is at the right side
            _left = *_pickedIndex + 1;
        } else {
            return 0; // key was found
        }
    }
	// if not found, pickedIndex is between existingkey values:
	// where existingkey[mid] <= key <= existingkey[mid + 1]
    return compareResult;
}

// assures the minimum size of the dictionary's buffer. Expanding its memory size if necessary
bool Dictionary_SetMinElements(dictionary_t* const _object, size_t _minCount) {
	if (_object->maxElementCount >= _minCount) { // buffer's current size already satisfied our size requirement
		return true;
	}
	_minCount *= _object->expansionRate;
	dictionary_entry_t *_expandedStorage = realloc(_object->entries, _minCount * sizeof(dictionary_entry_t));
	if (!_expandedStorage) {
		return false; // failed expanding our buffer's size, therefore initialization requirement wasn't met
	}
	memset(&_expandedStorage[_object->maxElementCount], 0, (_minCount - _object->maxElementCount) * sizeof(dictionary_entry_t)); // fill added memory with zeros
	_object->entries = _expandedStorage;
	_object->maxElementCount = _minCount;
	return true;
}

// assures that the dictionary's buffer has enough unused elements. Expanding its memory size if necessary
bool Dictionary_ReserveElements(dictionary_t* const _object, const size_t _reservedElementCount) {
	if ((_object->maxElementCount < _object->elementCount + _reservedElementCount) // requires expansion
	&& !Dictionary_SetMinElements(_object, _object->maxElementCount + _reservedElementCount)) {
		return false; // failed expanding buffer
	}
	return true;
}

// Frees the element from the dictionary together with its allocated key and data 
void Dictionary_Free_Entry(dictionary_t* restrict const _object, const void* restrict const _key, const size_t _keySize) {
	for (size_t _elementIndex = 0; _elementIndex < _object->elementCount; _elementIndex++) {
		dictionary_entry_t* const _entry = &_object->entries[_elementIndex];
		if ((_entry->keySize != _keySize) || memcmp(_entry->key, _key, _keySize)) {
			continue;
		}
		if (_entry->key) { // has allocated key buffer
			free(_entry->key);
		}
		if (_entry->data) { // has allocated data buffer
			free(_entry->data);
		}
		_object->elementCount--; // decrease element count by 1
		size_t shiftedElementCount = _object->elementCount - _elementIndex;
		if (shiftedElementCount) { // there are elements that needs to be shifted leftwards
			memmove(_entry, _entry + 1, shiftedElementCount * sizeof(dictionary_entry_t));
		} else {
			_entry->key = NULL;
			_entry->data = NULL;
		}
	}
}

// Frees all the elements of the dictionary together with their allocated key and data 
void Dictionary_Free_AllEntries(dictionary_t* const _object) {
	// we use maxelementcount so that we free unused allocated entries as well
	const dictionary_entry_t* const _outOfBoundsPtr = _object->entries + _object->maxElementCount;
	for (dictionary_entry_t* _entry = _object->entries; _entry < _outOfBoundsPtr; _entry++) {
		if (_entry->key) { // has allocated key buffer
			free(_entry->key);
			_entry->key = NULL;
		}
		if (_entry->data) { // has allocated data buffer
			free(_entry->data);
			_entry->data = NULL;
		}
	}
	_object->elementCount = 0;
}

// Frees the Dictionary's Storage
// Since this dictionary's storage has been freed, it must be re-initialized again before reusing it. 
void Dictionary_Free_Storage(dictionary_t* const _object) {
	if (_object->entries) { // has allocated buffer
		Dictionary_Free_AllEntries(_object);
		free(_object->entries);
		_object->entries = NULL;
	}
}

/* Frees a Dictionary object
 * CAUTION! Do not pass pointer to a permanent Dictionary variable!
 */
void Dictionary_Free(dictionary_t* _object) {
	if (_object->entries) { // has allocated buffer
		Dictionary_Free_AllEntries(_object);
		free((void*)_object->entries);
	}
	free((void*)_object);
}

// Removes all the elements of the dictionary together with their allocated key and data
// The allocated key and data aren't freed from memory but will be reused by a newer key and its data
void Dictionary_DeleteAllKeys(dictionary_t* const _object) {
	_object->elementCount = 0;
}

// Removes the element from the dictionary
// The allocated key and data aren't freed from memory but will be reused by a newer key and its data
bool Dictionary_DeleteKey(dictionary_t* restrict const _object, const void* restrict const _key, const size_t _keySize) {
	for (size_t _elementIndex = 0; _elementIndex < _object->elementCount; _elementIndex++) {
		dictionary_entry_t* const _entry = &_object->entries[_elementIndex];
		if ((_entry->keySize != _keySize) || memcmp(_entry->key, _key, _keySize)) {
			continue;
		}
		size_t shiftedElementCount = _object->elementCount - 1 - _elementIndex;
		if (shiftedElementCount) { // there are elements that needs to be shifted leftwards
			dictionary_entry_t reservedEntry = *_entry;
			memmove(_entry, _entry + 1, shiftedElementCount * sizeof(dictionary_entry_t));
			_entry[shiftedElementCount] = reservedEntry; // put at the back of the new last element (reserved for new key data's in the future)
		}
		_object->elementCount--; // decrease element count by 1
		return true; // key has been deleted successfully
	}
	return false; // key not found
}

/* Sets the data of a key
 * The key is created if it doesn't exist
 * _data = 0x0 = NULL will fill the data block with zeroes
 * _data = 0x1 will not touch the data block with zeroes, helpful when the storage element's data was previously a pointer to an allocated memory 
 * Returns a pointer to the data associated with the key in the dictionary
 * returns NULL if a the entry was not created due to a problem
 */
void* Dictionary_Set(
	dictionary_t* restrict const _object,
	const void* restrict const _key, const size_t _keySize,
	const void* restrict const _data, const size_t _dataSize
) {
	size_t _elementIndex;
	dictionary_entry_t* _entry;
	int8_t _comparedResult = Dictionary_PickEntryIndex(_object,  _key, _keySize, &_elementIndex);
	if (_comparedResult == 0) { // key already exists in the list
		_entry = &_object->entries[_elementIndex];
	} else {
		if (!Dictionary_ReserveElements(_object, 1)) {
			return NULL; // insufficient memory
		}

		if (_comparedResult == 1) { // must be created at the end
			_elementIndex++; // we will just push the data at the end
		}

		_entry = &_object->entries[_elementIndex];
		const size_t _shiftedCount = _object->elementCount - _elementIndex;
		if (_shiftedCount) { // some element needed to be shifted to the right
			 // rotate right from target index to last index (shifts to the right, while last index's contents(popped) becomes target index's contents)
			dictionary_entry_t unusedEntry = _object->entries[_object->elementCount];
			memmove(_entry + 1, _entry, sizeof(dictionary_entry_t) * _shiftedCount);
			*_entry = unusedEntry;
		}
		
		// initialize key
		if (!_entry->key) { // buffer is uninitialized
			_entry->key = malloc(_keySize + 1); // +1 for string null terminator compatibility
			if (!_entry->key) {
				return NULL; // failed allocating memory to our buffer
			}
			_entry->keyMaxSize = _keySize;
		} else if (_entry->keyMaxSize < _keySize) { // size of the unused allocated memory is not enough
			void* _expandedBuffer = realloc(_entry->key, _keySize + 1); // +1 for string null terminator compatibility
			if (!_expandedBuffer) {
				return NULL; // failed expanding our buffer's size, therefore, size requirement wasn't met
			}
			_entry->key = _expandedBuffer;
			_entry->keyMaxSize = _keySize;
		}
		memcpy(_entry->key, _key, _keySize);
		_entry->key[_keySize] = 0; // null terminator for string compatibility
		_entry->keySize = _keySize;
		//

		// initialize data
		if (!_entry->data) { // buffer is uninitialized
			_entry->data = calloc(_dataSize + 1, 1); // initially fill the allocated memory with zeroes // +1 for string null terminator compatibility
			if (!_entry->data) {
				return NULL; // failed allocating memory to our buffer
			}
			_entry->dataMaxSize = _dataSize;
		}
		//

		_object->elementCount++;
	}
	
	// assures the minimum size of the entry's data buffer. Expanding its memory size if necessary
	if (_entry->dataMaxSize < _dataSize) { // data requires expansion
		uint8_t* _expandedBuffer = realloc(_entry->data, _dataSize + 1); // +1 for string null terminator compatibility
		if (!_expandedBuffer) {
			return NULL; // failed expanding our buffer's size, therefore, size requirement wasn't met
		}
		memset(_expandedBuffer + _entry->dataMaxSize, 0, _dataSize + 1 - _entry->dataMaxSize); // fill the added memory with zeroes including the null terminator
		_entry->data = _expandedBuffer;
		_entry->dataMaxSize = _dataSize;
	}

	switch ((uintptr_t)_data) {
	case 0:
		memset(_entry->data, 0, _dataSize + 1); // +1 includes the null terminator for string compatibility
	break; case 1:
	break; default:
		memcpy(_entry->data, _data, _dataSize);
		_entry->data[_dataSize] = 0; // null terminator for string compatibility
	break;
	}
	_entry->dataSize = _dataSize;

	return _entry->data;
}

/* Searches the key inside the dictionary
 * if key is found, returns a pointer to the entry structure containing the key-value pair in the dictionary.
 * Returns NULL pointer if the Dictionary has no element with key
 * WARNING: This function is directly accessing a memory region that dynamically changes upon Adding/Deleting Entries
 * Not recommended to be used unless you know what you're doing
 */
dictionary_entry_t* Dictionary_Get_Entry(const dictionary_t* const _object, const void* const _key, const size_t _keySize) {
	size_t _elementIndex;
	return Dictionary_PickEntryIndex(_object,  _key, _keySize, &_elementIndex) ? NULL : &_object->entries[_elementIndex];
}

/* Searches the key inside the dictionary
 * if key is found, returns a pointer to data associated with the key in the dictionary. out_DataSize contains the size of the data
 * out_DataSize = NULL is allowed
 * Returns NULL pointer if the Dictionary has no element with key
 */
void* Dictionary_Get(
	const dictionary_t* restrict const _object,
	const void* restrict const _key, const size_t _keySize,
	size_t* restrict const out_DataSize
) {
	const dictionary_entry_t* const _entry = Dictionary_Get_Entry(_object, _key, _keySize);
	if (!_entry) {
		return NULL; // entry not found
	}
	if (out_DataSize) {
		*out_DataSize = _entry->dataSize;
	}
	return _entry->data;
}

// Checks if the dictionary has a key
bool Dictionary_Has_Key(const dictionary_t* const _object, const void* const _key, const size_t _keySize) {
	return Dictionary_Get_Entry(_object, _key, _keySize);
}

// Checks if the dictionary has a key with data
bool Dictionary_Has_Data(const dictionary_t* const _object, const void* const _data, const size_t _dataSize) {
	const dictionary_entry_t* const outofBoundsPtr = _object->entries + _object->elementCount;
	for (const dictionary_entry_t* _entry = _object->entries; _entry < outofBoundsPtr; _entry++) {
		if ((_dataSize == _entry->dataSize) && !memcmp(_data, _entry->data, _dataSize)) {
			return true;
		}
	}
	return false;
}

// Merges the source's contents with the destination's contents
// when overWriteValues = true, the source's value is used on existing destination keys
// when overWriteValues = false, the existing destination keys are not overwritten
bool Dictionary_Merge(dictionary_t* restrict _destination, const dictionary_t* restrict const _source, const bool overWriteValues) {
	const dictionary_entry_t* const outofBoundsPtr = _source->entries + _source->elementCount;
	for (const dictionary_entry_t* sourceEntry = _source->entries; sourceEntry < outofBoundsPtr; sourceEntry++) {
		if ((overWriteValues || !Dictionary_Get_Entry(_destination, sourceEntry->key, sourceEntry->keySize)) // entry can be added/modified
		&& !Dictionary_Set(_destination, sourceEntry->key, sourceEntry->keySize, sourceEntry->data, sourceEntry->dataSize)) {
			return false; // failed to add/modify entry
		}
	}
	return true; // merge successful
}

// Copies of the source's contents to the destination
// set _destination = NULL to create a new dictionary object
dictionary_t* Dictionary_Clone(dictionary_t* restrict _destination, const dictionary_t* restrict const _source) {
	_destination = Dictionary_InitWithMinSize(_destination, _source->maxElementCount, _source->expansionRate - 1.0);
	if (!_destination) {
		return NULL;
	}
	
	for (size_t i = 0; i < _source->elementCount; i++) {
		const dictionary_entry_t* const _entry = &_source->entries[i];
		Dictionary_Set(_destination, _entry->key, _entry->keySize, _entry->data, _entry->dataSize);
	}

	return _destination;
}

/* Properly initializes the Dictionary variable.
 * Allocates memory to the Dictionary variable if its current value is NULL
 * Reallocates memory to the Dictionary's buffer that satisfy the required minimum size
 */
dictionary_t* Dictionary_InitWithMinSize(dictionary_t* _object, const size_t _minCount, const float _expansionRate) {
	bool _mallocVar;
	if (!_object) { // if we are requesting to initialize it
		_object = malloc(sizeof(dictionary_t));
		if (!_object) {
			return NULL; // failed allocating dictionary variable
		}
		_object->entries = NULL; // indicate buffer requires initialization later
		_mallocVar = true;
	} else {
		_mallocVar = false;
	}
	_object->expansionRate = _expansionRate + 1.0;
	if (!_object->entries) { // buffer isn't initialized yet
		_object->entries = calloc(_minCount, sizeof(dictionary_entry_t)); // make sure to pad the entire memory with zeros
		if (!_object->entries) {
			if (_mallocVar) {
				free(_object);
			}
			return NULL; // failed allocating buffer to our dictionary variable
		}
		_object->maxElementCount = _minCount;
	} else if (!Dictionary_SetMinElements(_object, _minCount)) {
		if (_mallocVar) {
			free(_object);
		}
		return NULL; // minimum size requrement didn't met
	}
	_object->elementCount = 0;
	return _object; // initialization sucessful
}
