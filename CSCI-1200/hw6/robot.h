#ifndef ROBOT_H
#define ROBOT_H

#include "position.h"
#include "vision_map.h"


// Stores position and alphabetical label of a robot
class Robot
{
public: 
    Robot(Position p, char w) : pos(p), which(w) { assert (isalpha(w) && isupper(w)); }
    Robot(const Robot& r) : vision(r.vision), pos(r.pos), which(r.which) {}

    VisionMap vision;
    Position pos;
    char which;
};


#endif