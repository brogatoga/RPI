#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <list>
#include <cassert>
#include <cstdlib>

#include "item.h"
#include "customer.h"


// Type definitions
typedef std::list<Item> inventory_type;
typedef std::list<Customer> customers_type;
typedef std::list<std::string> preference_type;


// Function prototypes
std::string read_customer_name(std::istream &istr);
std::string read_dvd_name(std::istream &istr);
std::string grammar(const std::string& noun, int count); 

void usage(const char* program_name); 
void error_missing_customer(std::ostream& stream, const std::string& customer_name); 
void error_missing_movie(std::ostream& stream, const std::string& movie_name); 

customers_type::iterator find_customer(customers_type& customers, const std::string& customer_name); 
inventory_type::iterator find_DVD(inventory_type& inventory, const std::string& dvd_name);

void add_to_inventory(inventory_type& inventory, const std::string& dvd_name, int copies); 
void shipping_algorithm(inventory_type& inventory, customers_type& customers, std::ostream& ostr);


// Main function, entrypoint of the application
int main(int argc, char* argv[]) 
{
    // Error checking and input parsing/setup
    if (argc < 3 || argc > 5) {
        usage(argv[0]);
    }

    std::ifstream istr(argv[1]);
    if (!istr) {
        std::cerr << "ERROR: Could not open " << argv[1] << " for reading." << std::endl;
        usage(argv[0]); 
    }

    std::ofstream ostr(argv[2]);
    if (!ostr) {
        std::cerr << "ERROR: Could not open " << argv[2] << " for writing." << std::endl;
        usage(argv[0]); 
    }


    // Extra credit options
    bool use_improved_algorithm = false;
    bool print_analysis = false;
    for (int i = 3; i < argc; i++) {
        if (std::string(argv[i]) == "--improved") {
          use_improved_algorithm = true;
        } 
        else if (std::string(argv[i]) == "--analysis") {
          print_analysis = true;
        } 
        else {
          usage(argv[0]);
        }
    }


    // Process tokens
    inventory_type inventory;
    customers_type customers;
    std::string token;

    while (istr >> token) {

        // Add new DVD to the inventory
        if (token == "dvd") {
          std::string dvd_name = read_dvd_name(istr);
          int copies;
          istr >> copies;

          add_to_inventory(inventory, dvd_name, copies); 
          ostr << grammar("copy", copies) << " of " << dvd_name << " added" << std::endl;
        } 


        // Add new customer and his preferences to the customer database
        else if (token == "customer") {
            std::string customer_name = read_customer_name(istr);
            customers_type::iterator customer = find_customer(customers, customer_name); 
            std::string dvd_name; 
            preference_type preferences; 
            int num_movies;
            istr >> num_movies;

            // Read the preferred movies and add them to the list
            for (int c = 0; c < num_movies; c++) {
                dvd_name = read_dvd_name(istr);
                inventory_type::iterator item = find_DVD(inventory, dvd_name); 

                if (item != inventory.end()) 
                    preferences.push_back(dvd_name); 
                else 
                    error_missing_movie(ostr, dvd_name); 
            }


            // Add new entry for the customer
            if (customer == customers.end()) {
                customers.push_back(Customer(customer_name)); 
                customers_type::iterator customer = find_customer(customers, customer_name); 

                /*
                // Are there any duplicates in the list? 
                std::list<std::string> preferences_unique = preferences; 
                preferences_unique.unique(); 

                if (preferences.size() != preferences_unique.size()) {
                    ostr << "WARNING: Duplicates exist in the preferences list when adding new customer" << std::endl;
                    continue; 
                }
                */


                // Add movies from the list to the customer's preference list    
                for (preference_type::iterator it = preferences.begin(); it != preferences.end(); it++) {
                    if (!customer->add_preference(*it))
                        ostr << "WARNING: Duplicate movie " << *it << " on preference list!" << std::endl; 
                }


                // Display success message
                ostr << "new customer: " << customer_name << std::endl;
            }
            else ostr << "WARNING: Already have a customer named " << customer_name << std::endl; 
        } 


        // Ship movies to customers using the shipping algorithm
        else if (token == "ship") {
            shipping_algorithm(inventory,customers,ostr);
        } 


        // Return the oldest/newest movie that the customer owns
        else if (token == "return_oldest" || token == "return_newest") {
            std::string customer_name = read_customer_name(istr);
            customers_type::iterator customer = find_customer(customers, customer_name); 
            std::string returned; 

            if (customer != customers.end()) {
                if (customer->get_num_movies() > 0) {
                    returned = (token == "return_oldest") ? customer->return_oldest() : customer->return_newest(); 
                    inventory_type::iterator item = find_DVD(inventory, returned); 

                    if (item != inventory.end()) {
                        ostr << customer_name << " returns " << returned << std::endl;
                        item->returned(); 
                    }
                }
                else ostr << "WARNING: " << customer_name << " has no DVDs to return!" << std::endl;
            }
            else error_missing_customer(ostr, customer_name); 
        } 


        // Prints the details of a customer
        else if (token == "print_customer") {
            std::string customer_name = read_customer_name(istr);
            customers_type::iterator customer = find_customer(customers, customer_name);

            if (customer != customers.end())
                customer->print(ostr);
            else 
                error_missing_customer(ostr, customer_name); 
        } 


        // Prints the details of a DVD
        else if (token == "print_dvd") {
            std::string dvd_name = read_dvd_name(istr);
            inventory_type::iterator item = find_DVD(inventory, dvd_name);

            if (item != inventory.end())
                item->print(ostr);
            else
                error_missing_movie(ostr, dvd_name); 
        } 


        // Adds a movie to a customer's preference list
        else if (token == "add_preference") {
            std::string customer_name = read_customer_name(istr);
            std::string dvd_name = read_dvd_name(istr);
            customers_type::iterator customer = find_customer(customers, customer_name); 

            if (customer != customers.end()) {
                if (!customer->has_movie(dvd_name)) {
                    if (find_DVD(inventory, dvd_name) != inventory.end()) {
                        if (!customer->add_preference(dvd_name))
                            ostr << "WARNING: " << customer_name << " already has " << dvd_name << " on his/her preference list!" << std::endl;
                    }
                    else error_missing_movie(ostr, dvd_name); 
                }
                else ostr << "WARNING: " << customer_name << " currently has " << dvd_name << "!" << std::endl;
            }
            else error_missing_customer(ostr, customer_name);
        } 


        // Invalid token, display an error message and abort the program
        else {
            std::cerr << "ERROR: Unknown token " << token << std::endl;
            exit(1);
        }
    }
}


