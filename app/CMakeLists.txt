# For more information about using CMake with Android Studio, read the
# documentation: https://d.android.com/studio/projects/add-native-code.html

# Sets the minimum version of CMake required to build the native library.



# Creates and names a library, sets it as either STATIC
# or SHARED, and provides the relative paths to its source code.
# You can define multiple libraries, and CMake builds them for you.
# Gradle automatically packages shared libraries with your APK.

#add_library( # Sets the name of the library.
#             native-lib
#
#             # Sets the library as a shared library.
#             SHARED
#
#             # Provides a relative path to your source file(s).
#             src/main/cpp/native-lib.cpp )

# Searches for a specified prebuilt library and stores the path as a
# variable. Because CMake includes system libraries in the search path by
# default, you only need to specify the name of the public NDK library
# you want to add. CMake verifies that the library exists before
# completing its build.

# find_library( # Sets the name of the path variable.
#               log-lib
#
#               # Specifies the name of the NDK library that
#               # you want CMake to locate.
#               log )

# Specifies libraries CMake should link to your target library. You
# can link multiple libraries, such as libraries you define in this
# build script, prebuilt third-party libraries, or system libraries.

########################################################################################################################



cmake_minimum_required(VERSION 3.4.1)

# 设置cpp目录路径
set(CPP_DIR ${CMAKE_SOURCE_DIR}/src/main/cpp)

include_directories(
            ${CMAKE_SOURCE_DIR}/ffmpeg-3.0.11/
            ${CPP_DIR}/include/)


find_library(log-lib log)


add_library(avcodec
                SHARED
                IMPORTED)

set_target_properties(avcodec
                        PROPERTIES
                        IMPORTED_LOCATION
                        ${CMAKE_SOURCE_DIR}/libs/armeabi/libavcodec.so)

add_library(avfilter
                SHARED
                IMPORTED)

set_target_properties(avfilter
                        PROPERTIES
                        IMPORTED_LOCATION
                        ${CMAKE_SOURCE_DIR}/libs/armeabi/libavfilter.so)

add_library(avformat
                SHARED
                IMPORTED)

set_target_properties(avformat
                        PROPERTIES
                        IMPORTED_LOCATION
                        ${CMAKE_SOURCE_DIR}/libs/armeabi/libavformat.so)

add_library(avutil
                SHARED
                IMPORTED)

set_target_properties(avutil
                        PROPERTIES
                        IMPORTED_LOCATION
                        ${CMAKE_SOURCE_DIR}/libs/armeabi/libavutil.so)

add_library(swresample
                SHARED
                IMPORTED)

set_target_properties(swresample
                        PROPERTIES
                        IMPORTED_LOCATION
                        ${CMAKE_SOURCE_DIR}/libs/armeabi/libswresample.so)

add_library(swscale
                SHARED
                IMPORTED)

set_target_properties(swscale
                        PROPERTIES
                        IMPORTED_LOCATION
                        ${CMAKE_SOURCE_DIR}/libs/armeabi/libswscale.so)

add_library(yuv
                SHARED
                IMPORTED)

set_target_properties(yuv
                        PROPERTIES
                        IMPORTED_LOCATION
                        ${CMAKE_SOURCE_DIR}/libs/armeabi/libyuv.so)

add_library( # 库名称
             record_util

             # 动态库，生成so文件
             SHARED

             # 源码
             ${CPP_DIR}/ffmpeg/cmdutils.c
             ${CPP_DIR}/ffmpeg/ffmpeg.c
             ${CPP_DIR}/ffmpeg/ffmpeg_filter.c
             ${CPP_DIR}/ffmpeg/ffmpeg_opt.c
             ${CPP_DIR}/RecordUtil.cpp
             ${CPP_DIR}/audio/AACEncoder.cpp
             ${CPP_DIR}/video/H264Encoder.cpp
             ${CPP_DIR}/RtmpStreamer.cpp
              )


add_library( # 库名称
             rtmp_lib

             # 动态库，生成so文件
             SHARED

             # 源码
             ${CPP_DIR}/ffmpeg/cmdutils.c
             ${CPP_DIR}/ffmpeg/ffmpeg.c
             ${CPP_DIR}/ffmpeg/ffmpeg_filter.c
             ${CPP_DIR}/ffmpeg/ffmpeg_opt.c
             ${CPP_DIR}/RtmpLib.cpp
             ${CPP_DIR}/audio/AACEncoder.cpp
             ${CPP_DIR}/video/H264Encoder.cpp
             ${CPP_DIR}/RtmpStreamer.cpp
              )

add_library( # 库名称
           native_crash_lib

           # 动态库，生成so文件
           SHARED

           # 源码
           ${CPP_DIR}/util/NativeCrashHandler.cpp
            )

target_link_libraries(
                       record_util
                       avcodec
                       avfilter
                       avformat
                       avutil
                       swresample
                       swscale
                       yuv
                       ${log-lib} )

target_link_libraries(
                       rtmp_lib
                       avcodec
                       avfilter
                       avformat
                       avutil
                       swresample
                       swscale
                       yuv
                       ${log-lib} )

target_link_libraries(
                     native_crash_lib
                     ${log-lib} )

