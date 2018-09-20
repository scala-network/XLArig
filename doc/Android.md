# Android
XTLRig supports compilation and mining with an android cpu.  Please contact us via discord https://discord.gg/DVPJ5uD for any issues. 

## Build
For android build make sure you have ndk installed. You can read about ndk [here](https://developer.android.com/ndk/).

### Steps to build

* Go to android folder
* Run <NDK_ROOT>/ndk-build 
* Thats all. All compiled binaries will be located under /android/libs folder


## Pools
Below are pools that have low difficulty which is appropriate for mobile mining (less than 1000 difficulty).

| URL | Starting Difficulty |
|----------------------------------|---------------------|
| stratum.xtlpool.com:3333 | 1000 |
| dearmon.zone:8988 | 50 |
| xtl.pool.gntl.co.uk:2222 | 1000 |
| stellite.almsoft.net:3333 | 1000 |
| xtl.superpools.online:33333 | 500 |
| Pool.XTL.CryptoPool.Space | 500 |
| communitypool.stellite.cash:6677 | 1000 |
| Mine.Stellite.Cash:80 | 500 |


## How to use (via cli)

1. Compile base on platform architechure. Currently support
 * arm64-v8a
 * armeabi-v7a
 * x86
 
 2. Install adb. More info on adb and to install
 * https://developer.android.com/studio/command-line/adb
 * https://www.xda-developers.com/install-adb-windows-macos-linux/
 
 3. Once you have adb installed and your android plugged onto your computer. Check if your device is connected. You can list connected devices by 
 ```
 adb devices
 ```
 
 3. By using adb put your selected xtlandrig binary into /data/local/tmp. You can do so as below
```
adb push xtlrig /data/local/tmp/xtlrig
```

 4. Next execute the binary on your device. To do this you have to go to shell mode by 
 ```
 adb shell
 ```
 
 5. In shell mode go to your binary placed location ```cd /data/local/tmp``` and execute ```./xtlrig --help```. If you see the menu then your miner is ready.
 
 6. Running the binary requires 3 major things. 
    * Pool
    * XTL Address
    * Password
    
    Execute the binary ```./xtlrig -o <pool address> -u <xtl_address> -p <your_worker_id> --variant xtl```
    
    Where <pool address> is the full pool address and <xtl_address> is you full xtl address without < and >.
 
    Eg. 
    
```./xtlrig -o xtlpool.com:3333 -u Se4FFaA4n89epNPA7bXgzaFBup9a4wDABbYsEQXDWGiFNdbnwgmBoLgjXSX7ZHSnpCcie1uMmEZ7K2xaVbdsyxkc32AEBDr1p -p MyAndroid --variant xtl```
 
 ## Issues
 1. I ran ```./xtlrig --help``` and menu doesn't come out but throws ```-bash: ./xtlrig: Permission denied```
 
 You might not have the binary as executed binary. Try running ```chmod +x xtlrig``` to make it executable.
 
 2. I ran ```./xtlrig --help``` and menu doesn't come out but throws ```-bash: ./xtlrig: cannot execute binary file```

You might be using the wrong platform. Try other binaries compiled. 