// A customer name is simply two strings, first & last name
std::string read_customer_name(std::istream &istr) 
{
    std::string first, last;
    istr >> first >> last;
    return first + " " + last;
}


// A movie name is one or more strings inside of double quotes
std::string read_dvd_name(std::istream &istr) 
{
    std::string token;
    istr >> token;
    assert (token[0] == '"');
    std::string answer = token;

    while (answer[answer.size()-1] != '"') {
        istr >> token;
        answer += " " + token;
    }
    return answer;
}


// Adds an item to the inventory (DO NOT USE THIS FOR RETURNING MOVIES)
void add_to_inventory(inventory_type& inventory, const std::string& dvd_name, int copies)
{
    inventory_type::iterator item; 
    if ((item = find_DVD(inventory, dvd_name)) != inventory.end())
        item->add_copy(copies); 
    else
        inventory.push_back(Item(dvd_name, copies)); 
}


// A helper function to find a DVD from the inventory
inventory_type::iterator find_DVD(inventory_type& inventory, const std::string& dvd_name)
{
    for (inventory_type::iterator inventory_itr = inventory.begin(); inventory_itr != inventory.end(); inventory_itr++) {
        if (inventory_itr->get_name() == dvd_name)
            return inventory_itr;
    }
    return inventory.end();
}


