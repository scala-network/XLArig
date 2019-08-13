/*
Copyright (c) 2018-2019, tevador <tevador@gmail.com>

All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:
	* Redistributions of source code must retain the above copyright
	  notice, this list of conditions and the following disclaimer.
	* Redistributions in binary form must reproduce the above copyright
	  notice, this list of conditions and the following disclaimer in the
	  documentation and/or other materials provided with the distribution.
	* Neither the name of the copyright holder nor the
	  names of its contributors may be used to endorse or promote products
	  derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#ifndef RANDOMX_H
#define RANDOMX_H

#include <stddef.h>

#define RANDOMX_HASH_SIZE 32
#define RANDOMX_DATASET_ITEM_SIZE 64

#ifndef RANDOMX_EXPORT
#define RANDOMX_EXPORT
#endif

typedef enum {
  RANDOMX_FLAG_DEFAULT = 0,
  RANDOMX_FLAG_LARGE_PAGES = 1,
  RANDOMX_FLAG_HARD_AES = 2,
  RANDOMX_FLAG_FULL_MEM = 4,
  RANDOMX_FLAG_JIT = 8,
} defyx_flags;

typedef struct defyx_dataset defyx_dataset;
typedef struct defyx_cache defyx_cache;
typedef struct defyx_vm defyx_vm;

#if defined(__cplusplus)
extern "C" {
#endif

/**
 * Creates a defyx_cache structure and allocates memory for DefyX Cache.
 *
 * @param flags is any combination of these 2 flags (each flag can be set or not set):
 *        RANDOMX_FLAG_LARGE_PAGES - allocate memory in large pages
 *        RANDOMX_FLAG_JIT - create cache structure with JIT compilation support; this makes
 *                           subsequent Dataset initialization faster
 *
 * @return Pointer to an allocated defyx_cache structure.
 *         NULL is returned if memory allocation fails or if the RANDOMX_FLAG_JIT
 *         is set and JIT compilation is not supported on the current platform.
 */
RANDOMX_EXPORT defyx_cache *defyx_alloc_cache(defyx_flags flags);

/**
 * Initializes the cache memory and SuperscalarHash using the provided key value.
 *
 * @param cache is a pointer to a previously allocated defyx_cache structure. Must not be NULL.
 * @param key is a pointer to memory which contains the key value. Must not be NULL.
 * @param keySize is the number of bytes of the key.
*/
RANDOMX_EXPORT void defyx_init_cache(defyx_cache *cache, const void *key, size_t keySize);

/**
 * Releases all memory occupied by the defyx_cache structure.
 *
 * @param cache is a pointer to a previously allocated defyx_cache structure.
*/
RANDOMX_EXPORT void defyx_release_cache(defyx_cache* cache);

/**
 * Creates a defyx_dataset structure and allocates memory for DefyX Dataset.
 *
 * @param flags is the initialization flags. Only one flag is supported (can be set or not set):
 *        RANDOMX_FLAG_LARGE_PAGES - allocate memory in large pages
 *
 * @return Pointer to an allocated defyx_dataset structure.
 *         NULL is returned if memory allocation fails.
 */
RANDOMX_EXPORT defyx_dataset *defyx_alloc_dataset(defyx_flags flags);

/**
 * Gets the number of items contained in the dataset.
 *
 * @return the number of items contained in the dataset.
*/
RANDOMX_EXPORT unsigned long defyx_dataset_item_count(void);

/**
 * Initializes dataset items.
 *
 * Note: In order to use the Dataset, all items from 0 to (defyx_dataset_item_count() - 1) must be initialized.
 * This may be done by several calls to this function using non-overlapping item sequences.
 *
 * @param dataset is a pointer to a previously allocated defyx_dataset structure. Must not be NULL.
 * @param cache is a pointer to a previously allocated and initialized defyx_cache structure. Must not be NULL.
 * @param startItem is the item number where intialization should start.
 * @param itemCount is the number of items that should be initialized.
*/
RANDOMX_EXPORT void defyx_init_dataset(defyx_dataset *dataset, defyx_cache *cache, unsigned long startItem, unsigned long itemCount);

