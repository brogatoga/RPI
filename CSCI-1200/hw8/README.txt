HOMEWORK 8: FRIENDLY RECURSION

NAME: Ryan Lin
EMAIL: linr2@rpi.edu


COLLABORATORS AND OTHER RESOURCES:
http://www.cplusplus.com/reference/list/list/
http://www.cplusplus.com/reference/vector/vector/
http://stackoverflow.com/questions/2432857/sort-list-using-stl-sort-function


ESTIMATE OF # OF HOURS SPENT ON THIS ASSIGNMENT: 6 hours


NETWORK COMPLEXITY
Analysis of the maximum number of people at a party with chain
length n, if each person has at most k friends.

ANSWER: The maximum number of people is equal to SUM(j=0, n, k^j).
EXPLANATION: At first there is only 1 person, which is the host. However, the host has at most k friends, we will call these
	first-order friends. Then these k first-order friends will also have at most k friends each for a total of k^2 second-order
	friends. Then these k^2 second-order friends will also have k friends each, for a total of k^3 third-order friends. And so
	on, and so forth. Then the total number of friends is equal to 1 + k^2 + k^3 + ... + k^n, which is equivalent
	to SUM(j=0, n, k^j), which means that we sum the values of k^j, where 0 <= j <= n. 


YOUR NEW TEST CASES
Submitted as an input file named my_input.txt.  Briefly describe how
this test exercises the corner cases of your implementation.  Why is
this test case interesting and/or challenging?

My test case is interesting because it tests a number of different, strange scenarios as 
well as various corner cases for my program. Some of the corner cases it tests includes:
	- Friending a person with him/herself
	- Printing an invite list of a size greater than the max length of a person's network
	- Passing messages when a person has no friends
	- Printing invite lists for invalid (negative) lengths and for non-existent users
	- Circular friendships (eg: A -> B -> C -> A) and their effect on print_invite_list

It also creates an interesting scenario where there are five people (Bob, Bob2, ..., Bob5)
who are only friends with the next person in the series (eg: Bob2 is friends with Bob3), and
Bob5 has no friends. It then creates a new message for the person at the beginning of the list (Bob)
and then calls pass_message a bunch of times. If you call it enough times, then it should pass this
message eventually to the end of the friend chain, and print is called to check to see when this message
does actually get to Bob5, which is where it can't travel any further. 


EXTRA CREDIT: 

I implemented an algorithm that mutates the contents of all messages every time messages are passed. 
The algorithm mutates the messages in a number of ways by: 
	- Changing the vowels (aeiou) of the message to other vowels
	- Inserting additional repeating (or sometimes completely random) characters inside of words
	- Randomly uppercasing certain characters in words
	- Occasionally removing the first or last characters of words

Everything has a certain probability of happening, and I used the MT generator to generate random 
integers so that every time the program is run, the mutations of the messages are different. So now 
every time you pass messages in the graph, all the messages are changed in some way (although it certainly
is possible for the entire message to not change, though this is very rare). I have tested this "telephone"
algorithm multiple times and it has yielded some very interesting results. 


MISC. COMMENTS TO GRADER:  
None. 