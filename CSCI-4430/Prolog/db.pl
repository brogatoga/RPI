% ProgLang - Homework 1
% Problem #3
% Ryan Lin
% linr2@rpi.edu

:- [read_line].

% Definitions for the various sentence structures
input(X) --> positive_command(X).
input(X) --> negative_command(X).
input(X) --> multi_positive_command(X).
input(X) --> multi_negative_command(X).
input(X) --> did_query(X).
input(X) --> multi_did_query(X).
input(X) --> multi_didnot_query(X).

positive_command(positive_command(N,X,V)) --> [the], noun(N), [X], verb(V), period.
negative_command(negative_command(N,X,V)) --> [the], noun(N), [X], [did], [not], present_verb(V), period.
multi_positive_command(multi_positive_command(N,A,B,V)) --> [the], plural_noun(N), [A], [and], [B], verb(V), period.
multi_negative_command(multi_negative_command(N,A,B,V)) --> [the], plural_noun(N), [A], [and], [B], [did], [not], present_verb(V), period.

did_query(did_query(Q,N,V)) --> [did], quantifier(Q), noun(N), present_verb(V), qmark.
multi_did_query(multi_did_query(N,V)) --> [which], plural_noun(N), verb(V), qmark.
multi_didnot_query(multi_didnot_query(N,V)) --> [which], plural_noun(N), [did], [not], present_verb(V), qmark.

noun(train) --> [train].
noun(bike) --> [bike].
noun(person) --> [person].
noun(flight) --> [flight].

plural_noun(trains) --> [trains].
plural_noun(bikes) --> [bikes].
plural_noun(people) --> [people].
plural_noun(flights) --> [flights].

verb(flew) --> [flew].
verb(left) --> [left].
verb(arrived) --> [arrived].
verb(stayed) --> [stayed].

present_verb(fly) --> [fly].
present_verb(leave) --> [leave].
present_verb(arrive) --> [arrive].
present_verb(stay) --> [stay].

quantifier(a) --> [a].
quantifier(every) --> [every].

period --> ['.'].
qmark --> ['?'].


% Maps verbs in past tense to present tense, and vice versa
tense(flew,fly).
tense(left,leave).
tense(arrived,arrive).
tense(stayed,stay).


% Maps nouns in singular form to plural form, and vice versa
form(train,trains).
form(bike,bikes).
form(person,people).
form(flight,flights).


% Parse function
parse(Tree,Sentence) :- phrase(input(Tree),Sentence).


% Infinite number generator
natural(1).
natural(N) :- natural(M), N is M+1.


% Given a list of names, generates a user-friendly string representing the contents of the list
generate_response([],'None of them').
generate_response([X],X).

generate_response([X,Y],R) :-
	atom_concat(X,' and ',S),
	atom_concat(S,Y,R).
	
generate_response([H|T],R) :-
	generate_response(T,X),
	atom_concat(H,', ',S),
	atom_concat(S,X,R).
	
generate_response(_,'Error') :- !.
	

% Add in a positive command ("did" do something) to the database
process(Sent) :- 
	parse(positive_command(Noun,Name,Verb),Sent),
	not(entry(Name,Noun,Verb,yes)),
	assert(entry(Name,Noun,Verb,yes)),
	!,
	retract(entry(Name,Noun,Verb,no)).	  	% Remove the opposite fact
	
	
% Add in a negative command ("did NOT" do something) to the database
process(Sent) :-
	parse(negative_command(Noun,Name,PresentVerb),Sent),
	tense(Verb,PresentVerb),
	not(entry(Name,Noun,Verb,no)),
	assert(entry(Name,Noun,Verb,no)),
	!,
	retract(entry(Name,Noun,Verb,yes)).		% Remove the opposite fact


% And in a positive command for TWO people
process(Sent) :-
	parse(multi_positive_command(PluralNoun,First,Second,Verb),Sent),
	form(Noun,PluralNoun),
	not(entry(First,Noun,Verb,yes)),
	not(entry(Second,Noun,Verb,yes)),
	assert(entry(First,Noun,Verb,yes)),
	assert(entry(Second,Noun,Verb,yes)),
	!,
	( retract(entry(First,Noun,Verb,no)) ; retract(entry(Second,Noun,Verb,no)) ).	% Remove both opposite facts
	

% Add in a negative command for TWO people
process(Sent) :-
	parse(multi_negative_command(PluralNoun,First,Second,PresentVerb),Sent),
	form(Noun,PluralNoun),
	tense(Verb,PresentVerb),
	not(entry(First,Noun,Verb,no)),
	not(entry(Second,Noun,Verb,no)),
	assert(entry(First,Noun,Verb,no)),
	assert(entry(Second,Noun,Verb,no)),
	!,
	( retract(entry(First,Noun,Verb,yes)) ; retract(entry(Second,Noun,Verb,yes)) ).	% Remove both opposite facts


% If this is a duplicate command, then issue a warning
process(Sent) :- 
	( parse(positive_command(_,_,_),Sent) ; parse(negative_command(_,_,_),Sent) 
		; parse(multi_positive_command(_,_,_,_),Sent) ; parse(multi_negative_command(_,_,_,_),Sent)),
	!,
	write('WARNING: One or more of the statements you entered is a duplicate!'),nl.


% Query the database using an existential quantifier on a single thing
process(Sent) :-
	parse(did_query(a,Noun,PresentVerb),Sent),
	tense(Verb,PresentVerb),
	entry(_,Noun,Verb,yes),		% If any "did" entry exists, then say yes
	!,
	write('yes'),nl.
	
process(Sent) :-
	parse(did_query(a,_,_),Sent),
	!,
	write('no'),nl.
	
	
% Query the database using the universal quantifier on a single thing
process(Sent) :-
	parse(did_query(every,Noun,PresentVerb),Sent),
	tense(Verb,PresentVerb),
	entry(_,Noun,Verb,no),	  	% If any "did not" entry exists, then say no
	!,
	write('no'),nl.
	
process(Sent) :-
	parse(did_query(every,_,_),Sent),
	!,
	write('yes'),nl.
	

% Query the database for which things DID do something
process(Sent) :-
	parse(multi_did_query(PluralNoun,Verb),Sent),
	form(Noun,PluralNoun),
	findall(X,entry(X,Noun,Verb,yes),L),
	generate_response(L,R),
	!,
	write(R),nl.


% Query the database for which things DID NOT do something
process(Sent) :-
	parse(multi_didnot_query(PluralNoun,PresentVerb),Sent),
	form(Noun,PluralNoun),
	tense(Verb,PresentVerb),
	findall(X,entry(X,Noun,Verb,no),L),
	generate_response(L,R),
	!,
	write(R),nl.
	
	
% The following will be run if the user entered in an unrecognized command or query
process(_) :-
	!,
	write('Huh?'),nl.
	
	
% Infinite loop that allows users to update and did_query the database
loop :-
	natural(I),
	assert(entry(a,a,a,a)),		% Used to get rid of an error if did_query is asked first
	read_line(Sent),
	process(Sent),
	I =< 0,
	!, true.