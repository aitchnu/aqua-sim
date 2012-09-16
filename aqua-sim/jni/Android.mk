LOCAL_PATH := $(call my-dir)
include $(CLEAR_VARS)
LS_CPP=$(subst $(1)/,,$(wildcard $(1)/*.cpp))
LOCAL_MODULE    := aqua-sim
LOCAL_SRC_FILES := $(call LS_CPP,$(LOCAL_PATH))
LOCAL_LDLIBS    := -landroid -llog -lGLESv2 -lEGL
LOCAL_STATIC_LIBRARIES := android_native_app_glue box2d_static png
APP_OPTIM := debug
include $(BUILD_SHARED_LIBRARY)
$(call import-module,android/native_app_glue)
$(call import-module,box2d)
$(call import-module,libpng)