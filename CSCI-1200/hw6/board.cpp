#include <iomanip>
#include <cstdlib>
#include <cmath>
#include <algorithm>
#include "board.h"


// Constructor, initializes a new Board object
Board::Board(int r, int c)
{
    rows = r; 
    cols = c; 

    board = std::vector<std::vector<char> >(rows,std::vector<char>(cols,' '));
    vertical_walls = std::vector<std::vector<bool> >(rows,std::vector<bool>(cols+1,false));
    horizontal_walls = std::vector<std::vector<bool> >(rows+1,std::vector<bool>(cols,false));

    // Create outer walls of the grid
    for (int i = 0; i < rows; i++)
        vertical_walls[i][0] = vertical_walls[i][cols] = true;

    for (int i = 0; i < cols; i++) 
        horizontal_walls[0][i] = horizontal_walls[rows][i] = true;
}


// Query the existance of a horizontal wall
bool Board::getHorizontalWall(double r, int c) const
{
    assert (fabs((r - floor(r))-0.5) < 0.005);
    assert (r >= 0.4 && r <= rows+0.6);
    assert (c >= 1 && c <= cols);
    return horizontal_walls[floor(r)][c-1];
}


// Query the existance of a vertical wall
bool Board::getVerticalWall(int r, double c) const
{
    assert (fabs((c - floor(c))-0.5) < 0.005);
    assert (r >= 1 && r <= rows);
    assert (c >= 0.4 && c <= cols+0.6);
    return vertical_walls[r-1][floor(c)];
}


// Add an interior horizontal wall
void Board::addHorizontalWall(double r, int c)
{
    assert (fabs((r - floor(r))-0.5) < 0.005);
    assert (r >= 0 && r <= rows);
    assert (c >= 1 && c <= cols);
    assert (horizontal_walls[floor(r)][c-1] == false);
    horizontal_walls[floor(r)][c-1] = true;
}


// Add an interior vertical wall
void Board::addVerticalWall(int r, double c)
{
    assert (fabs((c - floor(c))-0.5) < 0.005);
    assert (r >= 1 && r <= rows);
    assert (c >= 0 && c <= cols);
    assert (vertical_walls[r-1][floor(c)] == false);
    vertical_walls[r-1][floor(c)] = true;
}


// Returns the value of the marker at position P
char Board::getspot(const Position &p) const
{
    assert (p.row >= 1 && p.row <= rows);
    assert (p.col >= 1 && p.col <= cols);
    return board[p.row-1][p.col-1];
}


// Sets the marker at position P to A
void Board::setspot(const Position &p, char a)
{
    assert (p.row >= 1 && p.row <=  rows);
    assert (p.col >= 1 && p.col <= cols);
    board[p.row-1][p.col-1] = a;
}


// Returns the goal at the given position
char Board::isGoal(const Position &p) const
{
    assert (p.row >= 1 && p.row <= rows);
    assert (p.col >= 1 && p.col <= cols);
   
    for (unsigned int i = 0; i < goals.size(); i++) {
        if (p == goals[i].pos)
            return goals[i].which;
    }
    return ' ';
}


// Place a new robot with marker A at position P
void Board::placeRobot(const Position &p, char a)
{
    assert (p.row >= 1 && p.row <= rows);
    assert (p.col >= 1 && p.col <= cols);
    assert (getspot(p) == ' ');
    assert (isalpha(a) && isupper(a));

    for (unsigned int i = 0; i < robots.size(); i++)
        assert (robots[i].which != a);

    robots.push_back(Robot(p, a));
    setspot(p,a);
}


// Updates the "marker" of the robot on the board
void Board::updateRobotPosition(int i, const Position& p)
{
    if (i < 0 || i >= robots.size() || robots[i].pos == p)
        return;

    int old_row = robots[i].pos.row,
        old_col = robots[i].pos.col; 

    robots[i].pos.row = p.row; 
    robots[i].pos.col = p.col; 

    setspot(Position(old_row, old_col), ' '); 
    setspot(Position(robots[i].pos.row, robots[i].pos.col), robots[i].which);
}


