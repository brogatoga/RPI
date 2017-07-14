/**
 * Homework 2 - Hockey Classes
 * Ryan Lin (linr2@rpi.edu)
 *
 * This program can parse hockey data from text files and then
 * generate three tables detailing the overall summary of the games
 * for that season. These tables provide information on the teams, 
 * players, and violations involved in the games. 
 *
 * Usage: ./hockey <input_file> <output_file>
 *
**/

#include "include.h"
#include "game_collection.h"


// Function prototypes
void createTeamTable(table &, GameCollection &); 
void createPlayerTable(table &, GameCollection &); 
void createCustomTable(table &, GameCollection &);

void readData(GameCollection &, std::istream &);
void error(std::string, const std::string &); 


// Main function, entrypoint of the application
int main(int argc, char *argv[])
{
	std::ofstream out_file(argv[2]);
	std::ifstream in_file(argv[1]);
	GameCollection games; 
	vv_string teamTable, playerTable, customTable; 

	if (argc != 3) {
		std::cerr << "Usage: " << argv[0] << " <input_file> <output_file>" << std::endl;
		return 1;
	}

	if (!out_file.good()) {
		std::cerr << "Error: unable to open output file '" << argv[2] << "' for writing to" << std::endl;
		return 1;
	}

	if (!in_file.good()) {
		std::cerr << "Error: unable to open or read input file '" << argv[1] << "'" << std::endl;
		return 1;
	}


	// Read data from the input file and store it in the vector of Games
	readData(games, in_file);

	// Generate the table data for the three tables
	createTeamTable(teamTable, games);
	createPlayerTable(playerTable, games);
	createCustomTable(customTable, games);

	// Write formatted tables to the output stream
	writeTable(teamTable, out_file);
	out_file << std::endl;
	writeTable(playerTable, out_file);
	out_file << std::endl;
	writeTable(customTable, out_file);

	return 0; 
}