// Returns an iterator to the customer with the specified name
customers_type::iterator find_customer(customers_type& customers, const std::string& customer_name)
{
    for (customers_type::iterator j = customers.begin(); j != customers.end(); j++) {
        if (j->get_name() == customer_name)
            return j; 
    }
    return customers.end(); 
}


// Prints usage and information about the program
void usage(const char* program_name) 
{
    std::cerr << "Usage: " << program_name << " <input_file> <output_file>" << std::endl;
    std::cerr << " -or-  " << program_name << " <input_file> <output_file> --analysis" << std::endl;
    std::cerr << " -or-  " << program_name << " <input_file> <output_file> --improved" << std::endl;
    std::cerr << " -or-  " << program_name << " <input_file> <output_file> --improved --analysis" << std::endl;
    exit(1);
}


// Returns the given noun in singular or plural form depending on the number
std::string grammar(const std::string& noun, int count)
{
    std::string result; 
    char last_char = noun[noun.length()-1]; 

    // SPECIAL CASE
    if (noun == "movie" && count == 1)
        return "1 movies"; 

    // If count is 0, then it should be "no [noun]", if it's non-zero then print the number out
    if (count > 0) {
        std::stringstream ss; 
        ss << count; 
        result = ss.str(); 
    }
    else result = "no"; 

    // Add a space
    result += " "; 

    // If count is 0 or >1, then pluralize it
    if (count != 1) {
        if (last_char == 'e')
            result += (noun + "s"); 
        else if (last_char == 'y')
            result += (noun.substr(0, noun.length()-1) + "ies"); 
    }

    // Otherwise, make it singular
    else {
        result += noun; 
    }
    
    return result;
}


// Displays error message about customer not existing in the database
void error_missing_customer(std::ostream& stream, const std::string& customer_name)
{
    stream << "WARNING: No customer named " << customer_name << std::endl;
}


// Displays error message about DVD/movie not being in the inventory
void error_missing_movie(std::ostream& stream, const std::string& movie_name)
{
    stream << "WARNING: No movie named " << movie_name << " in the inventory" << std::endl; 
}


// A basic algorithm for determining which movies to ship to each customer
void shipping_algorithm(inventory_type &inventory, customers_type &customers, std::ostream &ostr) 
{
    ostr << "Ship DVDs" << std::endl;
    customers_type::iterator customer_itr = customers.begin();


    // Iterate through all of the customers in a priority order
    while (customer_itr != customers.end()) {

        // If customer has three movies OR their preference list is empty, then skip over them
        if (customer_itr->has_max_num_movies() || customer_itr->preference_list_empty()) {
            customer_itr++;
            continue;
        }

        bool sent_dvd = false;
        const preference_type &preferences = customer_itr->get_preferences();


        // Iterate through preferences and try to ship the customer's first choice
        for (preference_type::const_iterator preferences_itr = preferences.begin(); preferences_itr != preferences.end(); preferences_itr++) {
            inventory_type::iterator inventory_itr = find_DVD(inventory,*preferences_itr);

            // If the preferred movie exists and is available, then ship it to the customer
            if (inventory_itr != inventory.end() && inventory_itr->available()) {
                ostr << "  " << customer_itr->get_name() << " receives " << *preferences_itr << std::endl;
                inventory_itr->shipped();
                customer_itr->receives(*preferences_itr);


                // Then movie customer to the back of the queue
                customers.push_back(*customer_itr);
                customer_itr = customers.erase(customer_itr);


                // If DVD has been shipped, then set the flag and don't ship anymore until next time
                sent_dvd = true;        
                break;
            }
        }

        // If DVD was NOT sent, then we will have to move to the next customer manually
        if (!sent_dvd) 
            customer_itr++;
    }
}
