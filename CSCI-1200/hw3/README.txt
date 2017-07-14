HOMEWORK 3: RESIZABLE TABLE


NAME:  Ryan Lin


COLLABORATORS AND OTHER RESOURCES:
List the names of everyone you talked to about this assignment
(classmates, TAs, ALAC tutors, upperclassmen, students/instructor via
LMS, etc.), and all of the resources (books, online reference
material, etc.) you consulted in completing this assignment.

http://code.google.com/p/drmemory/downloads/list


Remember: Your implementation for this assignment must be done on your
own, as described in "Academic Integrity for Homework" handout.


ESTIMATE OF # OF HOURS SPENT ON THIS ASSIGNMENT:  8



ORDER NOTATION:

Write each answer in terms of n = the number of rows and m = the
number of columns.  You should assume that calling new [] or delete []
on an array will take time proportional to the number of elements in
the array.

Note: The order notation of (push|pop)_back_(row|column) depends primarily
on resize_(rows|cols). These four functions only change the row or column
size by one, so endRow-startRow == 1 for these cases. Then, the order notation
of the resize functions would be:
	resize_rows --- O(n+n+m+n) = O(3n+m) = O(m+n)
	resize_cols --- O(n*(m+m+m)) = O(n*(3m)) = O(3mn) = O(mn)

get --- O(1)

set --- O(1)

numRows --- O(1)

numColumns --- O(1)

push_back_row --- O((m+n) + m) = O(2m+n) = O(m+n)

push_back_column --- O((mn) + n) = O(mn + n) = O(mn)

pop_back_row --- O(m+n)
	- since this function calls resize_rows(rows-1)

pop_back_column --- O(mn)
	- since this function calls resize_cols(cols-1)

print --- O(mn)


TESTING & DEBUGGING STRATEGY: 
Discuss your strategy for testing & debugging your program.  
What tools did you use (gdb/lldb/Visual Studio debugger,
Valgrind/Dr. Memory, std::cout & print, etc.)?  How did you test the
"corner cases" of your Table class design & implementation?

I used gdb to help me find two bugs in my code by setting breakpoints and
then stepping through the code. I also used drmemory to help me check for 
memory leaks in case I forgot to free some memory. I tested the corner
cases of my Table class (such as empty table, 1x1 table, etc) by writing
code in the test functions in main.cpp to initialize these tables, call
functions on them, and make sure that nothing crashed or went wrong. 


EXTRA CREDIT: 
Indicate here if you implemented push_back for multiple rows or
multiple columns and/or rewrote your code using pointers instead of
subscripts for extra credit.  Also document the order notation for
your multi-row and multi-column push_back functions.

The order notations of these two functions depends primarily on the size of
the tables that are being added to the current table. 

1. I implemented both push_back_row and push_back_col for adding multiple
rows and columns, given a Table<T> as the argument. Their order notations are:

push_back_rows --- O((n+m) + (mn)) = O(mn)
push_back_columns --- O((mn) + (mn)) = O(2mn) = O(mn)

2. I also rewrote my code to use pointers and pointer arithmetic instead
of subscripts. However if you want to also look at the original code in
subscript form, I made a folder called "no_pointer_arithmetic" that contains
this code. 



MISC. COMMENTS TO GRADER:  

Hello. 