// Reads data from specified input stream and stores it in the passed vector
void readData(GameCollection &games, std::istream &in_file)
{
	State state = START;   // Current state that the parser is in
	std::string input;     // String where the next input is read into
	Game game;             // Game object to store collected data into
	Action action;         // Action object to store collected action data into
	int period = 0; 	   // Current period 
	int n;                 // Temporary integer variables 
	std::string s;         // Temporary string variables


	// Loop until there are no more inputs left to process
	while (in_file >> input) {
		switch (state) {
			case START: {
				if (removeLast(input).length() > 0 && input[input.length()-1] == ',') {
					s += input; 
					state = MONTH; 
				}
				else error("At starting state, expected valid DoW but received ? instead", input); 
				break;
			}

			case MONTH: {
				if (monthToInt(input) != -1) {
					s += (" " + input); 
					state = DAY; 
				}
				else error("? is not a valid month", input);
				break;
			}

			case DAY: {
				n = atoi(removeLast(input).c_str());  
				if (n >= 1 && n <= 31 && input[input.length()-1] == ',') {
					s += (" " + input); 
					state = YEAR; 
				}
				else error("? is not a valid day", input); 
				break;
			}

			case YEAR: { 
				n = atoi(input.c_str()); 
				if (n >= 0 && n <= 3000) {
					s += (" " + input); 
					game.setDate(s); 
					s = ""; 
					state = AWAYTEAM; 
				}
				else error("? is not a valid year", input); 
				break;
			}

			case AWAYTEAM: {
				game.setAwayTeam(input);
				state = AT; 
				break;
			}

			case AT: {
				if (input == "at" || input == "vs.")
					state = HOMETEAM;
				else error("Expected 'at' or 'vs.', received ? instead", input); 
				break;
			}

			case HOMETEAM: {
				game.setHomeTeam(input);
				state = LINE_START; 
				break;
			}

			case LINE_START: {
				if (input == "PERIOD") state = PERIOD_NUMBER; 
				else if (input == "FINAL") state = FINAL; 
				else if (input == "OVERTIME") period = 4; 
				else if (validTime(input)) {
					action.setTime(input);
					state = TEAM; 
				}
				else error("Expected 'PERIOD', 'FINAL', 'OVERTIME' or valid time, received ? instead", input);
				break;
			}

			case TIME: {
				if (validTime(input)) {
					action.setTime(input);  
					state = TEAM; 
				}
				else error("? is not a valid time", input); 
				break;
			}

			case PERIOD_NUMBER: {
				n = atoi(input.c_str()); 
				if (n >= 1 && n <= 3) {
					period = n; 
					state = LINE_START; 
				}
				else error("? is not a valid period number", input); 
				break;
			}

			case TEAM: {
				action.setTeam(input);
				state = ACTION_TYPE;
				break;
			}

			case ACTION_TYPE: {
				if (input == "goal") { 
					action.setType(ACTION_GOAL); 
					state = GOAL_PLAYER;
				} 
				else if (input == "penalty") { 
					action.setType(ACTION_PENALTY);
					state = PENALTY_PLAYER;
				}
				else error("? is not a valid action type, expected 'goal' or 'penalty", input);
				break;
			}

			case GOAL_PLAYER: {
				action.setPlayer(input);
				state = LEFT_PAREN; 
				break;
			}

			case LEFT_PAREN: {
				if (input == "(") state = INSIDE_ASSIST; 
				else error("Expected '(', received ? instead", input); 
				break;
			}

			case INSIDE_ASSIST: {
				if (input == ")") {
					game.addAction(period, action); 
					action.clear(); 
					state = LINE_START; 
				}
				else {
					action.setFirstAssist(input);
					state = FIRST_ASSIST; 
				}
				break;
			}

			case FIRST_ASSIST: {
				if (input == ")") {
					game.addAction(period, action);
					action.clear(); 
					state = LINE_START; 
				}
				else {
					action.setSecondAssist(input);
					state = SECOND_ASSIST; 
				}
				break;
			}

			case SECOND_ASSIST: {
				if (input == ")") {
					game.addAction(period, action);
					action.clear(); 
					state = LINE_START; 
				}
				else error("Cannot add third assist, expected ')'", input); 
				break;
			}

			case PENALTY_PLAYER: {
				action.setPlayer(input);
				state = PENALTY_TIME; 
				break;
			}

			case PENALTY_TIME: {
				if (validTime(input)) {
					action.setPenaltyTime(input);
					state = VIOLATION;
				}
				else error("Expected valid penalty time MM:SS, received ? instead", input); 
				break;
			}

			case VIOLATION: {
				action.setViolation(input); 
				game.addAction(period, action);
				action.clear(); 
				state = LINE_START; 
				break;
			}

			case FINAL: {
				if (input == game.getAwayTeam()) state = AWAYTEAM_NAME; 
				else error("Expected away team name, received ? instead", input); 
				break;
			}

			case AWAYTEAM_NAME: {
				n = atoi(input.c_str()); 
				if (n >= 0) {
					game.setAwayScore(n);
					state = AWAYTEAM_SCORE;  
				} 
				else error("? is not a valid score for the away team", input);
				break;
			}

			case AWAYTEAM_SCORE: {
				if (input == game.getHomeTeam()) state = HOMETEAM_NAME; 
				else error("Expected home team name, received ? instead", input); 
				break;
			}

			case HOMETEAM_NAME: {
				n = atoi(input.c_str()); 
				if (n >= 0) {
					game.setHomeScore(n);
					games.add(game); 
					game.clear(); 
					state = START; 
				}
				else error("? is not a valid score for the home team", input); 
				break;
			}
		}
	}
}


// Displays an error message to the output stream
void error(std::string message, const std::string &replace)
{
	int pos = message.find_first_of("?");   
	int length = message.length(); 

	if (pos != std::string::npos && pos >= 0 && pos < length) {
		std::string beginning = message.substr(0, pos);
		std::string ending = message.substr((pos+1 <= length-1) ? pos+1 : length-1);
		message = beginning + replace + ending; 
	}
	
	std::cout << "Error: " << message << std::endl;
	system("read");    // wait for user key press before continuing (equivalent of "pause" on windows systems)
}


// Format the given floating point number as a string in the form X.XX
std::string formatDecimalString(float n)
{
	n = floor(n * 100 + 0.5) / 100;      // round to nearest integer
	std::string result = toString(n); 

	// Format result as (1|0).XX
	if (result.length() == 1) 
		result += '.'; 
	while (result.length() < 4)
		result += '0'; 

	return result.substr(0, 4);
}


// Sort: Win% DESC, Goal DESC, Team ASC
bool sortTeamTable(const std::vector<std::string> &a, const std::vector<std::string> &b)
{
	float wp1 = stringToFloat(a[4]),    // a[4] is the win percentage in 0.XX form
		  wp2 = stringToFloat(b[4]); 

	int goal1 = stringToInt(a[5]),
		goal2 = stringToInt(b[5]);

	std::string name1 = a[0],	          
				name2 = b[0]; 

	if (wp1 == wp2) {
		if (goal1 == goal2)
			return name1 < name2;
		return goal1 > goal2;
	}
	return wp1 > wp2; 
}


