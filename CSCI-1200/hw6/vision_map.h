#ifndef VISION_MAP_H
#define VISION_MAP_H

#include <vector>
#include <string>
#include "position.h"


// Robot ID and the direction it takes
class RobotMove
{
public:
	RobotMove(int i, char d) : id(i), direction(d) {}
	RobotMove(const RobotMove& rm) : id(rm.id), direction(rm.direction) {}

	int id;
	char direction;
}; 


// Entry for one location in a vision map that holds the marker and series of steps to take to get there
class VisionEntry
{
public:
	VisionEntry() : length(-1) {}
	VisionEntry(int l) : length(l) {}
	VisionEntry(int l, const std::vector<RobotMove>& s) : length(l), steps(s) {}
	VisionEntry(const VisionEntry& ve) : length(ve.length), steps(ve.steps) {}

	int length;
	std::vector<RobotMove> steps;
}; 


// Visual map that describes the possible locations a robot can move to, and how to get there
class VisionMap
{
public:
	VisionMap() : width(0), height(0) {}
	VisionMap(int w, int h) : width(w), height(h) { 
		map = std::vector<std::vector<VisionEntry> >(h, std::vector<VisionEntry>(w, VisionEntry()));
	}
	VisionMap(const VisionMap& vm) : width(vm.width), height(vm.height), map(vm.map) {}

	void set(const Position& p, const VisionEntry& ve) { map[p.row-1][p.col-1] = ve; }
	const VisionEntry& get(const Position& p) const { return map[p.row-1][p.col-1]; }; 
	void clear() { map.clear(); width = height = 0; }

	bool empty() const { return width <= 0 || height <= 0; }
	int getWidth() const { return width; }
	int getHeight() const { return height; }

private:
	std::vector<std::vector<VisionEntry> > map; 
	int width; 
	int height; 
}; 


#endif