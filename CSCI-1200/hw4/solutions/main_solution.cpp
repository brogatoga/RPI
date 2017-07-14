// ==================================================================
// Important Note: You are encouraged to read through this provided
//   code carefully and follow this structure.  You may modify the
//   file as needed to complete your implementation.
// ==================================================================

#include <fstream>
#include <iostream>
#include <string>
#include <list>
#include <cassert>
#include <cstdlib>


// 2 custom classes
#include "customer.h"
#include "dvd.h"

typedef std::list<DVD> inventory_type;
typedef std::list<Customer> customers_type;
typedef std::list<std::string> preference_type;



// ==================================================================

// Helper function prototypes
std::string read_customer_name(std::istream &istr);
std::string read_dvd_name(std::istream &istr);
inventory_type::iterator find_DVD(inventory_type &inventory, const std::string &dvd_name);

// The main algorithm for DVD distribution
void shipping_algorithm(inventory_type &inventory, customers_type &customers, std::ostream &ostr);



// additional helper functions
customers_type::iterator find_customer(customers_type &customers, const std::string &customer_name);
void return_DVD(inventory_type &inventory, customers_type &people, std::ostream &ostr, const std::string &customer_name, bool newest);
void add_preference(inventory_type &inventory, customers_type &people, std::ostream &ostr, const std::string &customer_name, const std::string &dvd_name);
void add_DVD(inventory_type &inventory, std::ostream &ostr, const std::string &dvd_name,int copies);



// ==================================================================


void usage(const char* program_name) {
  std::cerr << "Usage: " << program_name << " <input_file> <output_file>" << std::endl;
  std::cerr << " -or-  " << program_name << " <input_file> <output_file> --analysis" << std::endl;
  std::cerr << " -or-  " << program_name << " <input_file> <output_file> --improved" << std::endl;
  std::cerr << " -or-  " << program_name << " <input_file> <output_file> --improved --analysis" << std::endl;
  exit(1);
}


int main(int argc, char* argv[]) {

  if (argc < 3 || argc > 5) {
    usage(argv[0]);
  }

  // open input and output file streams
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

  
  // optional parameters for extra credit
  bool use_improved_algorithm = false;
  bool print_analysis = false;
  for (int i = 3; i < argc; i++) {
    if (std::string(argv[i]) == "--improved") {
      use_improved_algorithm = true;
    } else if (std::string(argv[i]) == "--analysis") {
      print_analysis = true;
    } else {
      usage(argv[0]);
    }
  }
  

  // stores information about the DVDs
  inventory_type inventory;
  // stores information about the customers
  customers_type customers;


  // read in and handle each of the 8 keyword tokens
  std::string token;
  while (istr >> token) {
    if (token == "dvd") {
      std::string movie = read_dvd_name(istr);
      int copies;
      istr >> copies;

      // O(d)
      add_DVD(inventory,ostr,movie,copies);

      
    } else if (token == "customer") {
      std::string customer_name = read_customer_name(istr);
      int num_movies;
      istr >> num_movies;
      // overall: O(p+c)

      Customer c(customer_name);

      // O(p), read p movies, p = # of dvds on preference list
      for (int i = 0; i < num_movies; i++) {
        std::string movie = read_dvd_name(istr);

        if (find_DVD(inventory,movie) == inventory.end()) {
          ostr << "WARNING: No movie named " << movie << " in the inventory" << std::endl;
        } else if (!c.add_preference(movie)) {
          ostr << "WARNING: Duplicate movie " << movie << " on preference list!" << std::endl;   
        }

      }

      // O(c), make sure this customer doesn't already exist, c = # of customers
      customers_type::iterator itr = find_customer(customers,customer_name);
      if (itr == customers.end()) {
        customers.push_back(c);
        ostr << "new customer: " << customer_name << std::endl;
      } else {
        ostr << "WARNING: Already have a customer named " << customer_name << std::endl;
      }


    } else if (token == "ship") {

      shipping_algorithm(inventory,customers,ostr);
      
    } else if (token == "return_oldest") {
      std::string customer_name = read_customer_name(istr);


      // O(c+d), see analysis below, c = # of customers, d = # of dvds
      return_DVD(inventory,customers,ostr,customer_name,false);


    } else if (token == "return_newest") {
      std::string customer_name = read_customer_name(istr);

      // O(c+d), see analysis below, c = # of customers, d = # of dvds      
      return_DVD(inventory,customers,ostr,customer_name,true);


    } else if (token == "print_customer") {
      std::string customer_name = read_customer_name(istr);

      
      // overall: O(c+p)
      // O(c), find customer, c = # of customers
      customers_type::iterator itr = find_customer(customers,customer_name);
      if (itr == customers.end()) {
        // O(p), print customer, p = # of dvds on preference list
        ostr << "WARNING: No customer named " << customer_name << std::endl;
      } else {
        ostr << *itr;
      }


    } else if (token == "print_dvd") {
      std::string dvd_name = read_dvd_name(istr);


      // overall: O(d)
      // O(d), find dvd, d = # of different dvds in inventory
      inventory_type::iterator itr = find_DVD(inventory,dvd_name);
      if (itr == inventory.end()) {
        ostr << "WARNING: No movie named " << dvd_name << " in the inventory" << std::endl;
      } else {
        // O(1) to print status of copies
        // (does not print the customers)
        ostr << *itr;
      }


    } else if (token == "add_preference") {
      std::string customer_name = read_customer_name(istr);
      std::string dvd_name = read_dvd_name(istr);


      // O(c+d+p), see analysis below
      add_preference(inventory,customers,ostr,customer_name,dvd_name);


    } else {
      std::cerr << "ERROR: Unknown token " << token << std::endl;
      exit(1);
    }
  }
}