// Sort: goals+assists DESC, penalties ASC, name ASC
bool sortPlayerTable(const std::vector<std::string> &a, const std::vector<std::string> &b)
{
	int total1 = stringToInt(a[2]) + stringToInt(a[3]),
	  	total2 = stringToInt(b[2]) + stringToInt(b[3]); 

	int penalty1 = stringToInt(a[4]),
		penalty2 = stringToInt(b[4]); 

	std::string name1 = a[0],
			    name2 = b[0]; 

	if (total1 == total2) {
		if (penalty1 == penalty2)
			return name1 < name2;
		return penalty1 < penalty2;
	} 
	return total1 > total2; 
}


// Sort: frequency DESC, name ASC
bool sortCustomTable(const std::vector<std::string> &a, const std::vector<std::string> &b)
{
	int freq1 = stringToInt(a[1]),
		freq2 = stringToInt(b[1]); 

	std::string name1 = a[0],
				name2 = b[0]; 

	if (freq1 == freq2)
		return name1 < name2; 
	return freq1 > freq2; 
}


// Generates the data to display the team table
// Format: Team W L T Win% Goals Pelanties
void createTeamTable(table &t, GameCollection &games)
{
	std::vector<std::string> teams = games.getTeams();
	std::vector<int> teamData(TEAM_DATASIZE); 
	const int COLUMNS = 7; 
	int size = teams.size(); 

	std::string header[] = {"Team Name", "W", "L", "T", "Win%", "Goals", "Penalties"};
	addRowToTable(t, header, COLUMNS);

	for (int c = 0; c < size; c++) {
		teamData = games.getTeamDataFor(teams[c]);
		if (teamData.size() == 0)
			continue;

		int w = teamData[TEAM_WINS], l = teamData[TEAM_LOSSES], ti = teamData[TEAM_TIES],
		g = teamData[TEAM_GOALS], p = teamData[TEAM_PENALTIES];

		float winp = (w + 0.5 * ti) / (w + l + ti); 
		std::string winpString = formatDecimalString(winp);

		std::string row[] = { teams[c], toString(w), toString(l), toString(ti), winpString, toString(g), toString(p) };
		addRowToTable(t, row, COLUMNS);  
	}
	sort(t.begin()+1, t.end(), sortTeamTable); 
} 


// Generates the data to display the player table
// Format: Player Team Goals Assists Penalties
void createPlayerTable(table &t, GameCollection &games)
{
	std::vector<std::string> players = games.getPlayers();
	std::vector<int> playerData(PLAYER_DATASIZE); 
	std::string playerTeam; 
	const int COLUMNS = 5; 
	int size = players.size(); 

	std::string header[] = { "Player Name", "Team", "Goals", "Assists", "Penalties" }; 
	addRowToTable(t, header, COLUMNS); 

	for (int c = 0; c < size; c++) {
		playerData = games.getPlayerDataFor(players[c]);
		playerTeam = games.getTeamFor(players[c]); 

		if (playerData.size() == 0 || playerTeam.length() == 0)
			continue; 

		std::string row[] = { players[c], playerTeam, toString(playerData[PLAYER_GOALS]),
			toString(playerData[PLAYER_ASSISTS]), toString(playerData[PLAYER_PENALTIES]) };
		
		addRowToTable(t, row, COLUMNS);
	}
	sort(t.begin()+1, t.end(), sortPlayerTable); 
}


// Generates the data to display the custom table (which is a frequency table of all the violations)
// Format: ViolationName Frequency
void createCustomTable(table &t, GameCollection &games)
{
	std::vector<std::string> violations = games.getViolations();
	std::vector<std::string> row;
	const int COLUMNS = 2; 
	int size = violations.size(); 

	std::string header[] = { "Violation Name", "Frequency" }; 
	addRowToTable(t, header, COLUMNS); 

	for (int c = 0; c < size; c++) {
		int frequency = games.getViolationFrequencyFor(violations[c]); 
		if (frequency <= 0)
			continue; 

		std::string row[] = { violations[c], toString(frequency) };
		addRowToTable(t, row, COLUMNS);
	}
	sort(t.begin()+1, t.end(), sortCustomTable); 
}