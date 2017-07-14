HOMEWORK 6: RICOCHET ROBOTS RECURSION


NAME: Ryan Lin
EMAIL: linr2@rpi.edu 


COLLABORATORS AND OTHER RESOURCES:
List the names of everyone you talked to about this assignment
(classmates, TAs, ALAC tutors, upperclassmen, students/instructor via
LMS, etc.), and all of the resources (books, online reference
material, etc.) you consulted in completing this assignment.

http://www.cplusplus.com/reference/vector/vector/
http://www.cplusplus.com/reference/cstdlib/


ESTIMATE OF # OF HOURS SPENT ON THIS ASSIGNMENT:  15 hours



ANALYSIS OF PERFORMANCE OF YOUR ALGORITHM:

i & j = dimensions of the board
    r = number of robots on the board
    g = number of goals
    w = number of interior walls
    m = maximum total number of moves allowed

The order notation of my algorithm is O(r^m). 

Explanation: My algorithm tries to recursively move each of the robots in one of four dirctions repeatedly until
it reaches the number of max moves, at which point then the recursive function visualize() will stop recursing and 
start terminating. The order notation for every call to visualize() which is the majority of the algorithm is
O(4r) = O(r). Now each robot can move up to m times and involve other robots as well (but they can't move backwards
or in the same directin that they moved last turn), so the final order notation would be O(r^m). 




SUMMARY OF PERFORMANCE OF YOUR PROGRAM ON THE PROVIDED PUZZLES:
Correctness & approximate wall clock running time for various command
line arguments.

puzzle1.txt -visualize A -max_moves 3 => 1.713 seconds
puzzle1.txt -visualize B -max_moves 3 => 1.752 seconds
puzzle1.txt -visualize C -max_moves 3 => 1.681 seconds
puzzle1.txt -visualize A -max_moves 10 => 12.523 seconds
puzzle1.txt -max_moves 8 => 3.423 seconds
puzzle1.txt -max_moves 8 -all_solutions => 3.423 seconds
puzzle2.txt -all_solutions => 2.352 seconds
puzzle4.txt -max_moves 12 -all_solutions =>	1.712 seconds
puzzle5.txt => 1.758 seconds


MISC. COMMENTS TO GRADER:  

None. 