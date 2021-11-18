#!/bin/bash -e

LIBRESSL_VERSION="3.2.4"

mkdir -p deps
mkdir -p deps/include
mkdir -p deps/lib

mkdir -p build && cd build

wget https://ftp.openbsd.org/pub/OpenBSD/LibreSSL/libressl-${LIBRESSL_VERSION}.tar.gz -O libressl-${LIBRESSL_VERSION}.tar.gz
tar -xzf libressl-${LIBRESSL_VERSION}.tar.gz

cd libressl-${LIBRESSL_VERSION}
./configure --disable-shared CPPFLAGS="-D_FORTIFY_SOURCE=0" CFLAGS="-static" 
make -j3
cp -fr include ../../deps
cp crypto/.libs/libcrypto.a ../../deps/lib
cp ssl/.libs/libssl.a ../../deps/lib
cd ..