// ==================================================================


// A customer name is simply two strings, first & last name
// O(1)
std::string read_customer_name(std::istream &istr) {
  std::string first, last;
  istr >> first >> last;
  return first + " " + last;
}


// A movie name is one or more strings inside of double quotes
// O(1)
std::string read_dvd_name(std::istream &istr) {
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


// A helper function to find a DVD from the inventory
// O(d) to find dvd, d = # of dvds
inventory_type::iterator find_DVD(inventory_type &inventory, const std::string &dvd_name) {
  for (inventory_type::iterator inventory_itr = inventory.begin(); 
       inventory_itr != inventory.end(); inventory_itr++) {
    if (inventory_itr->get_name() == dvd_name) {
      return inventory_itr;
    }
  }
  // if the DVD is not found, return the .end() iterator
  return inventory.end();
}


// ==================================================================

//
// A basic algorithm for determining which movies to ship to each customer
//

// O(3*c) loop over the customers, because each customer who receives
// a dvd is put at the back of the list, we will loop over the full
// list of customers a maximum of 3 times (if each customers receives
// 3 dvds in this round).  c = # of customers

// for each customer:  (multiplied)

// O(p), loop over the customers preference list. p = # of dvds on
// preference list

// for each dvd: (multiplied)

// O(d), find the dvd in the inventory

// O(1), check availability

// O(p+d) process rental (at most once!)

// overall O(c*(p*d+p+d)) = O(c*p*d)

void shipping_algorithm(inventory_type &inventory, customers_type &customers, std::ostream &ostr) {

  ostr << "Ship DVDs" << std::endl;

  // Loop over the customers in priority order
  //
  // Note that we edit the customers list as we go, to add customers
  // to the back of the list if they receive a DVD.  This allows a
  // customer to receive multiple DVDs in one shipment cycle, but only
  // after all other customers have had a chance to receive DVDs.
  //
  customers_type::iterator customer_itr = customers.begin();
  while (customer_itr != customers.end()) {

    // skip this customer if they already have 3 movies or if their
    // preference list is empty (no outstanding requests)
    if (customer_itr->has_max_num_movies() || customer_itr->preference_list_empty()) {
      // move on to the next customer
      customer_itr++;
      continue;
    }

    // a helper flag variable
    bool sent_dvd = false;
 
    // loop over the customer's preferences
    const preference_type &preferences = customer_itr->get_preferences();
    for (preference_type::const_iterator preferences_itr = preferences.begin(); 
         preferences_itr != preferences.end(); preferences_itr++) {

      // locate this DVD in the inventory
      inventory_type::iterator inventory_itr = find_DVD(inventory,*preferences_itr);
      if (inventory_itr != inventory.end() && inventory_itr->available()) { 

        // if the DVD is available, ship it to the customer!
        ostr << "  " << customer_itr->get_name() << " receives " << *preferences_itr << std::endl;
        inventory_itr->shipped();
        customer_itr->receives(*preferences_itr);
        
        // move this customer to the back of the priority queue
        // they will get a chance to receive another DVD, but only
        // after everyone else gets a chance
        customers.push_back(*customer_itr);
        customer_itr = customers.erase(customer_itr);

        // after setting the flag to true, leave the iteration over preferences
        sent_dvd = true;        
        break;
      }
    }

    // if no DVD was sent to this customer, then we move on to the next customer 
    // (do not change this customer's priority for tomorrow's shipment)
    if (!sent_dvd) {
      customer_itr++;
    }
  }
}

// ==================================================================


// helper function to locate specific customer by name
// O(c) to find the the customer, c = # of customers
customers_type::iterator find_customer(customers_type &customers, const std::string &customer_name) {
  for (customers_type::iterator itr = customers.begin();
       itr != customers.end(); itr++) {
    if (itr->get_name() == customer_name)
      return itr;
  }
  return customers.end();
}


// helper function to return a specific dvd from a specific customer
// running time:  overall O(c+d)
// O(c) to find the the customer, c = # of customers
// O(1) which dvd to return
// O(d) find the dvd in the inventory
// O(1) process the return
void return_DVD(inventory_type &inventory, customers_type &customers, std::ostream &ostr, const std::string &customer_name, bool newest) {
  std::string which = "";

  // find the customer
  customers_type::iterator itr = find_customer(customers,customer_name);
  if (itr == customers.end()) { 
    ostr << "WARNING: No customer named " << customer_name << std::endl;
    return; 
  }

  // figure out which dvd to return
  if (newest == false) {
    which = itr->return_oldest();
  } else {
    which = itr->return_newest();
  }

  // error checking
  if (which == "") {
    ostr << "WARNING: " << customer_name << " has no DVDs to return!" << std::endl;
    return;
  }
  inventory_type::iterator itr2 = find_DVD(inventory,which);
  if (itr2 == inventory.end()) {
    ostr << "WARNING: No movie named " << which << " in the inventory" << std::endl;
    return;
  }

  // success output message
  ostr << customer_name << " returns " << which << std::endl;
  itr2->returned();
}


// a helper function to determine if item is on the list
// running time: O(p), p = # of dvds on preference list
bool on_list(const preference_type &list, const std::string &x) {
  for (preference_type::const_iterator itr = list.begin();
       itr != list.end(); itr++) {
    if (*itr == x) {
      return true;
    }
  }
  return false;
}


// a helper function to remove an item from the list
// running time: O(p), p = # of dvds on preference list
bool remove_from_list(preference_type &list, const std::string &x) {
  for (preference_type::iterator itr = list.begin();
       itr != list.end(); itr++) {
    if (*itr == x) {
      list.erase(itr);
      return true;
    }
  }
  return false;
}


// helper function to add dvd to preference list
// running time:  overall O(c+d+p)
// O(c) to find the the customer, c = # of customers
// O(d) verify that the dvd exists in inventory, d = # of dvds
// O(3) see if user already holds that dvd
// O(p) see if user already has dvd on preference list
// O(1) add to back of preference list
void add_preference(inventory_type &inventory, customers_type &customers, std::ostream &ostr, const std::string &customer_name, const std::string &dvd_name) {
  customers_type::iterator customer_itr = find_customer(customers,customer_name);
  if (customer_itr == customers.end()) {
    ostr << "WARNING: No customer named " << customer_name << std::endl;
    return;
  }
  if (find_DVD(inventory,dvd_name) == inventory.end()) {
    ostr << "WARNING: No movie named " << dvd_name << " in the inventory" << std::endl;
  } else if (on_list(customer_itr->get_movies(),dvd_name)) {
    ostr << "WARNING: " << customer_name << " currently has " << dvd_name << "!" << std::endl;
  } else if (on_list(customer_itr->get_preferences(),dvd_name)) {
    ostr << "WARNING: " << customer_name << " already has " << dvd_name << " on his/her preference list!" << std::endl;
  } else {
    bool success = customer_itr->add_preference(dvd_name);
    assert (success);
  }
}



// helper function to add dvd to the inventory
// running time:  overall O(d)
// O(d) see if the dvd already exists in inventory, d = # of dvds
// O(1) add copies or add new dvd
void add_DVD(inventory_type &inventory, std::ostream &ostr, const std::string &dvd_name,int copies) {

  inventory_type::iterator itr = find_DVD(inventory,dvd_name);
  if (itr == inventory.end()) {
    inventory.push_back(DVD(dvd_name, copies));
  } else {
    itr->add_inventory(copies);
  }
  
  if (copies == 1) {
    ostr << copies << " copy of " << dvd_name << " added" << std::endl;
  } else {
    ostr << copies << " copies of " << dvd_name << " added" << std::endl;
  }
}

