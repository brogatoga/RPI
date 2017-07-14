HOMEWORK 9: PERFECT HASHING

NAME: Ryan Lin
EMAIL: linr2@rpi.edu


COLLABORATORS AND OTHER RESOURCES:
https://en.wikipedia.org/wiki/Hash_table
https://en.wikipedia.org/wiki/Perfect_hash_function


ESTIMATE OF # OF HOURS SPENT ON THIS ASSIGNMENT: 8 hours


SUMMARY OF RESULTS: 
Summarize your results on the provided examples.  Were you able to
construct a perfect hash function?  If not, how close did you get (how
many collisions)?  If yes, what was the smallest perfect hash you were
able to construct for the different examples?  What was your
compression ratio?  Did you make any additional test cases?

Unfortunately I was not able to construct a perfect hash function. I was not 
close at all, as there were a lot of collisions. I was only able to construct
a perfect hash for the chair, the other images had at least a few hundred
pixels difference from the originals. I did not make any additional test cases. 
The results for my compression ratio are below: 

ORIGINAL_SIZE  |  OCCUPANCY  |  HASH_DATA  |  OFFSET  | RATIO
_________________________________________________________________

119 bytes | 38 bytes | 18 bytes | 13 bytes | 50% compression
6.32 kb | 317 bytes | 1.70 kb | 160 bytes | 66% compression
48.0 kb | 2.01 kb | 4.24 kb | 377 bytes | 86% compression

On average, out of the three images, my compression ratio is about 67.3%.


OFFSET SEARCH FOR EXTRA CREDIT:
If you implemented a search strategy instead of or in addition to the
greedy method from the paper, describe it here.

I didn't do the extra credit. 



MISC. COMMENTS TO GRADER:  

None.