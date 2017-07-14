#include <iostream>
#include <fstream>
#include <string>
#include <cstdlib>
#include <cassert>
#include "MersenneTwister.h"
#include "graph.h"


// Helper function for reading quoted messages
std::string read_quoted_string(std::istream &istr) 
{
    std::string answer;
    istr >> answer;
    assert (answer[0] == '"');

    // Is this only a single word within quotes? (eg: "cat")
    if (answer[answer.size()-1] == '"') 
        return answer;

    // If there are multiple words within the quotes, then read them all in
    while (true) {
        std::string s;
        istr >> s;
        answer += ' ' + s;
       
        if (answer[answer.size()-1] == '"')
            return answer;
    } 
}


// Main function, entry point of the application
int main(int argc, char* argv[])
{   
    if (argc != 2) {
        std::cout << "Wrong number of arguments.\n";
        std::cout << "Usage:  " << argv[0] << " <graph-file>" << std::endl;
        return 1;
    }

    std::ifstream in_str(argv[1]);
    if (!in_str) {
        std::cout << "Bad file name: " << argv[1] << std::endl;
        return 1;
    }

    MTRand mtrand_fixed_seed(24);
    MTRand mtrand_autoseed;
    Graph friend_graph;
    std::string request, name1, name2;
    int count;

    
    // While there are more requests, determine the type of request and handle it. 
    while (in_str >> request) {

        if (request == "add_person") {
            in_str >> name1;
            if (friend_graph.add_person(name1))
                std::cout << "Added person " << name1 << std::endl;
            else
                std::cout << "Error trying to add person " << name1 << std::endl;
        } 

        else if (request == "remove_person") {
            in_str >> name1;
            if (friend_graph.remove_person(name1))
                std::cout << "Removed person " << name1 << std::endl;
            else 
                std::cout << "Error trying to remove person " << name1 << std::endl;
        } 

        else if (request == "add_friendship") {
            in_str >> name1 >> name2;
            if (friend_graph.add_friendship(name1, name2)) 
                std::cout << "Linked friends " << name1 << " and " << name2 << std::endl;
            else
                std::cout << "Error trying to link friends " << name1 << " and " << name2 << std::endl;
        } 

        else if (request == "remove_friendship") {
            in_str >> name1 >> name2;
            if (friend_graph.remove_friendship(name1, name2)) 
                std::cout << "Unlinked friends " << name1 << " and " << name2 << std::endl;
            else
                std::cout << "Error trying to unlink friends " << name1 << " and " << name2 << std::endl;
        } 

        else if (request == "add_message") {
            in_str >> name1;
            std::string msg = read_quoted_string(in_str);
            if (friend_graph.add_message(name1,msg)) 
                std::cout << "Added message " << msg << " to person " << name1 << std::endl;
            else
                std::cout << "Error trying to add message " << msg << " to person " << name1 << std::endl;
        } 

        else if (request == "pass_messages") {
            //friend_graph.pass_messages(mtrand_fixed_seed);
            friend_graph.pass_messages(mtrand_autoseed);
        } 

        else if (request == "print_invite_list") {
            in_str >> name1 >> count;
            if (!friend_graph.print_invite_list(std::cout,name1,count))
                std::cout << "Error trying to print invitation list for person " << name1 << std::endl;
        } 

        else if (request == "print") {
            friend_graph.print(std::cout);
        } 

        else {
            std::cout << "Illegal input: " << request << std::endl;
        }
    }
    return 0;
}