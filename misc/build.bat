@echo off
call "C:\Program Files (x86)\Microsoft Visual Studio\2017\Community\VC\Auxiliary\Build\vcvars64.bat"
rmdir /S /Q build
del %~dp0\xlarig-%1-win64.zip
mkdir build &&^
cd build &&^
git clone https://github.com/MoneroOcean/xlarig.git &&^
git clone https://github.com/xlarig/xlarig-deps.git &&^
mkdir xlarig\build &&^
cd xlarig\build &&^
git checkout %1 &&^
cmake .. -G "Visual Studio 15 2017 Win64" -DXMRIG_DEPS=%~dp0\build\xlarig-deps\msvc2017\x64 &&^
msbuild /p:Configuration=Release xlarig.sln &&^
cd Release &&^
copy ..\..\src\config.json . &&^
7z a -tzip -mx %~dp0\xlarig-%1-win64.zip xlarig.exe config.json &&^
cd %~dp0 &&^
rmdir /S /Q build