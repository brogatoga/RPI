@echo off
i686-pc-mingw32-g++.exe -static-libgcc -static-libstdc++ -ggdb -o lab.exe lab7.cpp
drmemory -brief -- lab.exe
rm lab.exe