@echo off
i686-pc-mingw32-g++.exe -static-libgcc -static-libstdc++ -ggdb -o table.exe main.cpp
drmemory -brief -- table.exe
