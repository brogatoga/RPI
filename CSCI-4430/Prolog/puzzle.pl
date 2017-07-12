% ProgLang - Homework 1
% Problem #1
% Ryan Lin
% linr2@rpi.edu

% Calculates the sum of all the numbers in a list
list_sum([], 0).
list_sum([H|T], Sum) :-
	sum_list(T, Rest),
	Sum is H + Rest.
	
	
% Removes X from the list represented as [H | L]
remove_from_list(H,[H|L],L).  
remove_from_list(X,[H|A],[H|B]) :- remove_from_list(X,A,B).


% Generates a permutation for the list range [1..N] and stores in P
generate_permutations([H|L],P) :- 
	generate_permutations(L,X), 
	remove_from_list(H,P,X).  
generate_permutations([],[]).


% Checks to make sure that the right sets of variables add up to the given number
everything_adds_up_to([A,B,C,D,E,F,G,H,I], X) :- 
	list_sum([A,B,D], X),
	list_sum([A,C,F], X),
	list_sum([D,E,F], X),
	list_sum([D,G,I], X),
	list_sum([F,H,I], X).


% Solves the puzzle
puzzle(S) :-
	generate_permutations([1,2,3,4,5,6,7,8,9], S),
	everything_adds_up_to(S, 17).