@echo off
echo Building mingw32 executable and running memory tests...
i686-pc-mingw32-g++.exe -static-libgcc -static-libstdc++ -ggdb -o graph.exe *.cpp

drmemory -brief -- graph.exe inputs/graph_test.txt
drmemory -brief -- graph.exe inputs/message_test.txt
drmemory -brief -- graph.exe inputs/remove_test.txt
drmemory -brief -- graph.exe inputs/custom_test.txt

rm graph.exe