HOMEWORK 4: DVD LISTS

NAME: Ryan Lin
EMAIL: linr2@rpi.edu


COLLABORATORS AND OTHER RESOURCES:
List the names of everyone you talked to about this assignment
(classmates, TAs, ALAC tutors, upperclassmen, students/instructor via
LMS, etc.), and all of the resources (books, online reference
material, etc.) you consulted in completing this assignment.

http://www.cplusplus.com/reference/list/list/
http://www.cplusplus.com/reference/cstdlib/
http://stackoverflow.com/questions/3387269/convert-int-to-stdstring


Remember: Your implementation for this assignment must be done on your
Maown, as described in "Academic Integrity for Homework" handout.


ESTIMATE OF # OF HOURS SPENT ON THIS ASSIGNMENT:  8


ORDER NOTATION:
For each function, using the variables:
  d = number of different movie DVD titles
  k = average or maximum copies of each DVD
  c = number of customers
  p = average or maximum movies ranked on each customer’s preference list
Include a short description of your order notation analysis.


dvd: O(d)
  - add_to_inventory() is called, which calls find_dvd() which iterates through all the inventory items, which is O(d)

customer: O(pd+c)
  - need to lookup customer to make sure he doesn't already exist, which is O(c)
  - need to iterate over his preferences to add them using a loop, which is O(p)
  - inside of the loop mentioned above, need to lookup movie to make sure it exists, which is O(d)

add_preference: O(c+d+p)
  - first customer needs to be looked up, which is O(c)
  - then movie needs to be looked up, which is O(d)
  - then when calling customer->add_preference(), you need to check if preference already exists or not, which is O(p)

ship: O(cpd)
  - in the top loop, customers is iterated over which is O(c)
  - in the sub-loop inside the top loop, preferences is iterated over which is O(p)
  - in the sub-sub-loop inside the sub-loop, need to look up the item in inventory which is O(d)
  - everything else (including list::erase) is constant time O(1)

return_oldest: O(c+d)
  - need to lookup customer name and movie, which are O(c) and O(d) respectively
  - everything else is constant time

return_newest: O(c+d)
  - same as return_oldest, both options work exactly the same way (exactly same code, just one different function which is O(1))

print_customer: O(c+p)
  - first the customer needs to be looked up, which means find_customer() needs to be called, which is O(c)
  - then his/her preference list needs to be iterated through in order to print all the movies on that list, which is O(p) 

print_dvd: O(d)
  - need to search for the DVD in the inventory first, which means find_DVD() is called, which is O(d)


EXTRA CREDIT:
Describe the analysis you performed on customer satisfaction.  Paste
in a small amount of sample analysis output and/or specify a sample
output file.  Describe your improved algorithm and compare the
analysis results of your new algorithm.  Please be concise!

I didn't do any of the extra credit. 



MISC. COMMENTS TO GRADER:  

Please be more lenient when grading the output of the hidden test case. I noticed that
sometimes there's an extra line inserted that "shifts" everything down by one and makes
everything below that point wrong. 

Thanks. 