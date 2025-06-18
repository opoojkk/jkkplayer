cmake_minimum_required(VERSION 3.22.1)

project(player)
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_VERBOSE_MAKEFILE ON)

# 系统 JNI 头文件路径，优先级最高，必须先加入以避免 jni.h 冲突
set(SYSTEM_INCLUDE_DIRS
        ${ANDROID_NDK}/sysroot/usr/include
        ${ANDROID_NDK}/sysroot/usr/include/${ANDROID_ABI}
)

# FFmpeg 预编译 so 存放目录（按 ABI 分类）
set(JNI_LIBS_DIR ${CMAKE_SOURCE_DIR}/../jniLibs)
set(LIB_DIR ${JNI_LIBS_DIR}/${ANDROID_ABI})

# 源文件构建本地库
add_library(player-native
        SHARED
        ffmpeg_jni.cpp
        VLog.cpp
)

# 1. 设置系统 include，使用 BEFORE 确保优先级，避免 jni.h 被 FFmpeg 的 jni.h 干扰
target_include_directories(player-native
        BEFORE
        PRIVATE
        ${SYSTEM_INCLUDE_DIRS}
)

# 2. 加载 FFmpeg 的 so 库
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

# 3. 设置 FFmpeg 头文件路径（注意不要把 libavcodec 加到顶级路径以免污染）
set(FFMPEG_INCLUDE_DIR ${CMAKE_SOURCE_DIR}/include/third_party/ffmpeg)

target_include_directories(player-native
        PRIVATE
        ${CMAKE_SOURCE_DIR}/include                # 你项目自己的头文件
        ${FFMPEG_INCLUDE_DIR}                      # FFmpeg 根目录
#        ${FFMPEG_INCLUDE_DIR}/log                  # 你的日志模块
        # ⚠️ 不要包含 ${FFMPEG_INCLUDE_DIR}/libavcodec... 作为全局路径
        # 如需要 avcodec/jni.h，请在 cpp 文件中显式 #include "libavcodec/jni.h"
)

# 4. 链接本地库和系统库
target_link_libraries(
        player-native
        log
        avcodec
        avfilter
        avformat
        avutil
        swscale
        swresample
)