// Moves the robot in one of four directions and returns true if it has moved, or false if it stays put
bool Board::moveRobot(int i, char direction)
{
    if (i < 0 || i >= robots.size())
        return false;

    int rx = robots[i].pos.col,
        ry = robots[i].pos.row; 
    char id = robots[i].which; 
    direction = toupper(direction); 


    // Moving north
    if (direction == 'N' && ry > 1) {
        int destination = 1; 
        for (int c = 0; c <= ry-1; c++) {
            if (getHorizontalWall(c+0.5, rx))
                destination = c+1; 
        }
        for (int c = 0; c < robots.size(); c++) {
            if (robots[c].which != id && robots[c].pos.col == rx
                && robots[c].pos.row < ry && robots[c].pos.row >= destination)
                destination = robots[c].pos.row + 1; 
        }
        updateRobotPosition(i, Position(destination, rx));
    }

    // Moving south 
    else if (direction == 'S' && ry < rows) {
        int destination = rows;
        for (int c = rows; c >= ry+1; c--) {
            if (getHorizontalWall(c-0.5, rx))
                destination = c-1; 
        }
        for (int c = 0; c < robots.size(); c++) {
            if (robots[c].which != id && robots[c].pos.col == rx
                && robots[c].pos.row > ry && robots[c].pos.row <= destination)
                destination = robots[c].pos.row - 1;  
        }
        updateRobotPosition(i, Position(destination, rx));  
    }

    // Moving west
    else if (direction == 'W' && rx > 1) {
        int destination = 1; 
        for (int c = 0; c <= rx-1; c++) {
            if (getVerticalWall(ry, c+0.5))
                destination = c+1;
        }
        for (int c = 0; c < robots.size(); c++) {
            if (robots[c].which != id && robots[c].pos.row == ry
                && robots[c].pos.col < rx && robots[c].pos.col >= destination)
                destination = robots[c].pos.col + 1; 
        }
        updateRobotPosition(i, Position(ry, destination)); 
    }

    // Moving east
    else if (direction == 'E' && rx < cols) {
        int destination = cols; 
        for (int c = cols; c >= rx+1; c--) {
            if (getVerticalWall(ry, c-0.5))
                destination = c-1;
        }
        for (int c = 0; c < robots.size(); c++) {
            if (robots[c].which != id && robots[c].pos.row == ry
                && robots[c].pos.col > rx && robots[c].pos.col <= destination)
                destination = robots[c].pos.col - 1; 
        }
        updateRobotPosition(i, Position(ry, destination)); 
    }

    // Invalid direction
    else {
        return false; 
    }

    // If robot's position has changed, then return true, otherwise return false
    return robots[i].pos != Position(ry, rx); 
}


// Returns the robot ID (index in the vector) given its alphabetical marker
int Board::getRIDFromMarker(char marker) const
{
    for (int c = 0; c < robots.size(); c++) {
        if (robots[c].which == marker)
            return c; 
    }
    return -1; 
}


// Adds a new goal to the board 
void Board::addGoal(const std::string &gr, const Position &p)
{
    // check that input data is reasonable
    assert (p.row >= 1 && p.row <= rows);
    assert (p.col >= 1 && p.col <= cols);

    char goal_robot;
    if (gr == "any") {
        goal_robot = '?';
    } else {
        assert (gr.size() == 1);
        goal_robot = gr[0];
        assert (isalpha(goal_robot) && isupper(goal_robot));
    }

    // verify that a robot of this name exists for this puzzle
    if (goal_robot != '?') {
        int robot_exists = false;
        for (unsigned int i = 0; i < robots.size(); i++) {
            if (getRobot(i) == goal_robot) 
                robot_exists = true;
        }
        assert (robot_exists);
    }
    
    // make sure we don't already have a robot at that location
    assert (isGoal(p) == ' ');

    // add this goal label and position to the vector of goals
    goals.push_back(Goal(p,goal_robot));
}


// Given a direction (N,S,W,E) outputs the full name of the direction
std::string Board::directionToString(char direction) const
{
    switch (direction) {
        case 'N': return "north";
        case 'S': return "south";
        case 'W': return "west";
        case 'E': return "east"; 
    }
    return "";
}


