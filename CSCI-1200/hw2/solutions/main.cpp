#include <string>
#include <vector>
#include <iostream>
#include <fstream>
#include <cassert>
#include <iomanip>
#include <ios>
#include <algorithm>

// two custom classes
#include "player.h"
#include "team.h"


// ============================================================


// Helper function to find the index of a team by name from the teams vector
int find_team_index(std::vector<Team> &teams, const std::string &name) {
  for (unsigned int i = 0; i < teams.size(); i++) {
    if (teams[i].getName() == name) { return i; }
  }
  // if the team doesn't already exist, make a new one
  teams.push_back(Team(name));
  return teams.size()-1;
}


// Helper function to find the index of a player by name from the players vector
int find_player_index(std::vector<Player> &players, const std::string &name, const std::string &team) {
  // bench is not a player
  if (name == "BENCH") return -1;
  for (unsigned int i = 0; i < players.size(); i++) {
    if (players[i].getName() == name) { 
      // make sure to also check the team name (in case multiple players have the same name)
      if (players[i].getTeam() == team) {
        return i; 
      }
    }
  }
  // if the player doesn't exist, add a new one
  players.push_back(Player(name,team));
  return players.size()-1;
}


// ============================================================


// The long and messy function for reading the input data
void read_data(std::vector<Team> &teams, std::vector<Player> &players, std::ifstream &in_str) {
  // variables for storing the game data
  std::string day, month, date, year;
  // OUTER LOOP: read each game, read first line of each game
  while (in_str >> day >> month >> date >> year) {

    // variables for storing the data
    std::string token, away, home;
    // read second line of the game
    in_str >> away >> token >> home;
    assert (token == "at" || token == "vs.");
    // get the index of each team
    int ai = find_team_index(teams,away);
    int hi = find_team_index(teams,home);
    // count the goals (a sanity check)
    int home_total=0;
    int away_total=0;

    // read in each line of data for the game
    while (in_str >> token) {

      // the penalty and goal data begins with the time (each time has a ':' char)
      if (token.find(":") != std::string::npos) {
        // read in details about the event type and team
        std::string which_team, what_event;
        in_str >> which_team >> what_event;
        assert (which_team == home || which_team == away);

        if (what_event == "penalty") {
          // read in the details for the penalty
          std::string player;
          std::string penalty_length;
          std::string penalty;
          in_str >> player >> penalty_length >> penalty;
          // look up the player
          int pi = find_player_index(players,player,which_team);
          // record the penalty for both team & player
          if (pi != -1) players[pi].addPenalty();
          if (which_team == away) teams[ai].addPenalty(); 
          else teams[hi].addPenalty();

        } else {
          assert (what_event == "goal");
          // record the goal for the team
          if (which_team == away) away_total++;
          else home_total++;
          // read in the details for the goal
          std::string player;
          //std::vector<std::string> player_assists;
          in_str >> player;
          int pi = find_player_index(players,player,which_team);
          if (pi != -1) players[pi].addGoal();
          // read in the players who assisted
          in_str >> token;
          assert (token == "(");
          while (in_str >> token) {
            if (token == ")") break;
            int pi = find_player_index(players,token,which_team);
            if (pi != -1) players[pi].addAssist();
          }
        }

      } else if (token == "PERIOD") {
        // also read in which period
        in_str >> token;
      } else if (token == "OVERTIME") {
      } else if (token == "FINAL") {
        break;
      } else {
        std::cerr << "ERROR: UNKNOWN TOKEN " << token << std::endl;
        exit(1);
      }
    }

    // read in the final score
    int total;
    in_str >> token; 
    assert (token == away);
    in_str >> total;
    assert (away_total == total);
    teams[ai].addGoals(away_total);
    in_str >> token; 
    assert (token == home);
    in_str >> total;
    assert (home_total == total);
    teams[hi].addGoals(home_total);

    // record the game result for each team
    if (away_total > home_total) {
      teams[ai].addWin();
      teams[hi].addLoss();
    } else if (away_total < home_total) {
      teams[ai].addLoss();
      teams[hi].addWin();
    } else {
      assert (away_total == home_total);
      teams[ai].addTie();
      teams[hi].addTie();
    }
  }
}


