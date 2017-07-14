#ifndef GOAL_H
#define GOAL_H

#include "position.h"


// Stores position of goal and which robot must occupy the goal (? for any)
class Goal
{
public:
    Goal(Position p, char w) : pos(p), which(w) { assert (w == '?' || (isalpha(w) && isupper(w))); }
    Goal(const Goal& g) : pos(g.pos), which(g.which) {}
    
    Position pos;
    char which;
};


#endif