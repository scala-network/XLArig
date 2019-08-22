
# XLArig

XLArig is a high performance Scala (XLA) CPU miner, with official support for Windows. Forked from XMRig for Monero and other cryptonight currencies.

<img src="https://i.imgur.com/g4DrWXl.png" width="670" >

#### Table of contents

* [Features](#features)
* [Download](#download)
* [Usage](#usage)
* [Algorithm variations](#algorithm-variations)
* [Build](#Building)
* [Common Issues](#common-issues)
* [Other information](#other-information)
* [Donations](#donations)
* [Release checksums](#release-checksums)
* [Contacts](#contacts)

## Features
* High performance.
* Official Windows support.
* Small Windows executable, without dependencies.
* x86/x64 support.
* Support for backup (failover) mining server.
* keepalived support.
* Command line options compatible with cpuminer.
* Smart automatic CPU configuration.
* It's open source software.

## Download
* Binary releases: https://github.com/scala-network/xlarig/releases


## Usage
On windows
```
xlarig.exe
```
on linux 
```
./xlarig
```

### Options
```
  -a, --algo=ALGO          specify the algorithm to use
                             cryptonight
                             cryptonight-lite
                             cryptonight-heavy
  -o, --url=URL            URL of mining server
  -O, --userpass=U:P       username:password pair for mining server
  -u, --user=USERNAME      username for mining server
  -p, --pass=PASSWORD      password for mining server
      --rig-id=ID          rig identifier for pool-side statistics (needs pool support)
  -t, --threads=N          number of miner threads
  -v, --av=N               algorithm variation, 0 auto select
  -k, --keepalive          send keepalived packet for prevent timeout (needs pool support)
      --nicehash           enable nicehash.com support
      --tls                enable SSL/TLS support (needs pool support)
      --tls-fingerprint=F  pool TLS certificate fingerprint, if set enable strict certificate pinning
  -r, --retries=N          number of times to retry before switch to backup server (default: 5)
  -R, --retry-pause=N      time to pause between retries (default: 5)
      --cpu-affinity       set process affinity to CPU core(s), mask 0x3 for cores 0 and 1
      --cpu-priority       set process priority (0 idle, 2 normal to 5 highest)
      --no-huge-pages      disable huge pages support
      --no-color           disable colored output
      --variant            algorithm PoW variant
      --user-agent         set custom user-agent string for pool
  -B, --background         run the miner in the background
  -c, --config=FILE        load a JSON-format configuration file
  -l, --log-file=FILE      log all output to a file
  -S, --syslog             use system log for output messages
      --max-cpu-usage=N    maximum CPU usage for automatic threads mode (default 75)
      --safe               safe adjust threads and av settings for current CPU
      --asm=ASM            ASM code for cn/2, possible values: auto, none, intel, ryzen.
      --print-time=N       print hashrate report every N seconds
      --api-port=N         port for the miner API
      --api-access-token=T access token for API
      --api-worker-id=ID   custom worker-id for API
      --api-id=ID          custom instance ID for API
      --api-ipv6           enable IPv6 support for API
      --api-no-restricted  enable full remote access (only if API token set)
      --dry-run            test configuration and exit
  -h, --help               display this help and exit
  -V, --version            output version information and exit
```

Also you can use configuration via config file, default name **config.json**. Some options available only via config file: [`autosave`](https://github.com/xlarig/xlarig/issues/767), [`hw-aes`](https://github.com/xlarig/xlarig/issues/563). `watch` option currently not implemented in miners only in proxy.

## Algorithm variations

- `av` option used for automatic and simple threads mode (when you specify only threads count).
- For [advanced threads mode](https://github.com/xlarig/xlarig/issues/563) each thread configured individually and `av` option not used.

| av | Hashes per round | Hardware AES |
|----|------------------|--------------|
| 1  | 1 (Single)       | yes          |
| 2  | 2 (Double)       | yes          |
| 3  | 1 (Single)       | no           |
| 4  | 2 (Double)       | no           |
| 5  | 3 (Triple)       | yes          |
| 6  | 4 (Quard)        | yes          |
| 7  | 5 (Penta)        | yes          |
| 8  | 3 (Triple)       | no           |
| 9  | 4 (Quard)        | no           |
| 10 | 5 (Penta)        | no           |

## Common Issues
### HUGE PAGES unavailable
* Run XLArig as Administrator.
* Since version 0.8.0 XLArig automatically enables SeLockMemoryPrivilege for current user, but reboot or sign out still required.

## Other information
* No HTTP support, only stratum protocol support.
* Donation free miner.

### CPU mining performance
* **Intel i7-7700** - 307 H/s (4 threads)
* **AMD Ryzen 7 1700X** - 560 H/s (8 threads)

Please note performance is highly dependent on system load. The numbers above are obtained on an idle system. Tasks heavily using a processor cache, such as video playback, can greatly degrade hashrate. Optimal number of threads depends on the size of the L3 cache of a processor, 1 thread requires 2 MB of cache.

### Maximum performance checklist
* Idle operating system.
* Do not exceed optimal thread count.
* Use modern CPUs with AES-NI instruction set.
* Try setup optimal cpu affinity.
* Enable fast memory (Large/Huge pages).

## Donations
The donation mining in XLArig has been turned off, if you wish to help the original creators or the Scala Project team donate to the addresses below.

* XMR: `48edfHu7V9Z84YzzMa6fUueoELZ9ZRXq9VetWzYGzKt52XU5xvqgzYnDK9URnRoJMk1j8nLwEVsaSWJ4fhdUyZijBGUicoD`
* BTC: `1P7ujsXeX7GxQwHNnJsRMgAdNkFZmNVqJT`
* XLA: `SEiTBcLGpfm3uj5b5RaZDGSUoAGnLCyG5aJjAwko67jqRwWEH26NFPd26EUpdL1zh4RTmTdRWLz8WCmk5F4umYaFByMtJT6RLjD6vzApQJWfi`

## Building 

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


## Contacts
* hello@scalaproject.io