// ============================================================


// Helper function to print the first table (data about teams)
void print_all_teams(const std::vector<Team> &teams, std::ofstream &out_str) {
  // find the longest team name
  unsigned int longest = 0;
  for (unsigned int i = 0; i < teams.size(); i++) {
    const Team &t = teams[i];
    if (t.getName().size() > longest) longest = t.getName().size();
  }
  // print the table header
  out_str << std::left << std::setw(longest+1) << "Team Name" << " " 
          << std::right << std::setw(3) << "W" << " " 
          << std::setw(3) << "L" << " " 
          << std::setw(3) << "T" << " " 
          << std::setw(6) << "Win%" << " " 
          << std::setw(6) << "Goals" << " "
          << std::setw(10) << "Penalties" << std::endl;
  // print out data for each team
  for (unsigned int i = 0; i < teams.size(); i++) {
    const Team &t = teams[i];
    out_str << std::left << std::setw(longest+1) << t.getName() << " " 
            << std::right << std::setw(3) << t.getWins() << " " 
            << std::setw(3) << t.getLosses() << " " 
            << std::setw(3) << t.getTies() << " " 
            << std::setw(6) << std::fixed << std::setprecision(2) << t.getWinPercentage() << " " 
            << std::setw(6) << t.getGoals() << " "
            << std::setw(10) << t.getPenalties() << std::endl;
  }
}


// Helper function to print the second table (data about players)
void print_all_players(const std::vector<Player> &players, std::ofstream &out_str) {
  // find the longest team name an the longest player name
  unsigned int longest_name = 0;
  unsigned int longest_team = 0;
  for (unsigned int i = 0; i < players.size(); i++) {
    const Player &p = players[i];
    if (p.getName().size() > longest_name) longest_name = p.getName().size();
    if (p.getTeam().size() > longest_team) longest_team = p.getTeam().size();
  }
  // print out the table header
  out_str << std::left << std::setw(longest_name+1) << "Player Name" << " " 
          << std::left << std::setw(longest_team+1) << "Team" << " " 
          << std::right << std::setw(6) << "Goals" << " " 
          << std::setw(8) <<  "Assists" << " " 
          << std::setw(10) << "Penalties" << std::endl;

  // print out data for each player
  for (unsigned int i = 0; i < players.size(); i++) {
    const Player &p = players[i];
    out_str << std::left << std::setw(longest_name+1) << p.getName() << " " 
            << std::left << std::setw(longest_team+1) << p.getTeam() << " " 
            << std::right << std::setw(6) << p.getGoals() << " " 
            << std::setw(8) << p.getAssists() << " "
            << std::setw(10) << p.getPenalties() << std::endl;
  }
}


// ============================================================


int main(int argc, char* argv[]) {

  // check the number of arguments
  if (argc != 3 && 
      argc != 4){
    std::cerr << "Usage: " << argv[0] << " <in-file> <out-file>\n";
    return 1;
  }

  // open and test the input file
  std::ifstream in_str(argv[1]);
  if (!in_str) {
    std::cerr << "Could not open " << argv[1] << " to read\n";
    return 1;
  }

  // open and test the output file
  std::ofstream out_str(argv[2]);
  if (!out_str) {
    std::cerr << "Could not open " << argv[2] << " to write\n";
    return 1;
  }

  // to store all the data for all the teams and players
  std::vector<Team> teams;
  std::vector<Player> players;

  read_data(teams,players,in_str);

  // sort and print out the teams data
  sort(teams.begin(),teams.end(),sort_by_wins);
  print_all_teams(teams,out_str);
  out_str << std::endl;

  // sort and print out the players data
  sort(players.begin(),players.end(),sort_by_goals);
  print_all_players(players,out_str);
  out_str << std::endl;

  out_str << "< ** YOUR STATISTIC GOES HERE ** >" << std::endl;
}

// ============================================================
