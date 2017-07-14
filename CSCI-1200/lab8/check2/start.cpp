#include <fstream>
#include <iostream>
#include <list>
#include <vector>
#include <cstdlib>
using namespace std;


class Point 
{
public:
  Point(int x0, int y0) : x(x0), y(y0) {}
  int x,y;
};


void read_grid(istream& istr, vector<vector<bool> > & blocked_grid,
	       int & start_x, int & start_y) {

  // Read the x y locations into a list of Points.  Keep track of the
  // max x and max y values so that the size of the grid can be
  // determined.
  int x, y;
  int max_x = 0, max_y = 0;  // keep track of the max coordinate values
  list<Point> blocked_points;
  while ((istr >> x >> y) && ! (x==0 && y==0)) {
    blocked_points.push_back(Point(x,y));
    if (x > max_x) max_x = x;
    if (y > max_y) max_y = y;
  }

  // Now that a 0 0 location has been read, read the start location.
  // If this is beyond the max x or y value then update these values.
  istr >> start_x >> start_y;
  if (start_x > max_x) max_x = start_x;
  if (start_y > max_y) max_y = start_y;

  // Make a vector of vectors with all entries marked false.   
  vector<bool> one_row_of_ys(max_y+1, false);
  vector<vector<bool> > empty_grid(max_x+1, one_row_of_ys);
  blocked_grid = empty_grid;

  // For Point in the list, mark the location in the list as blocked.
  list<Point>::iterator p;
  for (p = blocked_points.begin(); p != blocked_points.end(); ++p) {
    blocked_grid[p->x][p->y] = true;
  }
}


void print_grid(vector<vector<bool> > const& blocked_grid, int start_x, int start_y) {

  cout << "Here is the grid with the origin in the upper left corner, x increasing \n"
       << "horizontally and y increasing down the screen.  An 'X' represents a blocked\n"
       << "location and the 'S' represents the starting location.\n\n";

  for (int y=0; y<blocked_grid[0].size(); ++y) {
    for (int x=0; x<blocked_grid.size(); ++x) {
      if (x == start_x && y == start_y)
	cout << " S";
      else if (blocked_grid[x][y])
	cout << " X";
      else 
	cout << " .";
    }
    cout << '\n';
  }
}


int calculatePaths(int x, int y, const vector<vector<bool> >& blocked)
{
  if ((x == 0 && y == 0))
    return 1;
  else if (blocked[x][y])
    return 0;  
  else if (x == 0 && y > 0)
    return calculatePaths(x, y-1, blocked); 
  else if (y == 0 && x > 0)
    return calculatePaths(x-1, y, blocked); 
  return calculatePaths(x-1, y, blocked) + calculatePaths(x, y-1, blocked);
}


int main(int argc, char* argv[])
{
  if (argc != 2) {
    cerr << "Usage: " << argv[0] << " grid-file\n";
    return 1;
  }

  ifstream istr(argv[1]);
  if (!istr) {
    cerr << "Could not open " << argv[1] << endl;
    return 1;
  }
  
  vector<vector<bool> > blocked_grid;
  int start_x, start_y;
  read_grid(istr, blocked_grid, start_x, start_y);
  print_grid(blocked_grid, start_x, start_y);

  cout << "There are " << calculatePaths(start_x, start_y, blocked_grid) << " paths from that point to the origin" << endl;

  return 0;
}
      
