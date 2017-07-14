HOMEWORK 2: HOCKEY CLASSES


NAME:  Ryan Lin
EMAIL: linr2@rpi.edu


COLLABORATORS AND OTHER RESOURCES:
List the names of everyone you talked to about this assignment
(classmates, TAs, ALAC tutors, upperclassmen, students/instructor via
LMS, etc.), and all of the resources (books, online reference
material, etc.) you consulted in completing this assignment.

http://www.cprogramming.com/tutorial/enum.html
http://www.cplusplus.com/reference/string/string/
http://www.cplusplus.com/reference/vector/vector/
http://www.cplusplus.com/doc/tutorial/other_data_types/
http://en.cppreference.com/w/cpp/string/basic_string/to_string
http://stackoverflow.com/questions/1313988/c-what-is-the-optimal-way-to-convert-a-double-to-a-string
http://www.cplusplus.com/reference/cmath/floor/


Remember: Your implementation for this assignment must be done on your
own, as described in "Academic Integrity for Homework" handout.


ESTIMATE OF # OF HOURS SPENT ON THIS ASSIGNMENT:  12



DESCRIPTION OF 3RD STATISTIC:
Please be concise! (< 200 words)

1. What question you are trying to answer with your statistic? 
A: I am trying to figure out which violations happen the most and least frequently, 
   and a list of all the violations that have come up in a series of hockey games. 

2. What data did you need to organize? 
A: I needed to keep track of every single game and iterate through their actions to look
   for penalties. If the action was a penalty, then I recorded the violation and its respective
   frequency in a parallel array. 


3. What was interesting or challenging about the implementation of this statistic? 
A: The fact that I had to create both a vector of strings to hold the violation names, as well
   as a vector of integers to hold their frequencies. Most importantly though, I had to use these two
   vectors as parallel data structures (since we don't have hash tables yet) and figure out a way
   to sort them effectively and efficiently. 



NAME OF FILE WITH SAMPLE OUTPUT FROM 3RD STATISTIC: custom_statistic_2012_all_output.txt
Be sure to select (or create) a dataset with interesting results


custom_statistic_2012_all_output.txt



MISC. COMMENTS TO GRADER:  

1. The only long function I have is readData() which is a large switch statement to implement 
an FSM-like design for the input parser. I didn't manage to find any easy way to break this
up into shorter functions except to use function pointers (which we haven't learned yet), so
I'd appreciate if you don't take points off for that.


2. Can you please give me more partial credit on 2012_all.txt and the hidden test if possible? 
I have a feeling that sometimes if I'm off by one line (like everything is shifted down one), then all
lines below that point are marked as incorrect. 