// Prints the board
void Board::print() const
{
    // print the column headings
    std::cout << " ";
    for (int j = 1; j <= cols; j++) {
        std::cout << std::setw(5) << j;
    }
    std::cout << "\n";
    
    // for each row
    for (int i = 0; i <= rows; i++) {

        // don't print row 0 (it doesnt exist, the first real row is row 1)
        if (i > 0) {
            
            // Note that each grid rows is printed as 3 rows of text, plus
            // the separator.  The first and third rows are blank except for
            // vertical walls.  The middle row has the row heading, the
            // robot positions, and the goals.  Robots are always uppercase,
            // goals are always lowercase (or '?' for any).
            std::string first = "  ";
            std::string middle;
            for (int j = 0; j <= cols; j++) {

                if (j > 0) { 
                    // determine if a robot is current located in this cell
                    // and/or if this is the goal
                    Position p(i,j);
                    char c = getspot(p);
                    char g = isGoal(p);
                    if (g != '?') g = tolower(g);
                    first += "    ";
                    middle += " "; 
                    middle += c; 
                    middle += g; 
                    middle += " ";
                }

                // the vertical walls
                if (getVerticalWall(i,j+0.5)) {
                    first += "|";
                    middle += "|";
                } else {
                    first += " ";
                    middle += " ";
                }
            }

            // output the three rows
            std::cout << first << std::endl;
            std::cout << std::setw(2) << i << middle << std::endl;
            std::cout << first << std::endl;
        }

        // print the horizontal walls between rows
        std::cout << "  +";
        for (double j = 1; j <= cols; j++) {
            (getHorizontalWall(i+0.5,j)) ? std::cout << "----" : std::cout << "    ";
            std::cout << "+";
        }
        std::cout << "\n";
    }
}


// Prints the vision map for a specified robot
void Board::printVisionMap(char marker) const
{
    int rid;
    int width, height; 

    if ((rid = getRIDFromMarker(marker)) < 0)
        return;

    width = robots[rid].vision.getWidth();
    height = robots[rid].vision.getHeight(); 

    if (robots[rid].vision.empty()) {
        std::cout << "No vision map for " << marker << " has been generated yet" << std::endl;
        return;
    }

    std::cout << "Reachable by robot " << marker << ":" << std::endl;
    for (int r = 1; r <= height; r++) {
        for (int c = 1; c <= width; c++) {
            int length = robots[rid].vision.get(Position(r, c)).length; 
            std::cout << "  "; 

            if (length >= 0)
                std::cout << length;
            else 
                std::cout << "."; 
        }
        std::cout << std::endl;
    }
}


// Prints the solution(s) for the board
void Board::printSolution(int max_moves, bool all_solutions)
{
    // Only one goal?
    if (numGoals() == 1) {
        char robot = goals[0].which;
        int rid = getRIDFromMarker(robot);
        int shortest_path = -1; 
        Position destination = goals[0].pos;
        std::vector<std::vector<RobotMove> > result; 

        // If this goal can be occupied by any robot, then generate vision maps for all robots
        if (robot == '?') {
            for (int c = 0; c < robots.size(); c++) {
                generateVisionMap(c, max_moves, destination);
                int path_length = robots[c].vision.get(destination).steps.size();

                // If we found a new shortest path, then clear the list of solutions add this new solution
                if (shortest_path == -1 || (path_length < shortest_path && path_length != -1)) {
                    shortest_path = path_length;
                    result.clear();
                    result.insert(result.end(), solutions.begin(), solutions.end()); 
                }

                // If this is also a shortest path but with a different robot, then add the solutions as well
                else if (path_length == shortest_path) {
                    result.insert(result.end(), solutions.begin(), solutions.end()); 
                }
            }

            solutions = result; 
        }

        // Otherwise, just generate the vision map for the one robot that is supposed to occupy that goal
        else {
            generateVisionMap(rid, max_moves, destination);
        }
    }


    // Multiple goals?
    else if (numGoals() > 1) {
        /*
        int num_goals = numGoals(); 
        int any_goal_index = -1; 
        bool* checklist = new bool[robots.size()];

        // First look for any goals specified as "any", and turn them into goals for specific robots
        for (int c = 0; c < num_goals; c++) {
            if (goals[c].which != '?')
                checklist[getRIDFrommarker(goals[c].which)] = true;
            else
                any_goal_index = c; 
        }

        if (any_goal_index != -1) {
            for (int c = 0; c < robots.size(); c++) {
                if (!checklist[c]) {
                    num_goals[any_goal_index].which = robots[c].which; 
                    break;
                }
            }
        }

        delete[] checklist;

        // Now try to find a solution
        for (int c = 0; c < num_goals; c++) {
            int rid = getRIDFromMarker(goals[c].which);
            generateVisionMap(rid, max_moves, goals[c].pos);
            std::vector<RobotMove>& steps = robots[rid].vision.get(goals[c].pos).steps; 

            if (steps.size() == 0) {
                solutions.clear(); 
                break; 
            }

            for (int d = 0; d < steps.size(); d++) {
            }
        }
        */
    }


    // Get rid of all solutions that are NOT the shortest path
    pruneSolutions();


    // Start by printing out the starting map
    print(); 


    // Display solutions
    if (solutions.size() == 0) {
        if (max_moves > 0) 
            std::cout << "no solutions with " << max_moves << " or fewer moves" << std::endl;
        else 
            std::cout << "no solutions";
        return;
    }


    // Printing all solutions?
    if (all_solutions) {
        std::cout << solutions.size() << " different " << solutions[0].size() << " move solutions:" << std::endl << std::endl;
        for (int c = 0; c < solutions.size(); c++) {
            for (int d = 0; d < solutions[c].size(); d++) {
                std::cout << "robot " << robots[solutions[c][d].id].which << " moves " <<
                    directionToString(solutions[c][d].direction) << std::endl;
            }
            std::cout << "All goals are satisfied after " << solutions[c].size() << " moves" << std::endl << std::endl;
        }
    }


    // Printing just one solution?
    else {
        for (int c = 0; c < solutions[0].size(); c++) {
            std::cout << "robot " << robots[solutions[0][c].id].which << " moves " << 
                directionToString(solutions[0][c].direction) << std::endl;
            moveRobot(solutions[0][c].id, solutions[0][c].direction);
            print();  
        }
        std::cout << "All goals are satisfied after " << solutions[0].size() << " moves" << std::endl;
    }
}


