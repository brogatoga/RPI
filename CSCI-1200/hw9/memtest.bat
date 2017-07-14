@echo off
echo Building mingw32 executable and running memory tests...
i686-pc-mingw32-g++.exe -static-libgcc -static-libstdc++ -ggdb -o image.exe *.cpp

drmemory -brief -- image.exe compress files/car_original.ppm output/car_occupancy.pbm output/car_hash_data.ppm output/car_offset.offset
drmemory -brief -- image.exe uncompress output/car_occupancy.pbm output/car_hash_data.ppm output/car_offset.offset output/car.ppm
drmemory -brief -- image.exe compare files/car_original.ppm output/car.ppm output/compare.pbm

rm image.exe
rm output/*

echo Complete!