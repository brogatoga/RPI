#ifndef ACTION_H
#define ACTION_H
#include "include.h"

// The Action class stores information about each goal or penalty that is involved
// in a game, including the time, team, players involved, etc. 

class Action
{
private:
	int type; 
	int period;                // 1-4 only

	std::string time;          // {minutes}:{seconds}
	std::string team;
	std::string player;
	
	std::string assists[2];    // { A1, ""} for single assist, { A1, A2} for double assists, { "", "" } for no assists
	std::string violation;
	std::string penaltyTime;   // {minutes}:{seconds}

public:
	Action();
	void clear();


	// Mutators 
	void setType(int); 
	void setPeriod(int);
	void setTime(const std::string &); 
	void setTeam(const std::string &); 
	void setPlayer(const std::string &); 
	void setFirstAssist(const std::string &); 
	void setSecondAssist(const std::string &);
	void setViolation(const std::string &); 
	void setPenaltyTime(const std::string &); 


	// Accessors
	int getType() const;
	int getPeriod() const;
	int getAssistCount() const; 
	const std::string& getTeam() const;
	const std::string& getPlayer() const;
	const std::string& getFirstAssist() const; 
	const std::string& getSecondAssist() const;
	const std::string& getViolation() const; 
}; 


#endif