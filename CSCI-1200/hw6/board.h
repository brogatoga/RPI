#ifndef BOARD_H
#define BOARD_H

#include <iostream>
#include <cassert>
#include <vector>
#include <string>

#include "position.h"
#include "robot.h"
#include "goal.h"
#include "vision_map.h"


// Board class holds information about:
//   - dimensions of board
//   - location of walls
//   - locations of all robots
//   - all goal locations

class Board
{
public:
    Board(int num_rows, int num_cols);

    int  getRows() const { return rows; }
    int  getCols() const { return cols; }
    bool getHorizontalWall(double r, int c) const;
    bool getVerticalWall(int r, double c) const;

    unsigned int numRobots() const { return robots.size(); }
    char         getRobot(int i) const { assert (i >= 0 && i < (int)numRobots()); return robots[i].which; }
    Position     getRobotPosition(int i) const { assert (i >= 0 && i < (int)numRobots()); return robots[i].pos; }
    
    unsigned int numGoals() const { return goals.size(); }
    char         getGoalRobot(int i) const { assert (i >= 0 && i < (int)numGoals()); return goals[i].which; }
    Position     getGoalPosition(int i) const { assert (i >= 0 && i < (int)numGoals()); return goals[i].pos; }

    void addHorizontalWall(double r, int c);
    void addVerticalWall(int r, double c);

    bool moveRobot(int i, char direction);
    bool moveRobot(char marker, char direction) { return moveRobot(getRIDFromMarker(marker), direction); }
    void placeRobot(const Position &p, char a);
    void updateRobotPosition(int i,  const Position& p); 
    void addGoal(const std::string &goal_robot, const Position &p);
 
    int  getRIDFromMarker(char marker) const; 
    void generateVisionMap(int rid, int max_moves = -1, const Position& dest = Position());
    void generateVisionMap(char marker, int max_moves = -1, const Position& dest = Position()) {
        generateVisionMap(getRIDFromMarker(marker), max_moves, dest); 
    }
    std::string directionToString(char direction) const;
    void pruneSolutions(); 
    
    void print() const;
    void printVisionMap(char marker) const;
    void printSolution(int max_moves = -1, bool all_solutions = false);

private:
    int rows;    // Height of board               
    int cols;    // Width of board

    std::vector<std::vector<char> > board;                  // Board character data
    std::vector<std::vector<bool> > vertical_walls;         // All vertical walls on the board
    std::vector<std::vector<bool> > horizontal_walls;       // All horizontal walls on the board

    std::vector<Robot> robots;                              // Record of all robots on the board
    std::vector<Goal> goals;                                // All goal spots on the board
    std::vector<std::vector<RobotMove> > solutions;         // All solutions to the current position from previous generateVisionMap() call

    char getspot(const Position &p) const;
    void setspot(const Position &p, char a);
    char isGoal(const Position &p) const;
    void visualize(int rid, int max_moves, int move_number = 1, const Position& dest = Position(), 
        VisionEntry path = VisionEntry()); 
};


#endif