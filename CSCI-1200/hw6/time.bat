@echo off
g++ main.cpp board.cpp -o robots.exe
robots.exe inputs/puzzle5.txt > results/puzzle5.txt
rm robots.exe