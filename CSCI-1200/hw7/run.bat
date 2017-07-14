@echo off
g++ main.cpp -o map.exe

:: Standard tests
map.exe < inputs/spot_input.txt > outputs/my_spot_output.txt
map.exe < inputs/hg_input_2.txt > outputs/my_hg_output_2.txt
map.exe < inputs/hg_input_3.txt > outputs/my_hg_output_3.txt


:: Custom tests for Shakespeare


:: Custom tests for Wizard of Oz

