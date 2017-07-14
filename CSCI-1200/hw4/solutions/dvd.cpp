#include "dvd.h"

// custructor
DVD::DVD(const std::string &n, int c) 
: times_rented(0), m_name(n), m_copies(c), m_shipped(0) {}

// the print function
// O(1)
// note: if we stored and printed the different users who held each
// dvd, that would make this function O(k).
std::ostream& operator<<(std::ostream &ostr, const DVD& d) {
  ostr << d.get_name() << ":" << std::endl;
  int ch = d.m_shipped;
  int avail = d.m_copies-d.m_shipped;

  ostr << "  ";
  if (ch == 1) {
    ostr << ch << " copy checked out";
  }
  if (ch > 1) {
    ostr << ch << " copies checked out";
  }
  if (ch > 0 && avail > 0) {
    ostr << " and ";
  }
  if (avail == 1) {
    ostr << avail << " copy available";
  }
  if (avail > 1) {
    ostr << avail << " copies available";
  }
  ostr << std::endl;
  return ostr;
}
