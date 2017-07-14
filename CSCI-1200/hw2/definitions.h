#ifndef DEFINITIONS_H
#define DEFINITIONS_H


// Constants for the Action class
#define ACTION_GOAL     1
#define ACTION_PENALTY  2


// Constants for the GameCollection class
#define TEAM_DATASIZE   5
#define TEAM_WINS       0
#define TEAM_LOSSES     1
#define TEAM_TIES       2
#define TEAM_GOALS      3
#define TEAM_PENALTIES  4

#define PLAYER_DATASIZE   3
#define PLAYER_GOALS      0
#define PLAYER_ASSISTS    1
#define PLAYER_PENALTIES  2


// Shorter names to declare a vector of a vector of a string/int
typedef std::vector< std::vector<std::string> > vv_string;
typedef std::vector< std::vector<int> > vv_int; 
typedef vv_string table; 


// List of possible states that the input parser can be in
enum State {
	START,
	MONTH,
	DAY,
	YEAR,
	AWAYTEAM,
	AT,
	HOMETEAM,
	LINE_START,
	TIME,
	PERIOD_NUMBER,
	TEAM,
	ACTION_TYPE,
	INSIDE_ASSIST,
	GOAL_PLAYER,
	LEFT_PAREN,
	FIRST_ASSIST,
	SECOND_ASSIST,
	PENALTY_PLAYER,
	PENALTY_TIME,
	VIOLATION,
	FINAL,
	AWAYTEAM_NAME,
	AWAYTEAM_SCORE,
	HOMETEAM_NAME
};


// Function declarations for some utility functions
std::string dowToString(int);
std::string monthToString(int);
int dowToInt(const std::string &); 
int monthToInt(const std::string &); 
bool validTime(const std::string &); 
void getTime(const std::string &, int &, int &); 

std::string removeFirst(const std::string &); 
std::string removeLast(const std::string &); 
std::string toString(int);
std::string toString(float);

int stringToInt(const std::string &);
float stringToFloat(const std::string &);

void writeTable(const table &, std::ostream &); 
void addRowToTable(table &, std::string *, int);


#endif