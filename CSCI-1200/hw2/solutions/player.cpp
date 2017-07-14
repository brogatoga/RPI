#include "player.h"


// CONSTRUCTOR
Player::Player(const std::string &n, const std::string &t) {
  name = n;
  team = t;
  goals = 0;
  assists = 0;
  penalties = 0;
}


// Sorts players first by # goals + # of assists (higher # first)
//               secondarily by # of penaltys (lower # first)
//               thirdly by name, alphabetically
bool sort_by_goals(const Player &a, const Player &b) {
  if ((a.getGA() > b.getGA()) ||
      (a.getGA() == b.getGA() && a.getPenalties() < b.getPenalties()) ||
      (a.getGA() == b.getGA() && a.getPenalties() == b.getPenalties() &&  a.getName() < b.getName())) { 
    return true;
  }
  return false;
}
