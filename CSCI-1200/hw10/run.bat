@echo off
g++ *.cpp -o polygons.exe

::polygons.exe input/input.txt output/input_result.txt
polygons.exe input/quads.txt output/quads_result.txt
polygons.exe input/triangles.txt output/triangles_result.txt

::diff output/quads_result.txt output/quads_output.txt
diff output/triangles_result.txt output/triangles_output.txt

rm polygons.exe