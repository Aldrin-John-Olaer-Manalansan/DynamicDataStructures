/*
 * @File: KeyMap.c
 * @Author: Aldrin John O. Manalansan (ajom)
 * @Email: aldrinjohnolaermanalansan@gmail.com
 * @Brief: Dynamically construct an Array Mapped with Keys
 * @LastUpdate: June 7, 2025
 * 
 * Copyright (C) 2025  Aldrin John O. Manalansan  <aldrinjohnolaermanalansan@gmail.com>
 * 
 * This Source Code is served under Open-Source AJOM License
 * You should have received a copy of License_OS-AJOM
 * along with this source code. If not, see:
 * <https://raw.githubusercontent.com/Aldrin-John-Olaer-Manalansan/AJOM_License/refs/heads/main/LICENSE_AJOM-OS>
 */

#include "KeyMap.h"

// assures the minimum size of the keyData's data buffer. Expanding its memory size if necessary
bool KeyData_SetMinDataSize(keydata_t* const _keyData, const size_t _minSize) {
	if (_keyData->dataMaxSize >= _minSize) {
		return true; // size requirement was met
	}
	uint8_t *_expandedBuffer = realloc(_keyData->data, _minSize);
	if (!_expandedBuffer) {
		return NULL; // failed expanding our buffer's size, therefore, size requirement wasn't met
	}
	memset(_expandedBuffer + _keyData->dataMaxSize, 0, _minSize - _keyData->dataMaxSize); // fill the added memory with zeroes
	_keyData->data = _expandedBuffer;
	_keyData->dataMaxSize = _minSize;
	return true;
}

// assures the minimum size of the keymap's buffer. Expanding its memory size if necessary
bool KeyMap_SetMinElements(keymap_t* const _keyMap, size_t _minCount) {
	if (_keyMap->maxElementCount >= _minCount) { // buffer's current size already satisfied our size requirement
		return true;
	}
	_minCount *= _keyMap->expansionRate;
	keydata_t *_expandedMap = realloc(_keyMap->map, _minCount * sizeof(keydata_t));
	if (!_expandedMap) {
		return false; // failed expanding our buffer's size, therefore initialization requirement wasn't met
	}
	memset(&_expandedMap[_keyMap->maxElementCount], 0, (_minCount - _keyMap->maxElementCount) * sizeof(keydata_t)); // fill added memory with zeros
	_keyMap->map = _expandedMap;
	_keyMap->maxElementCount = _minCount;
	return true;
}

// assures that the keymap's buffer has enough unused elements. Expanding its memory size if necessary
bool KeyMap_ReserveElements(keymap_t* const _keyMap, const size_t _reservedElementCount) {
	if ((_keyMap->maxElementCount < _keyMap->elementCount + _reservedElementCount) // requires expansion
	&& !KeyMap_SetMinElements(_keyMap, _keyMap->maxElementCount + _reservedElementCount)) {
		return false; // failed expanding buffer
	}
	return true;
}

// Frees all the elements of the keyMap together with their allocated key and data 
void KeyMap_DestroyAllKeys(keymap_t* const _keyMap) {
	for (size_t _elementIndex = 0; _elementIndex < _keyMap->elementCount; _elementIndex++) {
		keydata_t* const _keyData = &_keyMap->map[_elementIndex];
		if (_keyData->key) { // has allocated key buffer
			free(_keyData->key);
			_keyData->key = NULL;
		}
		if (_keyData->data) { // has allocated data buffer
			free(_keyData->data);
			_keyData->data = NULL;
		}
	}
	_keyMap->elementCount = 0;
}

// Frees the element from the keyMap together with its allocated key and data 
void KeyMap_DestroyKey(keymap_t* const _keyMap, const uint8_t* restrict const _key, const size_t _keySize) {
	for (size_t _elementIndex = 0; _elementIndex < _keyMap->elementCount; _elementIndex++) {
		keydata_t* const _keyData = &_keyMap->map[_elementIndex];
		if ((_keyData->keySize != _keySize) || memcmp(_keyData->key, _key, _keySize)) {
			continue;
		}
		if (_keyData->key) { // has allocated key buffer
			free(_keyData->key);
		}
		if (_keyData->data) { // has allocated data buffer
			free(_keyData->data);
		}
		_keyMap->elementCount--; // decrease element count by 1
		size_t shiftedElementCount = _keyMap->elementCount - _elementIndex;
		if (shiftedElementCount) { // there are elements that needs to be shifted leftwards
			memmove(_keyData, &_keyMap->map[_elementIndex + 1], shiftedElementCount * sizeof(keydata_t));
		} else {
			_keyData->key = NULL;
			_keyData->data = NULL;
		}
	}
}

