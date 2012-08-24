LOCAL_PATH := $(call my-dir)
include $(CLEAR_VARS)
LOCAL_MODULE    := aqua-sim
LOCAL_SRC_FILES := Main.cpp
LOCAL_LDLIBS    := -landroid -llog -lGLESv2 -lEGL
LOCAL_STATIC_LIBRARIES := android_native_app_glue box2d_static png
APP_OPTIM := debug
include $(BUILD_SHARED_LIBRARY)
$(call import-module,android/native_app_glue)
$(call import-module,box2d)
$(call import-module,libpng)