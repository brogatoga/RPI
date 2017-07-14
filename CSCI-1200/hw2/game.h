#ifndef GAME_H
#define GAME_H

#include "include.h"
#include "action.h"

// The Game class stores information about each hockey game, including the date, scores, teams involved, 
// and also contains a record of all the actions performed in each of the four periods. 

class Game
{
private:
	std::string date;         // {day_of_week}, {month} {day}, {year}
	std::string home_team;    
	std::string away_team;    
 
	int home_score; 
	int away_score; 

	std::vector<Action> actions;

public: 
	Game(); 
	void clear(); 
	void addAction(int, const Action &);
	

	// Mutators
	void setDate(const std::string &); 
	void setHomeTeam(const std::string &); 
	void setAwayTeam(const std::string &); 
	void setHomeScore(int); 
	void setAwayScore(int); 


	// Accessors
	const std::vector<Action>& getActions() const;  
	const std::string& getDate() const; 
	const std::string& getHomeTeam() const; 
	const std::string& getAwayTeam() const; 
	int getHomeScore() const;
	int getAwayScore() const;
	
}; 


#endif