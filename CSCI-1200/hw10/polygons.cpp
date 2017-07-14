#include "polygons.h"

// Returns a vector containing the lengths of all three sides of the triangle
std::vector<double> Triangle::GetSides() const
{
	std::vector<double> result; 
	result.push_back(DistanceBetween(points[0], points[1])); 
	result.push_back(DistanceBetween(points[1], points[2]));
	result.push_back(DistanceBetween(points[0], points[2]));
	return result; 
}

// Returns a vector containing all three angles of the triangle
std::vector<double> Triangle::GetAngles() const
{
	std::vector<double> result; 
	result.push_back(Angle(points[0], points[1], points[2])); 
	result.push_back(Angle(points[2], points[0], points[1])); 
	result.push_back(Angle(points[1], points[2], points[0])); 
	return result; 
}

// Returns a vector containing the lengths of all four sides of the quadrilateral
std::vector<double> Quadrilateral::GetSides() const
{
	std::vector<double> result; 
	result.push_back(Vector(points[0], points[1]).Length()); 
	result.push_back(Vector(points[1], points[2]).Length()); 
	result.push_back(Vector(points[2], points[3]).Length()); 
	result.push_back(Vector(points[0], points[3]).Length()); 
	return result; 
}

// Returns a vector containing all four angles of the quadrilateral
std::vector<double> Quadrilateral::GetAngles() const
{
	std::vector<double> result; 
	result.push_back(Angle(points[0], points[1], points[2])); 
	result.push_back(Angle(points[1], points[2], points[3])); 
	result.push_back(Angle(points[2], points[3], points[0])); 
	result.push_back(Angle(points[3], points[0], points[1])); 
	return result; 
}

// Returns true if all sides are equal, false otherwise
bool Polygon::HasAllEqualSides() const 
{
	std::vector<double> sides = GetSides(); 
	for (int c = 1; c < sides.size(); c++) {
		if (!EqualSides(sides[0], sides[c]))
			return false; 
	}
	return true; 
}

// Returns true if all angles are equal, false otherwise
bool Polygon::HasAllEqualAngles() const {
	std::vector<double> angles = GetAngles(); 
	for (int c = 1; c < angles.size(); c++) {
		if (!EqualAngles(angles[0], angles[c]))
			return false; 
	}
	return true; 
}

// Returns true if the polygon has at least one right angle, false otherwise
bool Polygon::HasARightAngle() const {
	std::vector<double> angles = GetAngles(); 
	for (int c = 0; c < angles.size(); c++) {
		if (RightAngle(angles[c]))
			return true; 
	}
	return false; 
}

// Returns true if the polygon has at least one obtuse angle, false otherwise
bool Polygon::HasAnObtuseAngle() const {
	std::vector<double> angles = GetAngles(); 
	for (int c = 0; c < angles.size(); c++) {
		if (ObtuseAngle(angles[c]))
			return true; 
	}
	return false; 
}

// Returns true if the polygon has at least one acute angle, false otherwise
bool Polygon::HasAnAcuteAngle() const {
	std::vector<double> angles = GetAngles(); 
	for (int c = 0; c < angles.size(); c++) {
		if (AcuteAngle(angles[c]))
			return true; 
	}
	return false; 
}

// Returns true if the polygon is convex, false otherwise
bool Polygon::IsConvex() const {
	return false; 
} 

// Returns true if the polygon is concave, false otherwise
bool Polygon::IsConcave() const {
	return false; 
}

// Returns the point furthest in the given compass direction for a quadrilateral
Point Quadrilateral::GetPoint(int which) const
{
	if (which < 1 || which > 8)
		return Point(-1, -1); 

	Point result(-1, -1); 
	for (int c = 0; c < points.size(); c++) {
		if (result.x < 0 && result.y < 0) {
			result = points[c]; 
			continue; 
		}

		if ((which == POINT_N && points[c].y > result.y) ||
			(which == POINT_S && points[c].y < result.y) ||
			(which == POINT_W && points[c].x < result.x) ||
			(which == POINT_E && points[c].x > result.x) ||
			(which == POINT_NW && points[c].y >= result.y && points[c].x <= result.x) ||
			(which == POINT_NE && points[c].y >= result.y && points[c].x >= result.x) ||
			(which == POINT_SW && points[c].y <= result.y && points[c].x <= result.x) ||
			(which == POINT_SE && points[c].y <= result.y && points[c].x >= result.x))
			result = points[c]; 
	}
	return result; 
}