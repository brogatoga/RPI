@echo off
g++ *.cpp -g -o train.exe
gdb train.exe
rm train.exe