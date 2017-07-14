#include "team.h"


// CONSTRUCTOR
Team::Team(const std::string& n) {
  name = n;
  wins = 0;
  losses = 0;
  ties = 0;
  penalties = 0;
}


// CALCULATE WIN PERCENTAGE 
// a bit of math :)
double Team::getWinPercentage() const { 
  return 
    (double)(wins + 0.5 * ties) / 
    double (wins + ties + losses); 
}


// Sorts teams first by win percentage (higher % first)
//             secondarily by # of goals (higher # first)
//             thirdly by name, alphabetically
bool sort_by_wins(const Team &a, const Team &b) {
  if (a.getWinPercentage() > b.getWinPercentage() ||
      (a.getWinPercentage() == b.getWinPercentage() && a.getGoals() > b.getGoals()) ||
      (a.getWinPercentage() == b.getWinPercentage() && a.getGoals() == b.getGoals() && 
       a.getName() < b.getName())) {
    return true;
  }
  return false;
}
