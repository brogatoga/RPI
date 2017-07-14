@echo off
g++ main.cpp board.cpp -g -o robots.exe
gdb robots.exe
rm robots.exe