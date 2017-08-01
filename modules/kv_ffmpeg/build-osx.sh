#!/bin/bash

export ARCH="x86_64"

moddir=`pwd`
prefix="${moddir}/libs/MacOSX/$ARCH"
libdir="${moddir}/libs/MacOSX/$ARCH"
bindir="${moddir}/libs/MacOSX/$ARCH"
useropts="--disable-gpl --enable-shared --disable-doc"

if [ ! -d "ffmpeg" ]; then
    echo "FFmpeg is not present. Try running: git submodule update --init"
    exit 1
fi

set -ex

mkdir -p build && rm -rf build/*
rm -rf "${libdir}"

cd build && ../ffmpeg/configure --prefix="${prefix}" $useropts \
    --libdir="${libdir}" --bindir="${libdir}" \
    && make -j4 && make install

cd "${moddir}"
exit 0