/**
 * Returns a pointer to the internal memory buffer of the dataset structure. The size
 * of the internal memory buffer is defyx_dataset_item_count() * RANDOMX_DATASET_ITEM_SIZE.
 *
 * @param dataset is dataset is a pointer to a previously allocated defyx_dataset structure. Must not be NULL.
 *
 * @return Pointer to the internal memory buffer of the dataset structure.
*/
RANDOMX_EXPORT void *defyx_get_dataset_memory(defyx_dataset *dataset);

/**
 * Releases all memory occupied by the defyx_dataset structure.
 *
 * @param dataset is a pointer to a previously allocated defyx_dataset structure.
*/
RANDOMX_EXPORT void defyx_release_dataset(defyx_dataset *dataset);

/**
 * Creates and initializes a DefyX virtual machine.
 *
 * @param flags is any combination of these 4 flags (each flag can be set or not set):
 *        RANDOMX_FLAG_LARGE_PAGES - allocate scratchpad memory in large pages
 *        RANDOMX_FLAG_HARD_AES - virtual machine will use hardware accelerated AES
 *        RANDOMX_FLAG_FULL_MEM - virtual machine will use the full dataset
 *        RANDOMX_FLAG_JIT - virtual machine will use a JIT compiler
 *        The numeric values of the flags are ordered so that a higher value will provide
 *        faster hash calculation and a lower numeric value will provide higher portability.
 *        Using RANDOMX_FLAG_DEFAULT (all flags not set) works on all platforms, but is the slowest.
 * @param cache is a pointer to an initialized defyx_cache structure. Can be
 *        NULL if RANDOMX_FLAG_FULL_MEM is set.
 * @param dataset is a pointer to a defyx_dataset structure. Can be NULL
 *        if RANDOMX_FLAG_FULL_MEM is not set.
 *
 * @return Pointer to an initialized defyx_vm structure.
 *         Returns NULL if:
 *         (1) Scratchpad memory allocation fails.
 *         (2) The requested initialization flags are not supported on the current platform.
 *         (3) cache parameter is NULL and RANDOMX_FLAG_FULL_MEM is not set
 *         (4) dataset parameter is NULL and RANDOMX_FLAG_FULL_MEM is set
*/
RANDOMX_EXPORT defyx_vm *defyx_create_vm(defyx_flags flags, defyx_cache *cache, defyx_dataset *dataset);

/**
 * Reinitializes a virtual machine with a new Cache. This function should be called anytime
 * the Cache is reinitialized with a new key.
 *
 * @param machine is a pointer to a defyx_vm structure that was initialized
 *        without RANDOMX_FLAG_FULL_MEM. Must not be NULL.
 * @param cache is a pointer to an initialized defyx_cache structure. Must not be NULL.
*/
RANDOMX_EXPORT void defyx_vm_set_cache(defyx_vm *machine, defyx_cache* cache);

/**
 * Reinitializes a virtual machine with a new Dataset.
 *
 * @param machine is a pointer to a defyx_vm structure that was initialized
 *        with RANDOMX_FLAG_FULL_MEM. Must not be NULL.
 * @param dataset is a pointer to an initialized defyx_dataset structure. Must not be NULL.
*/
RANDOMX_EXPORT void defyx_vm_set_dataset(defyx_vm *machine, defyx_dataset *dataset);

/**
 * Releases all memory occupied by the defyx_vm structure.
 *
 * @param machine is a pointer to a previously created defyx_vm structure.
*/
RANDOMX_EXPORT void defyx_destroy_vm(defyx_vm *machine);

/**
 * Calculates a DefyX hash value.
 *
 * @param machine is a pointer to a defyx_vm structure. Must not be NULL.
 * @param input is a pointer to memory to be hashed. Must not be NULL.
 * @param inputSize is the number of bytes to be hashed.
 * @param output is a pointer to memory where the hash will be stored. Must not
 *        be NULL and at least RANDOMX_HASH_SIZE bytes must be available for writing.
*/
RANDOMX_EXPORT void defyx_calculate_hash(defyx_vm *machine, const void *input, size_t inputSize, void *output);

#if defined(__cplusplus)
}
#endif

#endif
