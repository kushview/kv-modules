#!/bin/bash
here=`pwd`
top=`dirname ${here}`
libdir="${top}/modules/kv_ffmpeg/libs/MacOSX/x86_64"
incdir="${top}/modules/kv_ffmpeg/include"

set -ex

mkdir -p build/ffmpeg && rm -rf build/ffmpeg/*
cd build/ffmpeg
../../ffmpeg-3.3.3/configure \
    --disable-gpl \
    --enable-shared \
    --enable-static \
    --disable-doc \
    --prefix="${here}/dist" \
    --libdir="${libdir}" --shlibdir="${libdir}" \
    --incdir="${top}/modules/kv_ffmpeg/include" \
    && make -j8 && make install

cd "${here}"
rm -rf build/ffmpeg
exit 0
