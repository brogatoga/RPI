#ifndef TRIANGLE_H
#define TRIANGLE_H 

// ------------------------------------------------------------------------------------

class Triangle : public Polygon
{
public:
	Triangle(const std::string& _name, const std::vector<Point>& _points) : Polygon(_name, _points) {
		if (points.size() != 3) throw -1; 
	}
	virtual std::vector<double> GetSides() const; 
	virtual std::vector<double> GetAngles() const; 
	virtual bool IsConvex() const { return true; }
	virtual bool IsConcave() const { return false; }
}; 

// ------------------------------------------------------------------------------------

class RightTriangle : virtual public Triangle
{
public:
	RightTriangle(const std::string& _name, const std::vector<Point>& _points) : Triangle(_name, _points)  {
		if (!Polygon::HasARightAngle()) throw -1; 
	}
	virtual bool HasARightAngle() const { return true; }
}; 

// ------------------------------------------------------------------------------------

class IsoscelesTriangle : virtual public Triangle
{
public:
	IsoscelesTriangle(const std::string& _name, const std::vector<Point>& _points) : Triangle(_name, _points) {
		std::vector<double> sides = GetSides(), angles = GetAngles();
		if (!(EqualSides(sides[0], sides[1]) || EqualSides(sides[1], sides[2]) || EqualSides(sides[0], sides[2])) ||
			!(EqualAngles(angles[0], angles[1]) || EqualAngles(angles[1], angles[2]) || EqualAngles(angles[0], angles[2])))
			throw -1; 
	}
}; 

// ------------------------------------------------------------------------------------

class ObtuseTriangle : virtual public Triangle
{
public:
	ObtuseTriangle(const std::string& _name, const std::vector<Point>& _points) : Triangle(_name, _points) {
		if (!Polygon::HasAnObtuseAngle())
			throw -1; 
	}
	virtual bool HasAnObtuseAngle() const { return true; }
}; 

// ------------------------------------------------------------------------------------

class EquilateralTriangle : public IsoscelesTriangle
{
public:
	EquilateralTriangle(const std::string& _name, const std::vector<Point>& _points)
	 : IsoscelesTriangle(_name, _points), Triangle(_name, _points) {
	 	if (!Polygon::HasAllEqualAngles() || !Polygon::HasAllEqualSides())
	 		throw -1; 
	}
	virtual bool HasAllEqualSides() const { return true; }
	virtual bool HasAllEqualAngles() const { return true; }
}; 

// ------------------------------------------------------------------------------------

class IsoscelesObtuseTriangle : public IsoscelesTriangle, public ObtuseTriangle
{
public:
	IsoscelesObtuseTriangle(const std::string& _name, const std::vector<Point>& _points) 
		: IsoscelesTriangle(_name, _points), ObtuseTriangle(_name, _points), Triangle(_name, _points) {
		// The two base constructors will handle the exceptions, as both requirements need to be met
	}
	virtual bool HasAnAcuteAngle() const { return true; }
}; 

// ------------------------------------------------------------------------------------

class IsoscelesRightTriangle : public IsoscelesTriangle, public RightTriangle
{
public:
	IsoscelesRightTriangle(const std::string& _name, const std::vector<Point>& _points) 
		: IsoscelesTriangle(_name, _points), RightTriangle(_name, _points), Triangle(_name, _points) {
		// Again, the two base constructors will handle the checking of both requirements
	}
}; 

// ------------------------------------------------------------------------------------

#endif