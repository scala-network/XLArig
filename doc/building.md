### Ubuntu 

```
sudo apt-get install git build-essential cmake automake libtool autoconf libhwloc-dev libuv1-dev
git clone https://github.com/scala-network/xlarig
mkdir xlarig/build && cd xlarig/build
cmake ..
make -j$(nproc)
```

### Arch Linux


```
pacman -Sy
pacman -S --needed base-devel hwloc openssl cmake libmicrohttpd git
git clone https://github.com/scala-network/xlarig
cd xlarig/
mkdir build && cd build
cmake ..
make -j$(nproc)
```


### Windows (MSYS2 64 bit)

Prerequisite : Download xmrig-deps from https://github.com/xmrig/xmrig-deps (click on the green button "code" and download the repo as an archive) and extract the content into `C:\xmrig-deps`

```
pacman -Syu
pacman -S git mingw-w64-x86_64-gcc make mingw-w64-x86_64-cmake mingw-w64-x86_64-pkg-config 
git clone https://github.com/scala-network/xlarig
mkdir xlarig/build && cd xlarig/build
cmake .. -G "Unix Makefiles" -DXMRIG_DEPS=c:/xmrig-deps/gcc/x64
make -j$(nproc)
```

### MacOS (Intel CPUs)

You'll need to install Homebrew first. 

If it is already installed, please do a `brew update` then a `brew upgrade` to update all dependencies to their latest version.

```
brew install cmake wget automake libtool autoconf
git clone https://github.com/scala-network/xlarig
mkdir xlarig/build && cd xlarig/scripts
./build.hwloc.sh && cd ../build
cmake .. -DOPENSSL_ROOT_DIR=/usr/local/opt/openssl -DHWLOC_INCLUDE_DIR=../scripts/deps/include -DHWLOC_LIBRARY=../scripts/deps/lib/libhwloc.a
make -j$(sysctl -n hw.logicalcpu)
```

### Raspberry Pi 4

Since Raspbian is a 32 bit OS and XLArig is a 64 bit miner, you'll need to be on a 64-bit container if you don't want to get a segmentation fault after starting it.
If the command `ds64-shell` doesn't work, here's the command to install that package : ``sudo apt-get install -y raspbian-nspawn-64``

```
sudo apt-get install git build-essential cmake libuv1-dev libssl-dev libhwloc-dev
git clone https://github.com/scala-network/xlarig
mkdir xlarig/build && cd xlarig/build
cmake ..
make -j$(nproc)
```
