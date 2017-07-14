#ifndef GAME_COLLECTION_H
#define GAME_COLLECTION_H

#include "include.h"
#include "game.h"


// The Game Collection class keeps tracks of the records of multiple games and
// their respective actions. As more games are added to the collection, it automatically
// computes statistics for individual games, teams, players, assists, violations, and more
// by aggregating all of the data together. 

class GameCollection
{
private:
	std::vector<Game> games; 

	std::vector<std::string> teams;
	vv_int teamData;     // apparently std::vector<int[X]> is not allowed, have to use this instead

	std::vector<std::string> players;
	std::vector<std::string> playerTeams;
	vv_int playerData;

	std::vector<std::string> violations;
	std::vector<int> violationFrequency;


	// Private methods, these should NEVER be called from outside the class
	int getIndexGeneric(const std::string &, const std::vector<std::string> &) const;
	int getTeamIndex(const std::string &) const; 
	int getPlayerIndex(const std::string &) const;
	int getViolationIndex(const std::string &) const; 

	void addTeamData(const Game &); 
	void addPlayerData(const Game &); 
	void addViolationData(const Game &); 


public:
	void add(const Game &); 
	int size() const; 

	const std::vector<std::string>& getTeams() const; 
	const std::vector<std::string>& getPlayers() const;
	const std::vector<std::string>& getViolations() const; 

	const std::vector<int> getTeamDataFor(const std::string &) const; 
	const std::vector<int> getPlayerDataFor(const std::string &) const;
	const std::string getTeamFor(const std::string &) const; 
	int getViolationFrequencyFor(const std::string &) const; 
}; 


#endif