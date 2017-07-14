@echo off
rm results/*.txt
g++ main.cpp board.cpp -o robots.exe

:: Failing:
:: puzzle7 reachability (takes too long)
:: 

:: Reachability tests for all puzzles (PASSED)
:reach_test

echo "Testing reachability for puzzle #1:"
robots.exe inputs/puzzle1.txt -visualize A -max_moves 3 > results/puzzle1_visualize_A_3.txt
diff results/puzzle1_visualize_A_3.txt outputs/puzzle1_reachable_A_3.txt

robots.exe inputs/puzzle1.txt -visualize B -max_moves 3 > results/puzzle1_visualize_B_3.txt
diff results/puzzle1_visualize_B_3.txt outputs/puzzle1_reachable_B_3.txt

robots.exe inputs/puzzle1.txt -visualize C -max_moves 3 > results/puzzle1_visualize_C_3.txt
diff results/puzzle1_visualize_C_3.txt outputs/puzzle1_reachable_C_3.txt

robots.exe inputs/puzzle1.txt -visualize A -max_moves 10 > results/puzzle1_visualize_A.txt
diff results/puzzle1_visualize_A.txt outputs/puzzle1_reachable_A.txt

echo "Testing reachability for puzzle #4:"
robots.exe inputs/puzzle4.txt -visualize A -max_moves 9 > results/puzzle4_visualize_A.txt

echo "Testing reachability for puzzle #6 (robot A):"
robots.exe inputs/puzzle6.txt -visualize A -max_moves 10 > results/puzzle6_visualize_A.txt

echo "Testing reachability for puzzle #6 (robot B):"
robots.exe inputs/puzzle6.txt -visualize B -max_moves 10 > results/puzzle6_visualize_B.txt

echo "Testing reachability for puzzle #7 (robot S):"	
robots.exe inputs/puzzle7.txt -visualize S -max_moves 10 > results/puzzle7_visualize_S.txt


:puzzle_test_1 :: (PASSED)
echo "Testing puzzle #1"
robots.exe inputs/puzzle1.txt -max_moves 8 > results/puzzle1.txt
robots.exe inputs/puzzle1.txt -max_moves 8 -all_solutions > results/puzzle1_all.txt
diff results/puzzle1.txt outputs/puzzle1_one_solution.txt
diff results/puzzle1_all.txt outputs/puzzle1_all_solutions.txt


:puzzle_test_2 :: (PASSED)
echo "Testing puzzle #2"
robots.exe inputs/puzzle2.txt -max_moves 8 -all_solutions > results/puzzle2_all.txt
robots.exe inputs/puzzle2.txt -max_moves 4 > results/puzzle_none.txt
diff results/puzzle2_all.txt outputs/puzzle2_all_solutions.txt
diff results/puzzle2_none.txt outputs/puzzle2_no_4_move_solutions.txt


:puzzle_test_3  
::robots.exe inputs/puzzle3.txt -max_moves 11 > results/puzzle3.txt
::robots.exe inputs/puzzle3.txt -max_moves 8 -all_solutions > results/puzzle3_all.txt


:puzzle_test_4 :: (PASSED)
echo "Testing puzzle #4"
robots.exe inputs/puzzle4.txt -max_moves 12 -all_solutions > results/puzzle4_all.txt


:puzzle_test_5 :: (PASSED)
echo "Testing puzzle #5"
robots.exe inputs/puzzle5.txt > results/puzzle5.txt


:puzzle_test_7 :: (SKIPPED)
echo "Testing puzzle #7"
robots.exe inputs/puzzle7.txt -max_moves 10 -all_solutions > results/puzzle7.txt > results/puzzle7_all.txt


:end
rm robots.exe