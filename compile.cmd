@echo off
if not exist ".\bin" mkdir ".\bin"
pushd ".\bin"
call g++ -std=c++17 -O3 "..\src\main.cpp" -o "tamachi.exe" -luser32 -lgdi32 -lmsimg32
popd