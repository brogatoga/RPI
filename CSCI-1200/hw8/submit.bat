@echo off
rm submit/hw8.zip
cp inputs/custom_test.txt my_input.txt
zip submit/hw8.zip *.cpp *.h README.txt my_input.txt
rm my_input.txt
echo Submission created!