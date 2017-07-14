@echo off
i686-pc-mingw32-g++.exe -static-libgcc -static-libstdc++ -ggdb -o train.exe main.cpp traincar.cpp
drmemory -brief -- train.exe
rm train.exe
rm *.stackdump