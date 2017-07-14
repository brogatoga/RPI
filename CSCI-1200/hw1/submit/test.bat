@echo off
g++ main.cpp -o justify.exe
echo --- Running tests --- 

echo Testing example 16w flush left: 
justify.exe example.txt output.txt 16 flush_left
diff output.txt example_16_flush_left.txt 
 
echo Testing example 16w flush right:
justify.exe example.txt output.txt 16 flush_right
diff output.txt example_16_flush_right.txt

echo Testing example 16w full justify:
justify.exe example.txt output.txt 16 full_justify
diff output.txt example_16_full_justify.txt

echo Testing GA 70w flush left:
justify.exe gettysburg_address.txt output.txt 70 flush_left
diff output.txt gettysburg_address_70_flush_left.txt

echo Testing GA 70w flush right:
justify.exe gettysburg_address.txt output.txt 70 flush_right
diff output.txt gettysburg_address_70_flush_right.txt

echo Testing GA 70w full justify:
justify.exe gettysburg_address.txt output.txt 70 full_justify
diff output.txt gettysburg_address_70_full_justify.txt

echo Testing long word 15w full justify:
justify.exe long_word.txt output.txt 15 full_justify
diff output.txt long_word_15_full_justify.txt


echo --- Testing is complete ---
rm output.txt
rm justify.exe