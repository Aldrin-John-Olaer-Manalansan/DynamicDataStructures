/*
 * @File: ChunkMap.h
 * @Author: Aldrin John O. Manalansan (ajom)
 * @Email: aldrinjohnolaermanalansan@gmail.com
 * @Brief: Implementation of ChunkMap Data Structure Management that uses DynamicArrays to store quickly, and BinarySearch to traverse quickly
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

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~USER CONFIGURATION~~~~~~~~~~~~~~~~~~~~~~~~~~~
/* Define the following macros at your configurations file "MainConfigurations.h" (rename if necessary)
 * #define CHUNKMAP_BITSPERCHUNK <bits>
 *     Sets the agressiveness of the chunk division
 * 	   Lower values will make the agressively organize the stored collection,, but the heavier the allocated memory size
 * 	   The lower the value, the more efficient the lookup would be, but the heavier the allocated memory size
 *     If not specified in MainConfigurations.h then it defaults to 128 = atleast 1024-bit precision
 */
#include "MainConfigurations.h" // rename if necessary
// ~~~~~~~~~~~~~~~~~~~~~~~~~~END OF USER CONFIGURATION~~~~~~~~~~~~~~~~~~~~~~~

#include <stdint.h>
#include <stdbool.h>

#if SIZE_MAX == 0xFFFF
	#define CHUNKMAP_CHUNK_BASESIZE 2
#elif SIZE_MAX == 0xFFFFFFFF
	#define CHUNKMAP_CHUNK_BASESIZE 4
#elif SIZE_MAX == 0xFFFFFFFFFFFFFFFF
	#define CHUNKMAP_CHUNK_BASESIZE 8
#endif

#ifndef CHUNKMAP_BITSPERCHUNK
	// single hex
	#define CHUNKMAP_BITSPERCHUNK 4
#endif

#define CHUNKMAP_CHUNK_SIZEMULTIPLIER (CHUNKMAP_BITSPERCHUNK + (CHUNKMAP_CHUNK_BASESIZE * 8) - 1) / (CHUNKMAP_CHUNK_BASESIZE * 8)
typedef size_t CHUNKMAP_CHUNK_DATATYPE[CHUNKMAP_CHUNK_SIZEMULTIPLIER];

typedef struct {
	void* data;
	CHUNKMAP_CHUNK_DATATYPE chunk;
} t_chunkmap;