#include "action.h"


// Default constructor for the Action class
Action::Action()
{
	clear(); 
}


// Reset the object back to its default state
void Action::clear()
{
	type = ACTION_GOAL; 
	period = 1; 
	team = time = player = 
	assists[0] = assists[1] = 
	violation = penaltyTime = ""; 
}


// Mutators 
void Action::setType(int n) { if (n == ACTION_GOAL || n == ACTION_PENALTY) type = n; }
void Action::setPeriod(int n) { period = n; }
void Action::setTime(const std::string &s) { time = s; }
void Action::setTeam(const std::string &s) { team = s; }
void Action::setPlayer(const std::string &s) { player = s; }
void Action::setFirstAssist(const std::string &s) { assists[0] = s; }
void Action::setSecondAssist(const std::string &s) { assists[1] = s; }
void Action::setViolation(const std::string &s) { violation = s; }
void Action::setPenaltyTime(const std::string &s) { penaltyTime = s; }


// Accessors
int Action::getType() const { return type; }
int Action::getPeriod() const { return period; }
int Action::getAssistCount() const { return (int)(assists[0].length() > 0) + (int)(assists[1].length() > 0); }
const std::string& Action::getTeam() const { return team; }
const std::string& Action::getPlayer() const { return player; }
const std::string& Action::getFirstAssist() const { return assists[0]; }
const std::string& Action::getSecondAssist() const { return assists[1]; }
const std::string& Action::getViolation() const { return violation; }