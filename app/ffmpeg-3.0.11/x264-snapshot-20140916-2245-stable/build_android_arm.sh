#!/bin/bash

#SYSROOT=$NDK/platforms/android-16/arch-arm/

NDK=/Users/leegend/Library/Android/sdk/ndk-bundle

PLATFORM=$NDK/platforms/android-16/arch-arm
TOOLCHAIN=$NDK/toolchains/arm-linux-androideabi-4.9/prebuilt/darwin-x86_64
PREFIX=./android/arm

function build_one
{
./configure \
--prefix=$PREFIX \
--disable-shared \
--enable-static \
--disable-asm \
--enable-pic \
--enable-strip \
--host=arm-linux-androideabi \
--cross-prefix=$TOOLCHAIN/bin/arm-linux-androideabi- \
--sysroot=$PLATFORM \
--extra-cflags="-Os -fpic" \
--extra-ldflags="" \

$ADDITIONAL_CONFIGURE_FLAG
make clean
make -j4
make install

}
build_one