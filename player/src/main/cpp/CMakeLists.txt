# You can visit https://developer.android.google.cn/studio/projects/configure-cmake?hl=zh_cn
# for more information about CMake and how to use it in Android projects.


# Sets the minimum version of CMake required to build your native library.
# This ensures that a certain set of CMake features is available to
# your build.

# 一定要用高版本的，不然jni.h的冲突解决不了，试了很多方案
cmake_minimum_required(VERSION 3.31.1)

project(player)
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_VERBOSE_MAKEFILE ON)

# Specifies a library name, specifies whether the library is STATIC or
# SHARED, and provides relative paths to the source code. You can
# define multiple libraries by adding multiple add_library() commands,
# and CMake builds them for you. When you build your app, Gradle
# automatically packages shared libraries with your APK.

set(SYSTEM_INCLUDE_DIRS
        ${ANDROID_NDK}/sysroot/usr/include
        ${ANDROID_NDK}/sysroot/usr/include/${ANDROID_ABI}
)

# for ffmpeg

set(JNI_LIBS_DIR ${CMAKE_SOURCE_DIR}/../jniLibs)

# 根据当前ABI设置库路径
set(LIB_DIR ${JNI_LIBS_DIR}/${ANDROID_ABI})

# 指定生成的库名称和类型，添加源文件
add_library(player-native
        SHARED
        ffmpeg_jni.cpp
        VLog.cpp
)

# 3. 关键步骤：将系统路径插入到搜索列表最前面
target_include_directories(player-native
        BEFORE PRIVATE          # BEFORE 确保最高优先级
        ${SYSTEM_INCLUDE_DIRS}
        SYSTEM                 # 标记为系统目录（抑制警告）
)

link_directories(${LIB_DIR})

add_library(avcodec SHARED IMPORTED)
set_target_properties(avcodec PROPERTIES IMPORTED_LOCATION ${LIB_DIR}/libavcodec.so)

add_library(avfilter SHARED IMPORTED)
set_target_properties(avfilter PROPERTIES IMPORTED_LOCATION ${LIB_DIR}/libavfilter.so)

add_library(avformat SHARED IMPORTED)
set_target_properties(avformat PROPERTIES IMPORTED_LOCATION ${LIB_DIR}/libavformat.so)

add_library(avutil SHARED IMPORTED)
set_target_properties(avutil PROPERTIES IMPORTED_LOCATION ${LIB_DIR}/libavutil.so)

add_library(swscale SHARED IMPORTED)
set_target_properties(swscale PROPERTIES IMPORTED_LOCATION ${LIB_DIR}/libswscale.so)

add_library(swresample SHARED IMPORTED)
set_target_properties(swresample PROPERTIES IMPORTED_LOCATION ${LIB_DIR}/libswresample.so)

set(FFMPEG_INCLUDE_DIR ${CMAKE_SOURCE_DIR}/include/third_party/ffmpeg)

# 文件不会递归，需要手动添加
target_include_directories(native-lib
        PRIVATE
        ${CMAKE_SOURCE_DIR}/include
        ${FFMPEG_INCLUDE_DIR}
        ${FFMPEG_INCLUDE_DIR}/log
        # Include directories for FFmpeg
        ${FFMPEG_INCLUDE_DIR}/libavutil
        ${FFMPEG_INCLUDE_DIR}/libavcodec
        ${FFMPEG_INCLUDE_DIR}/libavformat
        ${FFMPEG_INCLUDE_DIR}/libavfilter
        ${FFMPEG_INCLUDE_DIR}/libswscale
        ${FFMPEG_INCLUDE_DIR}/libswresample
)

target_link_libraries(player-native
        log
        avcodec
        avfilter
        avformat
        avutil
        swscale
        swresample
)