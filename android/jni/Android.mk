LOCAL_PATH := $(call my-dir)



#-----------------------------------------------------------------------------
include $(CLEAR_VARS)

LOCAL_MODULE := "xtlrig"
XTLRIG_SRC_PATH := ../../

LOCAL_SRC_FILES :=    \
   $(XTLRIG_SRC_PATH)/src/api/NetworkState.cpp \
    $(XTLRIG_SRC_PATH)/src/android/App.cpp \
    $(XTLRIG_SRC_PATH)/src/common/config/CommonConfig.cpp \
    $(XTLRIG_SRC_PATH)/src/common/config/ConfigLoader.cpp \
    $(XTLRIG_SRC_PATH)/src/common/config/ConfigWatcher.cpp \
    $(XTLRIG_SRC_PATH)/src/common/Console.cpp \
    $(XTLRIG_SRC_PATH)/src/common/crypto/Algorithm.cpp \
    $(XTLRIG_SRC_PATH)/src/common/crypto/keccak.cpp \
    $(XTLRIG_SRC_PATH)/src/common/log/ConsoleLog.cpp \
    $(XTLRIG_SRC_PATH)/src/common/log/FileLog.cpp \
    $(XTLRIG_SRC_PATH)/src/common/log/Log.cpp \
    $(XTLRIG_SRC_PATH)/src/common/net/Client.cpp \
    $(XTLRIG_SRC_PATH)/src/common/net/Job.cpp \
    $(XTLRIG_SRC_PATH)/src/common/net/Pool.cpp \
    $(XTLRIG_SRC_PATH)/src/common/net/strategies/FailoverStrategy.cpp \
    $(XTLRIG_SRC_PATH)/src/common/net/strategies/SinglePoolStrategy.cpp \
    $(XTLRIG_SRC_PATH)/src/common/net/SubmitResult.cpp \
    $(XTLRIG_SRC_PATH)/src/common/Platform.cpp \
    $(XTLRIG_SRC_PATH)/src/core/Config.cpp \
    $(XTLRIG_SRC_PATH)/src/core/Controller.cpp \
    $(XTLRIG_SRC_PATH)/src/Mem.cpp \
    $(XTLRIG_SRC_PATH)/src/net/Network.cpp \
		$(XTLRIG_SRC_PATH)/src/net/strategies/DonateStrategy.cpp \
    $(XTLRIG_SRC_PATH)/src/Summary.cpp \
    $(XTLRIG_SRC_PATH)/src/workers/CpuThread.cpp \
    $(XTLRIG_SRC_PATH)/src/workers/Handle.cpp \
    $(XTLRIG_SRC_PATH)/src/android/workers/MultiWorker.cpp \
    $(XTLRIG_SRC_PATH)/src/android/workers/Worker.cpp \
    $(XTLRIG_SRC_PATH)/src/android/workers/Workers.cpp \
    $(XTLRIG_SRC_PATH)/src/xtlrig.cpp \
    $(XTLRIG_SRC_PATH)/src/App_unix.cpp \
    $(XTLRIG_SRC_PATH)/src/common/Platform_unix.cpp \
    $(XTLRIG_SRC_PATH)/src/Cpu_unix.cpp \
    $(XTLRIG_SRC_PATH)/src/Mem_unix.cpp \
    $(XTLRIG_SRC_PATH)/src/crypto/c_groestl.c \
    $(XTLRIG_SRC_PATH)/src/crypto/c_blake256.c \
    $(XTLRIG_SRC_PATH)/src/crypto/c_jh.c \
    $(XTLRIG_SRC_PATH)/src/crypto/c_skein.c

LOCAL_C_INCLUDES := $(LOCAL_PATH)/../../src $(LOCAL_PATH)/../libuv/include $(LOCAL_PATH)/../../src/3rdparty

LOCAL_CPPFLAGS +=  -std=c++11
LOCAL_CFLAGS   +=  -Wall -Ofast -funroll-loops -fmerge-all-constants
LOCAL_CPPFLAGS +=  -Wall -fno-exceptions -fno-rtti -Wno-missing-braces -Ofast -funroll-loops -fmerge-all-constants
LOCAL_CFLAGS +=   -DXMRIG_NO_HTTPD -DXMRIG_NO_API -DXMRIG_NO_LIBCPUID -DXMRIG_NO_SUMO -DXMRIG_NO_IPBC

ifeq ($(TARGET_ARCH_ABI),armeabi-v7a)
LOCAL_CFLAGS   += -mfpu=neon -march=armv7-a
LOCAL_CPPFLAGS += -mfpu=neon -march=armv7-a
else ifeq ($(TARGET_ARCH_ABI),arm64-v8a)
LOCAL_CFLAGS   +=  -march=armv8-a+crypto
LOCAL_CPPFLAGS +=  -march=armv8-a+crypto
else ifeq ($(TARGET_ARCH_ABI),x86)
LOCAL_CFLAGS   +=  -maes
LOCAL_CPPFLAGS +=  -maes
endif

ifeq ($(TARGET_ARCH_ABI),armeabi-v7a)
LOCAL_CFLAGS += -DXMRIG_ARMv7 -DXMRIG_ARM
LOCAL_LDLIBS +=  -luv_armv7
LOCAL_SRC_FILES += $(XTLRIG_SRC_PATH)/src/Cpu_arm.cpp
else ifeq ($(TARGET_ARCH_ABI),arm64-v8a)
LOCAL_CFLAGS += -DXMRIG_ARMv8 -DXMRIG_ARM
LOCAL_LDLIBS +=  -luv_armv8
LOCAL_SRC_FILES += $(XTLRIG_SRC_PATH)/src/Cpu_arm.cpp
else ifeq ($(TARGET_ARCH_ABI), x86)
LOCAL_CFLAGS += -DRAPIDJSON_SSE2
LOCAL_LDLIBS +=  -luv_x86
LOCAL_SRC_FILES += $(XTLRIG_SRC_PATH)/src/Cpu_stub.cpp
endif

LOCAL_LDFLAGS += -L$(LOCAL_PATH)/../libuv/lib


LOCAL_CFLAGS += -fPIE -Wall
LOCAL_LDFLAGS += -fPIE -pie -llog
include $(BUILD_EXECUTABLE)
