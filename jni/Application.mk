# When building for OpenGL ES 2.0, android-8 (2.2) is required.
# Otherwise, android-4 is enough.

APP_PLATFORM := android-8
APP_STL := stlport_static
APP_ABI := armeabi armeabi-v7a x86
# APP_BUILD_SCRIPT := jni/Android.mk

NDK_TOOLCHAIN_VERSION := clang
APP_CPPFLAGS += -std=c++11 