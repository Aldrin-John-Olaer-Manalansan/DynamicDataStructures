/*
 * @File: Dictionary.h
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

#pragma once

#include <stdint.h>
#include <stdbool.h>

// linux compatibility
#include <stddef.h>
#include <limits.h>

#define DICTIONARY_DEFAULT_INITIALCOUNT 30
#define DICTIONARY_DEFAULT_EXPANSIONRATE 0.5

typedef struct {
    uint8_t* key;
    uint8_t* data;
    size_t keyMaxSize;
    size_t dataMaxSize;
    size_t keySize;
    size_t dataSize;
} dictionary_entry_t;

typedef struct {
    dictionary_entry_t* entries;
    size_t elementCount;    // how much elements is currently valid in the dictionary
    size_t maxElementCount; // max number of elements
	float expansionRate;    // how much elements is additionally added everytime we expand
} dictionary_t;

bool Dictionary_SetMinElements(dictionary_t* const _object, const size_t _minCount);
bool Dictionary_ReserveElements(dictionary_t* const _object, const size_t _reservedElementCount);
void Dictionary_Free_Entry(dictionary_t* restrict const _object, const void* restrict const _key, const size_t _keySize);
void Dictionary_Free_AllEntries(dictionary_t* const _object);
void Dictionary_Free_Storage(dictionary_t* const _object);
void Dictionary_Free(dictionary_t* _object);
void Dictionary_DeleteAllKeys(dictionary_t* const _object);
bool Dictionary_DeleteKey(dictionary_t* restrict const _object, const void* restrict const _key, const size_t _keySize);
void* Dictionary_Set(
	dictionary_t* restrict const _object,
	const void* restrict const _key, const size_t _keySize,
	const void* restrict const _data, const size_t _dataSize
);
dictionary_entry_t* Dictionary_Get_Entry(const dictionary_t* const _object, const void* const _key, const size_t _keySize);
void* Dictionary_Get(
	const dictionary_t* restrict const _object,
	const void* restrict const _key, const size_t _keySize,
	size_t* restrict const out_DataSize
);
bool Dictionary_Has_Key(const dictionary_t* const _object, const void* const _key, const size_t _keySize);
bool Dictionary_Has_Data(const dictionary_t* const _object, const void* const _data, const size_t _dataSize);
bool Dictionary_Merge(dictionary_t* restrict _destination, const dictionary_t* restrict const _source, const bool overWriteValues);
dictionary_t* Dictionary_Clone(dictionary_t* restrict _destination, const dictionary_t* restrict const _source);
dictionary_t* Dictionary_InitWithMinSize(dictionary_t* _object, const size_t _minCount, const float _expansionRate);

#define Dictionary_Init(_object) Dictionary_InitWithMinSize(_object, DICTIONARY_DEFAULT_INITIALCOUNT, DICTIONARY_DEFAULT_EXPANSIONRATE)
