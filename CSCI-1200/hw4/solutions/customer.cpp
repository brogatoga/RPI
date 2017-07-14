#include "customer.h"  


// constructor
Customer::Customer(const std::string &n)
  : name(n) {
  received_first_choice = 0;
  total_rented = 0;
  days_waiting = 0;
}


// O(1) add to the movies held list
// O(p), to remove this item from the preference list, p = # of preferences
void Customer::receives(const std::string& s) {
  movies.push_back(s);
  int i = remove_from_preferences(s);
  // keep track of statistics
  if (i == 0) {
    received_first_choice++;
  }
  total_rented++;
}


// O(p), make sure the movie is not already the preference list, p = # of preferences
bool Customer::add_preference(const std::string &m) { 
  if (!on_list(preferences,m)) {
    preferences.push_back(m);
    return true;
  } else {
    return false;
  }
}


// O(p), to remove this item from the preference list, p = # of preferences
int Customer::remove_from_preferences(const std::string& s) {
  int count = -1;
  for (std::list<std::string>::iterator itr = preferences.begin();
       itr != preferences.end();
       itr++) {
    if (*itr == s) {
      itr = preferences.erase(itr);
      return count;
    } 
    count++;
  }
  std::cout << "could not find move " << s << std::endl;
  return count;
}


// pop from on a list O(1)
std::string Customer::return_oldest() {
  if (movies.size() == 0) return "";
  std::string answer = movies.front();
  movies.pop_front();
  return answer;
}

// pop back on a list O(1)
std::string Customer::return_newest() {
  if (movies.size() == 0) return "";
  std::string answer = movies.back();
  movies.pop_back();
  return answer;
}


// O(3) loop through all the movies held
// O(p) loop through the movies in the preference list
std::ostream& operator<<(std::ostream &ostr, const Customer& p) {
  if (p.movies.size() == 0) {
    ostr << p.get_name() << " has no movies" << std::endl;
    } else {
    ostr << p.get_name() << " has " << p.movies.size() << " movies:" << std::endl;
    for (std::list<std::string>::const_iterator itr = p.movies.begin();
         itr != p.movies.end(); itr++) {
      ostr << "    " << *itr << std::endl;
    }
  }
  if (p.preferences.size() > 0) {
    ostr << "  preference list:" << std::endl;
    for (std::list<std::string>::const_iterator itr = p.preferences.begin();
         itr != p.preferences.end(); itr++) {
      ostr << "    " << *itr << std::endl;
    }
  }

  // tracking extra statistics about the movies received & preferences
  /*
  ostr << "  total movies rented: " << p.total_rented << std::endl;
  if (p.total_rented > 0) {
    ostr << "  first choice: " << p.received_first_choice << " = "
        << p.received_first_choice/double(p.total_rented)*100 << "%" << std::endl;
  }
  ostr << "  days without receiving a new movie: " << p.days_waiting << std::endl;
  */  
  return ostr;
}
