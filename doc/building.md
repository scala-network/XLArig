### Ubuntu 16.04 Xenial Xerus

```
sudo apt-get install git build-essential cmake libuv1-dev libmicrohttpd-dev libssl-dev libhwloc-dev
git clone https://github.com/scala-network/XLArig
cd XLArig
mkdir build
cd build
cmake ..
make
```

### Windows (MSYS2 64 bit)

Install MSYS2 dependencies 

```
pacman -Sy
pacman -S git
pacman -S mingw-w64-x86_64-gcc
pacman -S make
pacman -S mingw-w64-x86_64-cmake
pacman -S mingw-w64-x86_64-pkg-config
```
Download xmrig-deps from https://github.com/xmrig/xmrig-deps/releases and put it in C:
```
git clone https://github.com/scala-network/XLArig
cd XLARig
mkdir build
cd build
cmake .. -G "Unix Makefiles" -DXMRIG_DEPS=c:/xmrig-deps/gcc/x64
make
```