@echo off
g++ main.cpp image.cpp -o image.exe
rm output/*

:: Testing uncompress on provided car image data
echo Uncompressing car (provided files):
image.exe uncompress files/car_occupancy.pbm files/car_hash_data.ppm files/car_offset.offset output/car.ppm
image.exe compare files/car_original.ppm output/car.ppm output/compare.pbm

:: Testing compress+uncompress on car image
echo Compressing car:
image.exe compress files/car_original.ppm output/car_occupancy.pbm output/car_hash_data.ppm output/car_offset.offset
diff output/car_occupancy.pbm files/car_occupancy.pbm
image.exe uncompress output/car_occupancy.pbm output/car_hash_data.ppm output/car_offset.offset output/car.ppm
image.exe compare files/car_original.ppm output/car.ppm output/compare.pbm

:: Testing compress+uncompress on lightbulb image
echo Compressing lightbulb:
image.exe compress files/lightbulb.ppm output/lightbulb_occupancy.pbm output/lightbulb_hash_data.ppm output/lightbulb_offset.offset
image.exe uncompress output/lightbulb_occupancy.pbm output/lightbulb_hash_data.ppm output/lightbulb_offset.offset output/lightbulb.ppm
image.exe compare files/lightbulb.ppm output/lightbulb.ppm output/compare.pbm

:: Testing compress+uncompress on chair image 
echo Cmopressing chair:
image.exe compress files/chair.ppm output/chair_occupancy.pbm output/chair_hash_data.ppm output/chair_offset.offset
image.exe uncompress output/chair_occupancy.pbm output/chair_hash_data.ppm output/chair_offset.offset output/chair.ppm
image.exe compare files/chair.ppm output/chair.ppm output/compare.pbm

:: Clean up the files
rm image.exe
rm output/compare.pbm