/*
 * @File: KeyMap.h
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

#ifndef KEYMAP_H
#define KEYMAP_H

#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#define _KEYMAP_DEFAULT_INITIALCOUNT 30
#define _KEYMAP_DEFAULT_EXPANSIONRATE 0.5

typedef struct {
    uint8_t* key;
    uint8_t* data;
    size_t keyMaxSize;
    size_t keySize;
    size_t dataMaxSize;
    size_t dataSize;
} keydata_t;

typedef struct {
    keydata_t* map;         // array buffer
    size_t maxElementCount; // max number of elements
    size_t elementCount;    // how much elements is currently valid in the keymap
	float expansionRate;    // how much elements is additionally added everytime we expand
} keymap_t;

bool KeyData_SetMinDataSize(keydata_t* const _keyData, const size_t _minSize);
bool KeyMap_SetMinElements(keymap_t* const _keyMap, const size_t _minCount);
bool KeyMap_ReserveElements(keymap_t* const _keyMap, const size_t _reservedElementCount);
void KeyMap_DestroyAllKeys(keymap_t* const _keyMap);
void KeyMap_DestroyKey(keymap_t* const _keyMap, const uint8_t* restrict const _key, const size_t _keySize);
void KeyMap_DestroyMap(keymap_t* const _keyMap);
void KeyMap_Destroy(keymap_t* _keyMap);
void KeyMap_DeleteAllKeys(keymap_t* const _keyMap);
bool KeyMap_DeleteKey(keymap_t* const _keyMap, const uint8_t* restrict const _key, const size_t _keySize);
keydata_t* KeyMap_Push(keymap_t* const _keyMap, const uint8_t* restrict const _key, const size_t _keySize, const uint8_t* restrict const _data, const size_t _dataSize);
keydata_t* KeyMap_Set(keymap_t* const _keyMap, const uint8_t* restrict const _key, const size_t _keySize, const void* const _data, const size_t _dataSize);
keydata_t* KeyMap_GetElementWithKey(keymap_t* const _keyMap, const uint8_t* restrict const _key, const size_t _keySize);
keymap_t* KeyMap_InitWithMinSize(keymap_t* _keyMap, const size_t _minCount, const float _expansionRate);

#define KeyMap_Init(_keyMap) KeyMap_InitWithMinSize(_keyMap, _KEYMAP_DEFAULT_INITIALCOUNT, _KEYMAP_DEFAULT_EXPANSIONRATE)

#endif
