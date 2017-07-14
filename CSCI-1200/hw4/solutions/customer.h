#include <fstream>
#include <iostream>
#include <string>
#include <list>
#include <cassert>

// A global constant specifying the maximum number of DVDs a customer
// can hold at one time
const int MAX_RENTALS = 3;


// Simple class to store data for each customer, including name,
// preferences, and movies currently held.  This is also where data
// about total rentals, how many times they received their first
// choice, etc.  can be stored (for further analysis).

class Customer {
public:

  // CONSTRUCTOR
  Customer(const std::string &n);

  // ACCESSORS
  std::string get_name() const { return name; }
  const std::list<std::string>& get_preferences() const { return preferences; }
  const std::list<std::string>& get_movies() const { return movies; }
  int has_max_num_movies() { return (int)movies.size() == MAX_RENTALS; }

  // MODIFIERS
  bool add_preference(const std::string &m);
  void receives(const std::string& s);
  int remove_from_preferences(const std::string& s);
  std::string return_oldest();
  std::string return_newest();
  void incr_waiting() { days_waiting++; }
  bool preference_list_empty() { return (preferences.size() == 0); }
  
  // PRINTING
  friend std::ostream& operator<<(std::ostream &ostr, const Customer& p);

private:

  // REPRESENTATION
  int received_first_choice;
  int total_rented;
  int days_waiting;

  std::string name;
  std::list<std::string> movies;
  std::list<std::string> preferences;
};


// helper functions
bool on_list(const std::list<std::string> &list, const std::string &x);
bool remove_from_list(std::list<std::string> &list, const std::string &x);
