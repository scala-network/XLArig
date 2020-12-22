@echo off
cd %~dp0
xlarig.exe --bench=10M --submit
pause
