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

#include "defyx.h"
#include "dataset.hpp"
#include "vm_interpreted.hpp"
#include "vm_interpreted_light.hpp"
#include "vm_compiled.hpp"
#include "vm_compiled_light.hpp"
#include "blake2/blake2.h"
#include <cassert>
#include <limits>

extern "C" {

	defyx_cache *defyx_alloc_cache(defyx_flags flags) {
		defyx_cache *cache;

		try {
			cache = new defyx_cache();
			switch (flags & (RANDOMX_FLAG_JIT | RANDOMX_FLAG_LARGE_PAGES)) {
				case RANDOMX_FLAG_DEFAULT:
					cache->dealloc = &defyx::deallocCache<defyx::DefaultAllocator>;
					cache->jit = nullptr;
					cache->initialize = &defyx::initCache;
					cache->datasetInit = &defyx::initDataset;
					cache->memory = (uint8_t*)defyx::DefaultAllocator::allocMemory(defyx::CacheSize);
					break;

				case RANDOMX_FLAG_JIT:
					cache->dealloc = &defyx::deallocCache<defyx::DefaultAllocator>;
					cache->jit = new defyx::JitCompiler();
					cache->initialize = &defyx::initCacheCompile;
					cache->datasetInit = cache->jit->getDatasetInitFunc();
					cache->memory = (uint8_t*)defyx::DefaultAllocator::allocMemory(defyx::CacheSize);
					break;

				case RANDOMX_FLAG_LARGE_PAGES:
					cache->dealloc = &defyx::deallocCache<defyx::LargePageAllocator>;
					cache->jit = nullptr;
					cache->initialize = &defyx::initCache;
					cache->datasetInit = &defyx::initDataset;
					cache->memory = (uint8_t*)defyx::LargePageAllocator::allocMemory(defyx::CacheSize);
					break;

				case RANDOMX_FLAG_JIT | RANDOMX_FLAG_LARGE_PAGES:
					cache->dealloc = &defyx::deallocCache<defyx::LargePageAllocator>;
					cache->jit = new defyx::JitCompiler();
					cache->initialize = &defyx::initCacheCompile;
					cache->datasetInit = cache->jit->getDatasetInitFunc();
					cache->memory = (uint8_t*)defyx::LargePageAllocator::allocMemory(defyx::CacheSize);
					break;

				default:
					UNREACHABLE;
			}
		}
		catch (std::exception &ex) {
			if (cache != nullptr) {
				defyx_release_cache(cache);
				cache = nullptr;
			}
		}

		return cache;
	}

	void defyx_init_cache(defyx_cache *cache, const void *key, size_t keySize) {
		assert(cache != nullptr);
		assert(keySize == 0 || key != nullptr);
		cache->initialize(cache, key, keySize);
	}

	void defyx_release_cache(defyx_cache* cache) {
		assert(cache != nullptr);
		cache->dealloc(cache);
		delete cache;
	}

	defyx_dataset *defyx_alloc_dataset(defyx_flags flags) {

		//fail on 32-bit systems if DatasetSize is >= 4 GiB
		if (defyx::DatasetSize > std::numeric_limits<size_t>::max()) {
			return nullptr;
		}

		defyx_dataset *dataset;

		try {
			dataset = new defyx_dataset();
			if (flags & RANDOMX_FLAG_LARGE_PAGES) {
				dataset->dealloc = &defyx::deallocDataset<defyx::LargePageAllocator>;
				dataset->memory = (uint8_t*)defyx::LargePageAllocator::allocMemory(defyx::DatasetSize);
			}
			else {
				dataset->dealloc = &defyx::deallocDataset<defyx::DefaultAllocator>;
				dataset->memory = (uint8_t*)defyx::DefaultAllocator::allocMemory(defyx::DatasetSize);
			}
		}
		catch (std::exception &ex) {
			if (dataset != nullptr) {
				defyx_release_dataset(dataset);
				dataset = nullptr;
			}
		}

		return dataset;
	}

	constexpr unsigned long DatasetItemCount = defyx::DatasetSize / RANDOMX_DATASET_ITEM_SIZE;

	unsigned long defyx_dataset_item_count() {
		return DatasetItemCount;
	}

	void defyx_init_dataset(defyx_dataset *dataset, defyx_cache *cache, unsigned long startItem, unsigned long itemCount) {
		assert(dataset != nullptr);
		assert(cache != nullptr);
		assert(startItem < DatasetItemCount && itemCount <= DatasetItemCount);
		assert(startItem + itemCount <= DatasetItemCount);
		cache->datasetInit(cache, dataset->memory + startItem * defyx::CacheLineSize, startItem, startItem + itemCount);
	}

	void *defyx_get_dataset_memory(defyx_dataset *dataset) {
		assert(dataset != nullptr);
		return dataset->memory;
	}

	void defyx_release_dataset(defyx_dataset *dataset) {
		assert(dataset != nullptr);
		dataset->dealloc(dataset);
		delete dataset;
	}

	defyx_vm *defyx_create_vm(defyx_flags flags, defyx_cache *cache, defyx_dataset *dataset) {
		assert(cache != nullptr || (flags & RANDOMX_FLAG_FULL_MEM));
		assert(cache == nullptr || cache->isInitialized());
		assert(dataset != nullptr || !(flags & RANDOMX_FLAG_FULL_MEM));

		defyx_vm *vm = nullptr;

		try {
			switch (flags & (RANDOMX_FLAG_FULL_MEM | RANDOMX_FLAG_JIT | RANDOMX_FLAG_HARD_AES | RANDOMX_FLAG_LARGE_PAGES)) {
				case RANDOMX_FLAG_DEFAULT:
					vm = new defyx::InterpretedLightVmDefault();
					break;

				case RANDOMX_FLAG_FULL_MEM:
					vm = new defyx::InterpretedVmDefault();
					break;

				case RANDOMX_FLAG_JIT:
					vm = new defyx::CompiledLightVmDefault();
					break;

				case RANDOMX_FLAG_FULL_MEM | RANDOMX_FLAG_JIT:
					vm = new defyx::CompiledVmDefault();
					break;

				case RANDOMX_FLAG_HARD_AES:
					vm = new defyx::InterpretedLightVmHardAes();
					break;

				case RANDOMX_FLAG_FULL_MEM | RANDOMX_FLAG_HARD_AES:
					vm = new defyx::InterpretedVmHardAes();
					break;

				case RANDOMX_FLAG_JIT | RANDOMX_FLAG_HARD_AES:
					vm = new defyx::CompiledLightVmHardAes();
					break;

				case RANDOMX_FLAG_FULL_MEM | RANDOMX_FLAG_JIT | RANDOMX_FLAG_HARD_AES:
					vm = new defyx::CompiledVmHardAes();
					break;

				case RANDOMX_FLAG_LARGE_PAGES:
					vm = new defyx::InterpretedLightVmLargePage();
					break;

				case RANDOMX_FLAG_FULL_MEM | RANDOMX_FLAG_LARGE_PAGES:
					vm = new defyx::InterpretedVmLargePage();
					break;

				case RANDOMX_FLAG_JIT | RANDOMX_FLAG_LARGE_PAGES:
					vm = new defyx::CompiledLightVmLargePage();
					break;

				case RANDOMX_FLAG_FULL_MEM | RANDOMX_FLAG_JIT | RANDOMX_FLAG_LARGE_PAGES:
					vm = new defyx::CompiledVmLargePage();
					break;

				case RANDOMX_FLAG_HARD_AES | RANDOMX_FLAG_LARGE_PAGES:
					vm = new defyx::InterpretedLightVmLargePageHardAes();
					break;

				case RANDOMX_FLAG_FULL_MEM | RANDOMX_FLAG_HARD_AES | RANDOMX_FLAG_LARGE_PAGES:
					vm = new defyx::InterpretedVmLargePageHardAes();
					break;

				case RANDOMX_FLAG_JIT | RANDOMX_FLAG_HARD_AES | RANDOMX_FLAG_LARGE_PAGES:
					vm = new defyx::CompiledLightVmLargePageHardAes();
					break;

				case RANDOMX_FLAG_FULL_MEM | RANDOMX_FLAG_JIT | RANDOMX_FLAG_HARD_AES | RANDOMX_FLAG_LARGE_PAGES:
					vm = new defyx::CompiledVmLargePageHardAes();
					break;

				default:
					UNREACHABLE;
			}

			if(cache != nullptr)
				vm->setCache(cache);

			if(dataset != nullptr)
				vm->setDataset(dataset);

			vm->allocate();
		}
		catch (std::exception &ex) {
			delete vm;
			vm = nullptr;
		}

		return vm;
	}

	void defyx_vm_set_cache(defyx_vm *machine, defyx_cache* cache) {
		assert(machine != nullptr);
		assert(cache != nullptr && cache->isInitialized());
		machine->setCache(cache);
	}

	void defyx_vm_set_dataset(defyx_vm *machine, defyx_dataset *dataset) {
		assert(machine != nullptr);
		assert(dataset != nullptr);
		machine->setDataset(dataset);
	}

	void defyx_destroy_vm(defyx_vm *machine) {
		assert(machine != nullptr);
		delete machine;
	}

	void defyx_calculate_hash(defyx_vm *machine, const void *input, size_t inputSize, void *output) {
		assert(machine != nullptr);
		assert(inputSize == 0 || input != nullptr);
		assert(output != nullptr);
		alignas(16) uint64_t tempHash[8];
		int blakeResult = blake2b(tempHash, sizeof(tempHash), input, inputSize, nullptr, 0);
		int yescryptRH = sipesh(tempHash, sizeof(tempHash), input, inputSize, input, inputSize, 0, 0);
		int kangarooTwelve = k12(input, inputSize, tempHash);
		assert(blakeResult == 0);
		machine->initScratchpad(&tempHash);
		machine->resetRoundingMode();
		for (int chain = 0; chain < RANDOMX_PROGRAM_COUNT - 1; ++chain) {
			machine->run(&tempHash);
			blakeResult = blake2b(tempHash, sizeof(tempHash), machine->getRegisterFile(), sizeof(defyx::RegisterFile), nullptr, 0);
			assert(blakeResult == 0);
		}
		machine->run(&tempHash);
		machine->getFinalResult(output, RANDOMX_HASH_SIZE);
	}

}
