# Copyright (c) 2019, The Monero Project
# 
# All rights reserved.
# 
# Redistribution and use in source and binary forms, with or without modification, are
# permitted provided that the following conditions are met:
# 
# 1. Redistributions of source code must retain the above copyright notice, this list of
#    conditions and the following disclaimer.
# 
# 2. Redistributions in binary form must reproduce the above copyright notice, this list
#    of conditions and the following disclaimer in the documentation and/or other
#    materials provided with the distribution.
# 
# 3. Neither the name of the copyright holder nor the names of its contributors may be
#    used to endorse or promote products derived from this software without specific
#    prior written permission.
# 
# THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY
# EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
# MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL
# THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
# SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
# PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
# INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
# STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF
# THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

cmake_minimum_required(VERSION 2.8.7)

set (defyx_sources
src/aes_hash.cpp
src/argon2_ref.c
src/bytecode_machine.cpp
src/dataset.cpp
src/soft_aes.cpp
src/virtual_memory.cpp
src/vm_interpreted.cpp
src/allocator.cpp
src/assembly_generator_x86.cpp
src/instruction.cpp
src/defyx.cpp
src/superscalar.cpp
src/vm_compiled.cpp
src/vm_interpreted_light.cpp
src/argon2_core.c
src/blake2_generator.cpp
src/instructions_portable.cpp
src/reciprocal.c
src/virtual_machine.cpp
src/vm_compiled_light.cpp
src/blake2/blake2b.c
src/blake2/sha256.c
src/blake2/yescrypt-best.c
src/blake2/KangarooTwelve.c
src/blake2/KeccakP-1600-reference.c
src/blake2/KeccakSpongeWidth1600.c
src/blake2/yescrypt-common.c
src/blake2/yescrypt-platform.c
src/blake2/yescrypt-ref.c
src/blake2/yescrypt-simd.c
src/blake2/yescrypt-opt.c)

if(NOT ARCH_ID)
  # allow cross compiling
  if(CMAKE_SYSTEM_PROCESSOR STREQUAL "")
    set(CMAKE_SYSTEM_PROCESSOR ${CMAKE_HOST_SYSTEM_PROCESSOR})
  endif()
  string(TOLOWER "${CMAKE_SYSTEM_PROCESSOR}" ARCH_ID)
endif()

if(NOT ARM_ID)
  set(ARM_ID "${ARCH_ID}")
endif()

if(NOT ARCH)
  set(ARCH "default")
endif()

if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Release)
  message(STATUS "Setting default build type: ${CMAKE_BUILD_TYPE}")
endif()

include(CheckCXXCompilerFlag)
include(CheckCCompilerFlag)

function(add_flag flag)
  string(REPLACE "-" "_" supported_cxx ${flag}_cxx)
  check_cxx_compiler_flag(${flag} ${supported_cxx})
  if(${${supported_cxx}})
    message(STATUS "Setting CXX flag ${flag}")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${flag}" PARENT_SCOPE)
  endif()
  string(REPLACE "-" "_" supported_c ${flag}_c)
  check_c_compiler_flag(${flag} ${supported_c})
  if(${${supported_c}})
    message(STATUS "Setting C flag ${flag}")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${flag}" PARENT_SCOPE)
  endif()
endfunction()

# x86-64
if (ARCH_ID STREQUAL "x86_64" OR ARCH_ID STREQUAL "x86-64" OR ARCH_ID STREQUAL "amd64")
  list(APPEND defyx_sources
    src/jit_compiler_x86_static.S
    src/jit_compiler_x86.cpp)
  # cheat because cmake and ccache hate each other
  set_property(SOURCE src/jit_compiler_x86_static.S PROPERTY LANGUAGE C)

  if(ARCH STREQUAL "native")
    add_flag("-march=native")
  else()
    # default build has hardware AES enabled (software AES can be selected at runtime)
    add_flag("-maes")
  endif()
endif()

# PowerPC
if (ARCH_ID STREQUAL "ppc64" OR ARCH_ID STREQUAL "ppc64le")
  if(ARCH STREQUAL "native")
    add_flag("-mcpu=native")
  endif()
  # PowerPC AES requires ALTIVEC (POWER7+), so it cannot be enabled in the default build
endif()

# ARMv8
if (ARM_ID STREQUAL "aarch64" OR ARM_ID STREQUAL "arm64" OR ARM_ID STREQUAL "armv8-a")
  if(ARCH STREQUAL "native")
    add_flag("-march=native")
  else()
    # default build has hardware AES enabled (software AES can be selected at runtime)
    add_flag("-march=armv8-a+crypto")
  endif()
endif()

set(RANDOMX_INCLUDE "${CMAKE_CURRENT_SOURCE_DIR}/src" CACHE STRING "DefyX Include path")

add_library(defyx
  ${defyx_sources})
set_property(TARGET defyx PROPERTY POSITION_INDEPENDENT_CODE ON)
set_property(TARGET defyx PROPERTY CXX_STANDARD 11)
set_property(TARGET defyx PROPERTY CXX_STANDARD_REQUIRED ON)

add_executable(DefyX-tests
  src/tests/tests.cpp)
target_link_libraries(DefyX-tests
  PRIVATE defyx)
set_property(TARGET DefyX-tests PROPERTY POSITION_INDEPENDENT_CODE ON)
set_property(TARGET DefyX-tests PROPERTY CXX_STANDARD 11)

add_executable(defyx-codegen
  src/tests/code-generator.cpp)
target_link_libraries(defyx-codegen
  PRIVATE defyx)

set_property(TARGET defyx-codegen PROPERTY POSITION_INDEPENDENT_CODE ON)
set_property(TARGET defyx-codegen PROPERTY CXX_STANDARD 11)

if (NOT Threads_FOUND AND UNIX AND NOT APPLE)
  set(THREADS_PREFER_PTHREAD_FLAG ON)
  find_package(Threads)
endif()

add_executable(defyx-benchmark
  src/tests/benchmark.cpp
  src/tests/affinity.cpp)
target_link_libraries(defyx-benchmark
  PRIVATE defyx
  PRIVATE ${CMAKE_THREAD_LIBS_INIT})

include(CheckCXXSourceCompiles)
check_cxx_source_compiles("
#include <cstdint>
#include <atomic>
int main() {
  std::atomic<uint64_t> a;
  a.is_lock_free();
}" HAVE_CXX_ATOMICS)

if(NOT HAVE_CXX_ATOMICS)
  target_link_libraries(defyx-benchmark
    PRIVATE "atomic")
endif()
set_property(TARGET defyx-benchmark PROPERTY POSITION_INDEPENDENT_CODE ON)
set_property(TARGET defyx-benchmark PROPERTY CXX_STANDARD 11)
