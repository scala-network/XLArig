### Ubuntu 

We will use the `build_deps.sh` script to build the most recent versions of libuv, openssl and hwloc as static libraries.

```
sudo apt-get install git build-essential cmake automake libtool autoconf
git clone https://github.com/scala-network/XLArig
mkdir xlarig/build && cd xlarig/scripts
sudo chmod u+x build*
./build_deps.sh && cd ../build
cmake .. -DXMRIG_DEPS=scripts/deps -DBUILD_STATIC=ON
make -j$(nproc)
```

### Windows (MSYS2 64 bit)

Prerequisite : Download xmrig-deps from https://github.com/xmrig/xmrig-deps (click on the green button "code" and download the repo as an archive) and extract the content into `C:\xmrig-deps`

```
pacman -Syu
pacman -S git mingw-w64-x86_64-gcc make mingw-w64-x86_64-cmake mingw-w64-x86_64-pkg-config 
git clone https://github.com/scala-network/XLArig
mkdir xlarig/build && cd xlarig/build
cmake .. -G "Unix Makefiles" -DXMRIG_DEPS=c:/xmrig-deps/gcc/x64
make -j$(nproc)
```

### MacOS (Intel CPUs)

You'll need to install Homebrew first. 

If it is already installed, please do a `brew update` then a `brew upgrade` to update all dependencies to their latest version.

```
brew install cmake wget automake libtool autoconf
git clone https://github.com/scala-network/XLArig
mkdir xlarig/build && cd xlarig/scripts
sudo chmod u+x build*
./build.hwloc.sh && cd ../build
cmake .. -DOPENSSL_ROOT_DIR=/usr/local/opt/openssl -DHWLOC_INCLUDE_DIR=../scripts/deps/include -DHWLOC_LIBRARY=../scripts/deps/lib/libhwloc.a
make -j$(sysctl -n hw.logicalcpu)
```