// Frees the KeyMap's Map
void KeyMap_DestroyMap(keymap_t* const _keyMap) {
	if (_keyMap->map) { // has allocated buffer
		KeyMap_DestroyAllKeys(_keyMap);
		free(_keyMap->map);
		_keyMap->map = NULL;
	}
}

/* Frees a KeyMap object
 * CAUTION! Do not pass pointer to a permanent KeyMap variable!
 */
void KeyMap_Destroy(keymap_t* _keyMap) {
	if (_keyMap->map) { // has allocated buffer
		KeyMap_DestroyAllKeys(_keyMap);
		free((void*)_keyMap->map);
	}
	free((void*)_keyMap);
}

// Removes all the elements of the keyMap together with their allocated key and data
// The allocated key and data aren't freed from memory but will be reused by a newer key and its data
void KeyMap_DeleteAllKeys(keymap_t* const _keyMap) {
	_keyMap->elementCount = 0;
}

// Removes the element from the keyMap
// The allocated key and data aren't freed from memory but will be reused by a newer key and its data
bool KeyMap_DeleteKey(keymap_t* const _keyMap, const uint8_t* restrict const _key, const size_t _keySize) {
	for (size_t _elementIndex = 0; _elementIndex < _keyMap->elementCount; _elementIndex++) {
		keydata_t* const _keyData = &_keyMap->map[_elementIndex];
		if ((_keyData->keySize != _keySize) || memcmp(_keyData->key, _key, _keySize)) {
			continue;
		}
		size_t shiftedElementCount = _keyMap->elementCount - 1 - _elementIndex;
		if (shiftedElementCount) { // there are elements that needs to be shifted leftwards
			keydata_t reservedKeyData = *_keyData;
			memmove(_keyData, &_keyData[1], shiftedElementCount * sizeof(keydata_t));
			_keyData[shiftedElementCount] = reservedKeyData; // put at the back of the new last element (reserved for new key data's in the future)
		}
		_keyMap->elementCount--; // decrease element count by 1
		return true; // key has been deleted successfully
	}
	return false; // key not found
}

/* Pushes a data at the end of the stack as Key
 * _data = 0x0 = NULL will fill the data block with zeroes
 * _data = 0x1 will not touch the data block with zeroes, helpful when the map element's data is was previously a pointer to an allocated memory 
 * Returns a pointer to the appended keyData element from the keyMap
 * returns NULL if a the keyData was not created due to a problem
 * CAUTION: does not check if key exists. Use KeyMap_HasKey function to know if key is currently present.
 * To avoid Duplicate Keys, use KeyMap_Set function
 */
keydata_t* KeyMap_Push(keymap_t* const _keyMap, const uint8_t* restrict const _key, const size_t _keySize, const uint8_t* restrict const _data, const size_t _dataSize) {
	if (!KeyMap_ReserveElements(_keyMap, 1)) {
		return NULL; // insufficient memory
	}
	keydata_t* const _keyData = &_keyMap->map[_keyMap->elementCount];
	if (!_keyData->key) { // buffer is uninitialized
		_keyData->key = malloc(_keySize);
		if (!_keyData->key) {
			return NULL; // failed allocating memory to our buffer
		}
		_keyData->keyMaxSize = _keySize;
	} else if (_keyData->keyMaxSize < _keySize) { // size of the unused allocated memory is not enough
		uint8_t *_expandedBuffer = realloc(_keyData->key, _keySize);
		if (!_expandedBuffer) {
			return NULL; // failed expanding our buffer's size, therefore, size requirement wasn't met
		}
		_keyData->key = _expandedBuffer;
		_keyData->keyMaxSize = _keySize;
	}
	memcpy(_keyData->key, _key, _keySize);
	_keyData->keySize = _keySize;

	if (!_keyData->data) { // buffer is uninitialized
		_keyData->data = calloc(_dataSize, 1); // initially fill the allocated memory with zeroes
		if (!_keyData->data) {
			return NULL; // failed allocating memory to our buffer
		}
		_keyData->dataMaxSize = _dataSize;
	} else if (!KeyData_SetMinDataSize(_keyData, _dataSize)) {
		return NULL; // failed expanding memory
	}
	switch ((uintptr_t)_data) {
		case 0:
			memset(_keyData->data, 0, _dataSize);
		break;
		case 1:
		break;
		default:
			memcpy(_keyData->data, _data, _dataSize);
		break;
	}
	_keyData->dataSize = _dataSize;

	_keyMap->elementCount++;
	return _keyData;
}