// Removes all solutions that are not the shortest path length
void Board::pruneSolutions()
{
    if (solutions.size() <= 1)
        return;

    int min_length = -1; 
    bool multiple_lengths = false; 

    for (int c = 0; c < solutions.size(); c++) {
        if (min_length == -1 || solutions[c].size() < min_length)
            min_length = solutions[c].size();

        if (min_length >= 0 && solutions[c].size() != min_length)
            multiple_lengths = true;
    }

    // If the length never changed more than once, then there are only solutions of length min_length, so don't remove anything
    if (!multiple_lengths)
        return; 

    for (int c = 0; c < solutions.size(); ) {
        if (solutions[c].size() != min_length)
            solutions.erase(solutions.begin()+c); 
        else c++; 
    }
}

// Generates a new vision map for the specified robot
void Board::generateVisionMap(int rid, int max_moves, const Position& dest)
{
    solutions.clear();
    robots[rid].vision.clear(); 
    robots[rid].vision = VisionMap(cols, rows); 
    robots[rid].vision.set(robots[rid].pos, VisionEntry(0));

    // If max_moves was not set, then automatically set an upper bound to prevent issues
    if (max_moves <= 0 && robots.size() >= 1)
        max_moves = 50;

    std::vector<std::vector<char> > current_board = board; 
    visualize(rid, max_moves, 1, dest);
    board = current_board; 
}


// Recursive helper function to help calculate the vision map for generateVisionMap() function
void Board::visualize(int rid, int max_moves, int move_number, const Position& dest, VisionEntry path)
{
    // If we have reached the maximum number of moves, then stop processing
    if (max_moves >= 0 && move_number > max_moves)
        return;

    char directions[] = "NSWE"; 
    const int directions_size = 4; 
    const int cycle_min_size = 3;    // Minimum number of elements in a path that must be repeated before considered a cycle

    for (int r = 0; r < robots.size(); r++) {
        for (int d = 0; d < directions_size; d++) {

            // Don't move in the same or opposite direction that this robot previously moved in
            if (path.steps.size() > 0 && path.steps.back().id == r) {
                char last_dir = path.steps.back().direction;
                if ((directions[d] == 'N' && last_dir == 'S') ||
                    (directions[d] == 'S' && last_dir == 'N') ||
                    (directions[d] == 'W' && last_dir == 'E') ||
                    (directions[d] == 'E' && last_dir == 'W') ||
                    (directions[d] == last_dir))
                    continue;
            }

            Position original_position = robots[r].pos; 
            path.steps.push_back(RobotMove(r, directions[d])); 

            // Try to move the targeted robot in one of the four directions
            if (moveRobot(r, directions[d])) {

                // If the specified robot moved to this location, then mark it on its vision graph
                if (r == rid) {
                    int length = -1;     // Current marker at the given location
                    if (!robots[rid].vision.empty())
                        length = robots[rid].vision.get(robots[rid].pos).length; 

                    // But only mark it if the location is either undiscovered, or we found a shorter path
                    if (length == -1 || length > move_number) {
                        path.length = move_number;
                        robots[rid].vision.set(robots[rid].pos, VisionEntry(path));
                    }

                    // If this is the destination position, then add the solution
                    if (dest.row > 0 && dest.col > 0 && robots[rid].pos == dest)
                        solutions.push_back(path.steps);
                }
                visualize(rid, max_moves, move_number+1, dest, path); 
            }

            robots[r].pos = original_position; 
            path.steps.pop_back();
        }
    }
} 