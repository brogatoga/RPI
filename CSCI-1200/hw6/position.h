#ifndef POSITION_H
#define POSITION_H


// Position on the 2D board (r, c)
class Position
{
public:
    Position(int r = 0,int c = 0) : row(r), col(c) {}
    Position(const Position& p) : row(p.row), col(p.col) {}

    const Position& operator=(const Position& p) { this->row = p.row; this->col = p.col; }
    bool operator==(const Position &p) const { return this->row == p.row && this->col == p.col; }
    bool operator!=(const Position &p) const { return this->row != p.row || this->col != p.col; }

    int row; 
    int col; 
};


#endif