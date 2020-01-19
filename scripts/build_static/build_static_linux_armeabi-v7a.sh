#!/bin/bash

## Script made for Debian 10 (AARCH64 VPS).  You'll need to adapt the packages to your OS.
 
apt -y install build-essential cmake clang git openssl curl wget libuv1-dev libssl-dev libhwloc-dev automake libtool autoconf zip

git clone https://github.com/scala-network/XLArig xlarig --depth 1

rm -rf xlarig/build
mkdir xlarig/build
cd xlarig/scripts
chmod u+x build_deps.sh
./build_deps.sh

cd ../build

cmake .. -DWITH_HWLOC=OFF -DBUILD_STATIC=ON -DXMRIG_DEPS=scripts/deps -DARM_TARGET=7
make -j$(nproc)
strip xlarig
mv xlarig ~