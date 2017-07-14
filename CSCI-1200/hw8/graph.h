#ifndef GRAPH_H
#define GRAPH_H

#include <string>
#include <iostream>
#include <list>
#include <vector>
#include <algorithm>
#include "MersenneTwister.h"

class Person;
class Message;


// ================================================================
// A graph stores a vector of all the people currently logged on and a
// vector of all the messages in the system.  People are added and
// removed from the system.  Friendships are established between
// people.  Messages can be added and passed randomly within the
// network.  The current and complete state of the system can be 
// printed.  Alternatively, the invitation list of a certain depth of
// friends from a given person can also be printed.

class Graph {
public:
    Graph();
    ~Graph();

    Person* find_person(const std::string &name) const; 

    bool add_person(const std::string& person_name);
    bool remove_person(const std::string& person_name);
    bool add_friendship(const std::string& person_name1, const std::string& person_name2);
    bool remove_friendship(const std::string& person_name1, const std::string& person_name2);
    bool add_message(const std::string& person_name, const std::string& message);
    void pass_messages(MTRand &mtrand);

    void print(std::ostream &ostr) const;
    bool print_invite_list(std::ostream &ostr, const std::string &name, int n);

private:
    std::vector<Person*> m_people;
    std::vector<Message*> m_messages;

    std::vector<Person*> get_invite_list(std::ostream& ostr, int n, int current, Person* p); 
};


#endif
