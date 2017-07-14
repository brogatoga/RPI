HOMEWORK 7: WORD FREQUENCY MAPS

NAME: Ryan Lin
EMAIL: linr2@rpi.edu


COLLABORATORS AND OTHER RESOURCES:
List the names of everyone you talked to about this assignment
(classmates, TAs, ALAC tutors, upperclassmen, students/instructor via
LMS, etc.), and all of the resources (books, online reference
material, etc.) you consulted in completing this assignment.

http://www.cplusplus.com/reference/map/map/
http://www.cplusplus.com/reference/vector/vector/
http://www.cplusplus.com/reference/string/string/
http://www.cplusplus.com/reference/cstdlib/srand/
ALAC @ SAGE 5510


ESTIMATE OF # OF HOURS SPENT ON THIS ASSIGNMENT:  8 hours


ANALYSIS OF PERFORMANCE OF YOUR ALGORITHM:
(order notation & concise paragraph, < 200 words)

n = total number of words in the sample text file
m = number of unique words in the file
w = width of the sequencing window
p = average number of words observed to follow a particular word

How much memory will the map data structure require, in terms of n, m,
w, and p (order notation for memory use)?

Memory Used: O(m*p^w)
Explanation: Suppose that we have a map A. If we have m unique words, then we will need O(m) memory
to store the keys in A->first. Now we know that each word has an average of p words following it, so if
we have m words, then we will have mp words in the "inner" submap. Then for each of the mp words in the
inner submap, they will have p words as well. So, if there is a submap inside of that too
(which will be the case if window_size > 2), then that submap will have mp^2 words. And so on and so forth. 
We know that a window size of w will have w maps total, so therefore the final answer is O(m*p^w) total memory used. 


What is the order notation for performance (running time) of each of
the commands?

load -- O(n log n)
First we need to read n words from the text file -- O(n)
Every time we read in a word, we have to add/update each of the three maps -- O(3 log n) = O(log n)

print -- O(p (log m)^w)
First we need to call std::map<T>::find(), and the word-frequency maps contain m elements, so this is O(log m)
Then we need to call find() again to get the sub-maps, for a total of w-1 times, so approximately O((log m)^w)
Then we have to iterate through the frequencies at the last map, which is O(p)

generate random -- O(p (log m)^w)
We have to call find() repeatedly to get to the inner-most map, which is O((log m)^w)
Then we have to iterate through once to find the total frequency, then again to get random word, which is O(2p) = O(p)

generate most_common -- O(p (log m)^w)
The runtime is the same as generating random words, except that we only iterate through the inner map once to
calculate the max frequency, instead of twice with random numbers. So the runtime would then be O(p)

quit -- O(1)
The "break" statement exits the loop which then leads to end of the program. This is a constant time operation. 


EXTRA CREDIT
Parsing & using punctuation data, implementation supports *any*
window size, new test cases (describe & summarize performance, but
don't include large datasets with submission).

Didn't do the extra credit.


MISC. COMMENTS TO GRADER:

None.