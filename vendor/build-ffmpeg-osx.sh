#!/bin/bash
here=`pwd`
top=`dirname ${here}`
libdir="${top}/modules/kv_ffmpeg/libs/MacOSX/x86_64"
incdir="${top}/modules/kv_ffmpeg/include"
bindir="${top}/modules/kv_ffmpeg/bin"

set -e

mkdir -p build/ffmpeg && rm -rf build/ffmpeg/*
rm -rf "${libdir}"
rm -rf "${incdir}"

set -x

cd build/ffmpeg
../../ffmpeg-3.3.3/configure \
    --disable-gpl --enable-shared --disable-static --disable-doc \
    --prefix="${here}/dist" \
    --libdir="${libdir}" --shlibdir="${libdir}" \
    --incdir="${incdir}" --bindir="${bindir}" \
    && make -j8 && make install

cd "${here}"
rm -rf build/ffmpeg
exit 0
