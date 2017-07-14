#include "game.h"


// Default constructor for Game objects
Game::Game()
{
	clear(); 
}


// Resets the object back to its default state
void Game::clear()
{
	date = home_team = away_team = ""; 	
	home_score = away_score = 0; 
	actions.clear();
}


// Adds an entry for a goal or penalty to a specified period
void Game::addAction(int period, const Action &action)
{
	if (period < 1 || period > 4)
		return;

	Action a(action);
	a.setPeriod(period);
	actions.push_back(a);
}


// Mutators
void Game::setDate(const std::string &s) { date = s; } 
void Game::setHomeTeam(const std::string &s) { home_team = s; }
void Game::setAwayTeam(const std::string &s) { away_team = s; }
void Game::setHomeScore(int n) { home_score = n; }
void Game::setAwayScore(int n) { away_score = n; }


// Accessors
const std::vector<Action>& Game::getActions() const { return actions; }
const std::string& Game::getDate() const { return date; }
const std::string& Game::getHomeTeam() const { return home_team; }
const std::string& Game::getAwayTeam() const { return away_team; }
int Game::getHomeScore() const { return home_score; }
int Game::getAwayScore() const { return away_score; }