@echo off
g++ *.cpp -g -o graph.exe
gdb graph.exe
rm graph.exe