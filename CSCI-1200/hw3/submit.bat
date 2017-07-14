@echo off
zip hw3.zip main.cpp table.h table.hpp README.txt *.bat no_pointer_arithmetic/*
rm submit/hw3.zip
mv hw3.zip submit/hw3.zip
echo Submission created!