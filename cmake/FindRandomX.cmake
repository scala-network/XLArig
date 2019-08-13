find_path(
    RANDOMX_INCLUDE_DIR
    NAMES defyx.h
    PATHS "${XMRIG_DEPS}" ENV "XMRIG_DEPS"
    PATH_SUFFIXES "include"
    NO_DEFAULT_PATH
)

find_path(RANDOMX_INCLUDE_DIR NAMES defyx.h)

find_library(
    RANDOMX_LIBRARY
    NAMES libdefyx.a defyx libdefyx
    PATHS "${XMRIG_DEPS}" ENV "XMRIG_DEPS"
    PATH_SUFFIXES "lib"
    NO_DEFAULT_PATH
)

find_library(RANDOMX_LIBRARY NAMES libdefyx.a defyx libdefyx)

set(RANDOMX_LIBRARIES ${RANDOMX_LIBRARY})
set(RANDOMX_INCLUDE_DIRS ${RANDOMX_INCLUDE_DIR})

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(RANDOMX DEFAULT_MSG RANDOMX_LIBRARY RANDOMX_INCLUDE_DIR)
