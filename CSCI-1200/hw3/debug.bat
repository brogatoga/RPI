@echo off
g++ main.cpp -g -o table.exe
gdb table.exe
rm table.exe