#ifndef POLYGON_H
#define POLYGON_H

#include <iostream>
#include <string>
#include <vector>
#include "utilities.h"

// ------------------------------------------------------------------------------------

class Polygon
{
protected:
	std::string name; 
	std::vector<Point> points; 

	virtual std::vector<double> GetSides() const { return std::vector<double>(); }
	virtual std::vector<double> GetAngles() const { return std::vector<double>(); }

public:
	Polygon(const std::string& _name, const std::vector<Point>& _points) : name(_name), points(_points) {
		if (name.length() == 0 || points.size() < 3) throw -1; 
	}
	const std::string& getName() const { return name; }

    virtual bool HasAllEqualSides() const;
	virtual bool HasAllEqualAngles() const;
	virtual bool HasARightAngle() const;
	virtual bool HasAnObtuseAngle() const;
	virtual bool HasAnAcuteAngle() const;
	virtual bool IsConvex() const;
	virtual bool IsConcave() const;
}; 

// ------------------------------------------------------------------------------------

#include "triangle.h"
#include "quadrilateral.h"
#endif