#include <fstream>
#include <iostream>
#include <string>
#include <list>
#include <cassert>


// Simple class to store data for each dvd, including the name and how
// many copies.  This is also where data about how many times a dvd
// has been shipping can be stored (for further analysis).

class DVD {
public:

  // CONSTRUCTOR
  DVD(const std::string &n, int c);

  // ACCESSOR
  const std::string& get_name() const { return m_name; }
  bool available()const { 
    assert (m_shipped <= m_copies);
    return (m_shipped < m_copies); }

  // MODIFIER
  void shipped() { m_shipped++; assert (m_shipped <= m_copies); }
  void returned() { m_shipped--; assert (m_shipped >= 0); }
  void add_inventory(int copies) { m_copies += copies; }

  // PRINTING
  friend std::ostream& operator<<(std::ostream &ostr, const DVD& d);

private:
  // REPRESENTATION
  int times_rented;
  std::string m_name;
  int m_copies;
  int m_shipped;
};

// helper function
bool remove_from_list(std::list<std::string> &list, const std::string &x);
