@echo off
g++ main.cpp -o dvd.exe

echo Testing requests_medium:
dvd.exe inputs/requests_medium.txt output.txt