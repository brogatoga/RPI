#include <vector>
#include <set>
#include <iostream>


class Sudoku {
public:

  // Construct an empty board
  Sudoku(int s);

  // To construct the puzzle, set the value of a particular cell
  void Set(int i, int j, int value);

  // Is the puzzle solved, or impossible?
  bool KnownValue(int i, int j) const;
  bool IsSolved() const;
  bool Impossible() const;

  // Propagate information from this cell along columns & rows &
  // within quadrant. Return the number of cells that changed.
  int Propagate(int i, int j);

  bool OnlyChoiceRows(int &changed);
  bool OnlyChoiceColumns(int &changed);
  bool OnlyChoiceQuad(int &changed);

  // Output
  void Print() const;

private:
  std::vector<std::vector<std::set<int> > > board;
  int size;             // overall board dimensions
  int quadrant_size;    // dimensions of the quadrants
};
