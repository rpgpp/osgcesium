LOCAL_PATH :=             $(HopeMap3D_DIR)\HopeMap3DUtility

include $(CLEAR_VARS)

LIBDIR 			:= $(OSG_ANDROID_DIR)/lib

LOCAL_C_INCLUDES:= $(OSG_ANDROID_DIR)/include $(OSGE_ANDROID_DIR) $(HopeMap3D_DIR)

LOCAL_CPP_EXTENSION :=    cpp

LOCAL_LDLIBS :=           -llog -lGLESv2 -lz -lgnustl_static

LOCAL_MODULE :=           HopeMap3DUtility

MY_CPP_LIST := $(wildcard $(LOCAL_PATH)/src/*.cpp)

LOCAL_SRC_FILES := $(MY_CPP_LIST:$(LOCAL_PATH)/%=%)

LOCAL_CFLAGS :=            -DANDROID

LOCAL_CPPFLAGS :=          -DANDROID

LOCAL_LDFLAGS   := -L $(LIBDIR) \
-losgViewer \
-losgVolume \
-losgTerrain \
-losgText \
-losgShadow \
-losgSim \
-losgParticle \
-losgManipulator \
-losgGA \
-losgFX \
-losgDB \
-losgAnimation \
-losgUtil \
-losg \
-lOpenThreads \
-losgEarth \

include $(BUILD_STATIC_LIBRARY)
#include $(BUILD_SHARED_LIBRARY)

