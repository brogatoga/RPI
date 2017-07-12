% ProgLang - Homework 1
% Problem #2
% Ryan Lin
% linr2@rpi.edu

:- [read_line].

% Definitions for sentence structures for converting from parse tree to sentence
s(s(NP,VP)) --> np(NP), vp(VP), end.
np(np(Det,Nom)) --> det(Det), nom(Nom).
vp(vp(Verb)) --> verb(Verb).
nom(nom(Noun)) --> noun(Noun).

det(det(a)) --> [a].
det(det(the)) --> [the].
det(det(every)) --> [every].

noun(noun(train)) --> [train].
noun(noun(bike)) --> [bike].
noun(noun(person)) --> [person].
noun(noun(flight)) --> [flight].

verb(verb(flew)) --> [flew].
verb(verb(left)) --> [left].
verb(verb(arrived)) --> [arrived].
verb(verb(stayed)) --> [stayed].

end --> ['.'].


% Number generator
natural(1).
natural(N) :- natural(M), N is M+1.


% Parse functions
parse(Tree,Sentence) :- phrase(s(Tree),Sentence).


% Converts the list containing the sentence components into a proper sentence
list_to_sentence([Det,Noun,Verb,Period],X) :-
	atom_concat(Det,' ',A),
	atom_concat(A,Noun,B),
	atom_concat(B,' ',C),
	atom_concat(C,Verb,D),
	atom_concat(D,Period,X).


% Concatenates all the elements of the list together into a single string
compress_list([],'') :- !.
compress_list([H|T],X) :-
	compress_list(T,S),
	atom_concat(H,S,X).
	
	
% Mirror function, returns exactly what is passed into it
mirror(X,X).


% Filters the input list and removes "useless" symbols, returning the cleaned list
filter_input_list([A,B,C,D,E,F,G,H],X) :-
	filter_input_string(D,DX),
	filter_input_string(F,FX),
	filter_input_string(H,HX),
	mirror([A,B,C,DX,E,FX,G,HX],X).
	
	
% Removes everything after the first right parentheses (including that symbol too)
filter_input_string(In,Out) :-
	string_codes(In,Codes),
	fis_helper(Codes,Out).
	
fis_helper([],'') :- !.
fis_helper([H|_],'') :- H = 41.
fis_helper([H|T],R) :-
	H \= 41,
	string_codes(C,[H]),
	name(A,C),
	fis_helper(T,S),
	atom_concat(A,S,R).


% Given a list of codes representing the parse tree string, turns it into a token list
process_parsetree(S,X) :-
	atomic_list_concat(RawInputs,'(',S),				% Split the list using left parentheses as a delimiter
	filter_input_list(RawInputs,X).						% Now clean up any of the 8 entries with non-alphabetic symbols
	
	
% Converts a parse tree input string to the sentence that it represents
parse_tree_to_sentence(Input,Sentence) :-
	compress_list(Input,String),												% Compress the inputted list into a single string
	process_parsetree(String,X),												% "Clean up" the string and get the resulting token list
	mirror(X,[s,np,det,Det,noun,Noun,verb,Verb]),								% Extract the useful parts of the token list out
	s(s(np(det(Det),nom(noun(Noun))),vp(verb(Verb))),SentenceList,[]),			% See if we can find a sentence that has this parse tree
	list_to_sentence(SentenceList,Sentence),									% If a valid SentenceList was returned, then convert it to an actual sentence
	!,
	write(Sentence),nl, 														% And then write it!
	true.
	
parse_tree_to_sentence(_,_) :-
	!,
	write('Not a valid parse tree, or no sentence exists for this parse tree!'),nl,
	false.


% Infinite loop that reads in a sentence and writes the parse tree for it
loop :- 
	natural(I),
	read_line(Sent),
	parse(Tree,Sent),
	write(Tree), nl,
	I =< 0,
	!, true.
	
	
% The second loop that converts a parse tree into its corresponding sentence
loop2 :-
	natural(I),
	read_line(Input),
	parse_tree_to_sentence(Input,_),
	I =< 0,
	!, true.