/* Sets the data of a key
 * _data = 0x0 = NULL will fill the data block with zeroes
 * _data = 0x1 will not touch the data block with zeroes, helpful when the map element's data is was previously a pointer to an allocated memory 
 * Returns a pointer to the appended keyData element from the keyMap
 * returns NULL if a the keyData was not created due to a problem
 * The key is created if it doesn't exist, but wastes more CPU cycles than KeyMap_Push function
 * if you know the key doesn't exists, use KeyMap_Push function
 */
keydata_t* KeyMap_Set(keymap_t* const _keyMap, const uint8_t* restrict const _key, const size_t _keySize, const void* const _data, const size_t _dataSize) {
	for (size_t _elementIndex = 0; _elementIndex < _keyMap->elementCount; _elementIndex++) {
		keydata_t* const _keyData = &_keyMap->map[_elementIndex];
		if ((_keyData->keySize != _keySize) || memcmp(_keyData->key, _key, _keySize)) {
			continue;
		}
		if (!KeyData_SetMinDataSize(_keyData, _dataSize)) {
			return NULL; // failed expanding memory
		}
		switch ((uintptr_t)_data) {
			case 0:
				memset(_keyData->data, 0, _dataSize);
			break;
			case 1:
			break;
			default:
				memmove(_keyData->data, _data, _dataSize);
			break;
		}
		_keyData->dataSize = _dataSize;
		return _keyData; // set operation successful
	}
	return KeyMap_Push(_keyMap, _key, _keySize, _data, _dataSize);
}

/* Returns a pointer to the keyData element with key from the keyMap 
 * Returns NULL pointer if the KeyMap has no element with key
 */
keydata_t* KeyMap_GetElementWithKey(keymap_t* const _keyMap, const uint8_t* restrict const _key, const size_t _keySize) {
	for (size_t _elementIndex = 0; _elementIndex < _keyMap->elementCount; _elementIndex++) {
		keydata_t* keyData = &_keyMap->map[_elementIndex];
		if ((keyData->keySize == _keySize) && !memcmp(keyData->key, _key, _keySize)) {
			return keyData;
		}
	}
	return NULL;
}

/* Properly initializes the KeyMap variable.
 * Allocates memory to the KeyMap variable if its current value is NULL
 * Reallocates memory to the KeyMap's buffer that satisfy the required minimum size
 */
keymap_t* KeyMap_InitWithMinSize(keymap_t* _keyMap, const size_t _minCount, const float _expansionRate) {
	bool _mallocVar;
	if (!_keyMap) { // if we are requesting to initialize it
		_keyMap = malloc(sizeof(keymap_t));
		if (!_keyMap) { 
			return NULL; // failed allocating keyMap variable
		}
		_keyMap->map = NULL; // indicate buffer requires initialization later
		_mallocVar = true;
	} else {
		_mallocVar = false;
	}
	_keyMap->expansionRate = _expansionRate + 1.0;
	if (!_keyMap->map) { // buffer isn't initialized yet
		_keyMap->map = calloc(_minCount, sizeof(keydata_t)); // make sure to pad the entire memory with zeros
		if (!_keyMap->map) {
			if (_mallocVar) {
				free(_keyMap);
			}
			return NULL; // failed allocating buffer to our keyMap variable
		}
		_keyMap->maxElementCount = _minCount;
	} else if (!KeyMap_SetMinElements(_keyMap, _minCount)) {
		if (_mallocVar) {
			free(_keyMap);
		}
		return NULL; // minimum size requrement didn't met
	}
	_keyMap->elementCount = 0;
	return _keyMap; // initialization sucessful
}
