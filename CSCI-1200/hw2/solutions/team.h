#ifndef _TEAM_H_
#define _TEAM_H_

#include <string>

// ============================================================

class Team {
public:

  // CONSTRUCTOR
  Team(const std::string& n);
  
  // ACCESSORS
  const std::string& getName() const { return name; }
  int getWins() const { return wins; }
  int getLosses() const { return losses; }
  int getTies() const { return ties; }
  int getGoals() const { return goals; }
  int getPenalties() const { return penalties; }
  double getWinPercentage() const;
    
  // MODIFIERS
  void addGoals(int g) { goals+=g; }
  void addWin() { wins++; }
  void addLoss() { losses++; }
  void addTie() { ties++; }
  void addPenalty() { penalties++; }
  
private:
  // PRIVATE MEMBER VARIABLES
  std::string name;
  int goals;
  int wins;
  int losses;
  int ties;
  int penalties;
};

// helper functions for sorting
bool sort_by_wins(const Team &a, const Team &b);
 
// ============================================================

#endif

