#include "game_collection.h"


// Adds a new game to the collection
void GameCollection::add(const Game &game)
{
	games.push_back(Game(game));
	addTeamData(game); 
	addPlayerData(game); 
	addViolationData(game);
}


// Adds the team data for the provided game
void GameCollection::addTeamData(const Game &game)
{
	// Search for the team, does it already exist? 
	std::string homeTeamName = game.getHomeTeam();
	std::string awayTeamName = game.getAwayTeam(); 
	int homeTeamIndex = getTeamIndex(homeTeamName); 
	int awayTeamIndex = getTeamIndex(awayTeamName); 


	// If team does not exist, then create the records for it
	if (homeTeamIndex == -1) {
		teams.push_back(homeTeamName); 
		teamData.push_back(std::vector<int>(TEAM_DATASIZE)); 
		homeTeamIndex = teams.size() - 1; 
	}
	
	if (awayTeamIndex == -1) {
		teams.push_back(awayTeamName); 
		teamData.push_back(std::vector<int>(TEAM_DATASIZE)); 
		awayTeamIndex = teams.size() - 1; 
	}


	// Set the wins, losses, and ties
	std::vector<int> &homeTeamData = teamData[homeTeamIndex]; 
	std::vector<int> &awayTeamData = teamData[awayTeamIndex]; 
	int homeScore = game.getHomeScore();
	int awayScore = game.getAwayScore(); 

	if (homeScore > awayScore) {
		homeTeamData[TEAM_WINS] ++; 
		awayTeamData[TEAM_LOSSES] ++; 
	}
	else if (homeScore < awayScore) {
		awayTeamData[TEAM_WINS] ++; 
		homeTeamData[TEAM_LOSSES] ++; 
	}
	else {
		homeTeamData[TEAM_TIES] ++; 
		awayTeamData[TEAM_TIES] ++; 
	}


	// Iterate through the actions and sum up the goals and penalties
	std::vector<Action> gameActions = game.getActions(); 
	int actionSize = gameActions.size(); 

	for (int c = 0; c < actionSize; c++) {
		if (gameActions[c].getTeam() == homeTeamName) {
			if (gameActions[c].getType() == ACTION_GOAL)
				homeTeamData[TEAM_GOALS] ++; 
			else
				homeTeamData[TEAM_PENALTIES] ++; 
		}
		else {       // otherwise action is from the away team
			if (gameActions[c].getType() == ACTION_GOAL)
				awayTeamData[TEAM_GOALS] ++; 
			else
				awayTeamData[TEAM_PENALTIES] ++; 
		}
	}
}


// Adds the player data for the specified game
void GameCollection::addPlayerData(const Game &game)
{
	std::vector<Action> actions = game.getActions(); 
	std::string playerName, teamName; 
	int actionSize = actions.size(); 
	int playerIndex = -1; 


	// Iterate through all of the actions in the game
	for (int c = 0; c < actionSize; c++) {
		playerName = actions[c].getPlayer(); 
		teamName = actions[c].getTeam(); 
		playerIndex = getPlayerIndex(playerName); 

		// Apparently BENCH is not a valid player and it shouldn't appear in the output
		if (playerName == "BENCH")
			continue; 

		// If player does not exist, then create a record for this player
		if (playerIndex == -1) {
			players.push_back(playerName); 
			playerTeams.push_back(teamName); 
			playerData.push_back(std::vector<int>(PLAYER_DATASIZE)); 
			playerIndex = players.size() - 1; 
		}

		// Set the team, goals, assists, and penalties
		std::vector<int> &data = playerData[playerIndex]; 
		playerTeams[playerIndex] = teamName; 
		int assistCount = actions[c].getAssistCount(); 

		if (actions[c].getType() == ACTION_GOAL)
			data[PLAYER_GOALS] ++; 
		else
			data[PLAYER_PENALTIES] ++; 


		for (int d = 0; d < assistCount; d++) {
			std::string assistName = (d == 0) ? actions[c].getFirstAssist() : actions[c].getSecondAssist(); 
			int index = getPlayerIndex(assistName); 

			if (index == -1) {
				players.push_back(assistName); 
				playerTeams.push_back(teamName); 
				playerData.push_back(std::vector<int>(PLAYER_DATASIZE)); 
				index = players.size() - 1; 
			}

			std::vector<int> &assistData = playerData[index]; 
			assistData[PLAYER_ASSISTS] ++; 
		}
	}
}


// Adds the violations data for the specified game
void GameCollection::addViolationData(const Game &game)
{
	std::vector<Action> actions = game.getActions(); 
	int actionSize = actions.size(); 

	for (int c = 0; c < actionSize; c++) {
		if (actions[c].getType() != ACTION_PENALTY)
			continue; 

		std::string violation = actions[c].getViolation(); 
		int index = getViolationIndex(violation); 

		if (index == -1) {
			violations.push_back(violation); 
			violationFrequency.push_back(1); 
			index = violations.size() - 1; 
		}
		else violationFrequency[index] ++; 
	}
}


// Customizable version of the getIndex() function
int GameCollection::getIndexGeneric(const std::string &s, const std::vector<std::string> &searchVector) const
{
	int size = searchVector.size(); 
	for (int c = 0; c < size; c++) {
		if (searchVector[c] == s)
			return c;
	}
	return -1; 
}


// Accessors
int GameCollection::size() const { return games.size(); }
const std::vector<std::string>& GameCollection::getTeams() const { return teams; }
const std::vector<std::string>& GameCollection::getPlayers() const { return players; }
const std::vector<std::string>& GameCollection::getViolations() const { return violations; }
int GameCollection::getTeamIndex(const std::string &s) const { return getIndexGeneric(s, teams); }
int GameCollection::getPlayerIndex(const std::string &s) const { return getIndexGeneric(s, players); }
int GameCollection::getViolationIndex(const std::string &s) const { return getIndexGeneric(s, violations); }


const std::vector<int> GameCollection::getTeamDataFor(const std::string &s) const 
{
	int index = getTeamIndex(s); 
	return (index != -1) ? teamData[index] : std::vector<int>(); 
}


const std::vector<int> GameCollection::getPlayerDataFor(const std::string &s) const
{
	int index = getPlayerIndex(s); 
	return (index != -1) ? playerData[index] : std::vector<int>(); 
}


const std::string GameCollection::getTeamFor(const std::string &s) const
{
	int index = getPlayerIndex(s); 
	return (index != -1) ? playerTeams[index] : ""; 
}


int GameCollection::getViolationFrequencyFor(const std::string &s) const
{
	int index = getViolationIndex(s); 
	return (index != -1) ? violationFrequency[index] : -1; 
}