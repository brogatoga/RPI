#ifndef _PLAYER_H_
#define _PLAYER_H_

#include <string>

// ============================================================

class Player {

public:

  // CONSTRUCTOR
  Player(const std::string &n, const std::string &t);
  
  // ACCESSORS
  const std::string& getName() const { return name; }
  const std::string& getTeam() const { return team; }
  int getGoals() const { return goals; }
  int getAssists() const { return assists; }
  int getGA() const { return goals + assists; }
  int getPenalties() const { return penalties; }

  // MODIFIERS
  void addGoal() { goals++; }
  void addAssist() { assists++; }
  void addPenalty() { penalties++; }
  
private:

  // PRIVATE MEMBER VARIABLES
  std::string name;
  std::string team;
  int goals;
  int assists;
  int penalties;
};

// helper functions for sorting
bool sort_by_goals(const Player &a, const Player &b);

// ============================================================
 
#endif
