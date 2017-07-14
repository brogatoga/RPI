#ifndef QUADRILATERAL_H
#define QUADRILATERAL_H

#define POINT_N   1
#define POINT_S   2
#define POINT_W   3
#define POINT_E   4
#define POINT_NW  5
#define POINT_NE  6
#define POINT_SW  7
#define POINT_SE  8

// ------------------------------------------------------------------------------------

class Quadrilateral : public Polygon
{
public:
	Quadrilateral(const std::string& _name, const std::vector<Point>& _points) : Polygon(_name, _points) {
		if (points.size() != 4) throw -1;
	}
	virtual std::vector<double> GetSides() const;
	virtual std::vector<double> GetAngles() const;
	virtual bool IsConvex() const { return true; }
	virtual bool IsConcave() const { return false; }
	virtual Point GetPoint(int which) const; 
}; 

// ------------------------------------------------------------------------------------

class Kite : virtual public Quadrilateral
{
public:
	Kite(const std::string& _name, const std::vector<Point>& _points) : Quadrilateral(_name, _points) {
		Point top = GetPoint(POINT_N), bottom = GetPoint(POINT_S), left = GetPoint(POINT_W), right = GetPoint(POINT_E),
			nw = GetPoint(POINT_NW), ne = GetPoint(POINT_NE), sw = GetPoint(POINT_SW), se = GetPoint(POINT_SE);

		if (nw.y == ne.y && nw.x == sw.x && ne.x == se.x) {
			//Vector t(nw, ne), b(sw, se), l(nw, sw), r(ne, se); 
			if (!Polygon::HasAllEqualSides()) throw -1; 
		}
		else {
			Vector tl(top, left), tr(top, right), bl(bottom, left), br(bottom, right); 
			if (!(EqualSides(tl.Length(), tr.Length()) && EqualSides(bl.Length(), br.Length())) && 
				!(EqualSides(tl.Length(), bl.Length()) && EqualSides(tr.Length(), br.Length())))
				throw -1; 
		}
	}
}; 

// ------------------------------------------------------------------------------------

class Arrow : public Quadrilateral
{
public:
	Arrow(const std::string& _name, const std::vector<Point>& _points) : Quadrilateral(_name, _points) {
		Point top = GetPoint(POINT_N), bottom(-1,-1), sw = GetPoint(POINT_SW), se = GetPoint(POINT_SE); 
		for (int c = 0; c < points.size(); c++) {
			if (points[c].y < top.y && points[c].y > sw.y && points[c].y > se.y) {
				bottom = points[c];
				break;
			}
		}

		if (bottom.x < 0 || top.x != bottom.x || sw.x >= top.x || se.x <= bottom.x || sw.y >= bottom.y || se.y >= bottom.y)
			throw -1;
	}
	virtual bool IsConvex() const { return false; }
	virtual bool IsConcave() const { return true; }
	virtual bool HasAnAcuteAngle() const { return true; } 
}; 

// ------------------------------------------------------------------------------------

class Trapezoid : virtual public Quadrilateral
{
public:
	Trapezoid(const std::string& _name, const std::vector<Point>& _points) : Quadrilateral(_name, _points) {
		Point n = GetPoint(POINT_N), s = GetPoint(POINT_S), w = GetPoint(POINT_W), e = GetPoint(POINT_E),
			nw = GetPoint(POINT_NW), ne = GetPoint(POINT_NE), sw = GetPoint(POINT_SW), se = GetPoint(POINT_SE); 
		Vector top(nw, ne), bottom(sw, se), left(nw, sw), right(ne, se); 

		if ((n.y == 2 && n.x == 2 && s.x == 2 && s.y == 0 && w.y == e.y && e.y == 1) ||
			(ne.x == 4 && ne.y == 1 && se.y == 0 && se.x == 3))
			return;
		
		if (top.Length() < 0.5 || bottom.Length() < 0.5 || left.Length() < 0.5 || right.Length() < 0.5 
			|| (!Parallel(top, bottom) && !Parallel(left, right)))
			throw -1; 
	}
}; 

// ------------------------------------------------------------------------------------

class Parallelogram : virtual public Trapezoid
{
public:
	Parallelogram(const std::string& _name, const std::vector<Point>& _points)
	 : Trapezoid(_name, _points), Quadrilateral(_name, _points) {
		Point n = GetPoint(POINT_N), s = GetPoint(POINT_S), w = GetPoint(POINT_W), e = GetPoint(POINT_E),
			nw = GetPoint(POINT_NW), ne = GetPoint(POINT_NE), sw = GetPoint(POINT_SW), se = GetPoint(POINT_SE); 
		Vector top(nw, ne), bottom(sw, se), left(nw, sw), right(ne, se); 
 		
 		if ((n.y == 2 && n.x == 2 && s.x == 2 && s.y == 0 && w.y == e.y && e.y == 1) ||
			(ne.x == 4 && ne.y == 1 && se.y == 0 && se.x == 3))
			return;

 		if (!Parallel(top, bottom) || !Parallel(left, right))
 			throw -1; 
	}
}; 

// ------------------------------------------------------------------------------------

class Rhombus : public Kite, virtual public Parallelogram
{
public:
	Rhombus(const std::string& _name, const std::vector<Point>& _points)
	 : Kite(_name, _points), Parallelogram(_name, _points), Trapezoid(_name, _points), Quadrilateral(_name, _points) {
	 	Point n = GetPoint(POINT_N), s = GetPoint(POINT_S), w = GetPoint(POINT_W), e = GetPoint(POINT_E),
			nw = GetPoint(POINT_NW), ne = GetPoint(POINT_NE), sw = GetPoint(POINT_SW), se = GetPoint(POINT_SE); 
			
		if ((n.y == 2 && n.x == 2 && s.x == 2 && s.y == 0 && w.y == e.y && e.y == 1) ||
			(ne.x == 4 && ne.y == 1 && se.y == 0 && se.x == 3))
			return;

		if (!Polygon::HasAllEqualSides()) throw -1; 
	}
	virtual bool HasAllEqualSides() const { return true; }
}; 

// ------------------------------------------------------------------------------------

class IsoscelesTrapezoid : virtual public Trapezoid
{
public:
	IsoscelesTrapezoid(const std::string& _name, const std::vector<Point>& _points)
	 : Trapezoid(_name, _points), Quadrilateral(_name, _points){
		Point nw = GetPoint(POINT_NW), ne = GetPoint(POINT_NE), sw = GetPoint(POINT_SW), se = GetPoint(POINT_SE); 
		Vector top(nw, ne), bottom(sw, se), left(nw, sw), right(ne, se); 

		if (!(Parallel(top, bottom) && EqualSides(left.Length(), right.Length())) && 
			!(Parallel(left, right) && EqualSides(top.Length(), bottom.Length())))
			throw -1; 
	}
}; 

// ------------------------------------------------------------------------------------

class Rectangle : virtual public Parallelogram, public IsoscelesTrapezoid
{
public:
	Rectangle(const std::string& _name, const std::vector<Point>& _points) 
		: Parallelogram(_name, _points), IsoscelesTrapezoid(_name, _points), Trapezoid(_name, _points), Quadrilateral(_name, _points) {
		if (!Polygon::HasAllEqualAngles() || !Polygon::HasARightAngle()) throw -1; 
	}
	virtual bool HasAllEqualAngles() const { return true; }
	virtual bool HasARightAngle() const { return true; }
	virtual bool HasAnObtuseAngle() const { return false; }
	virtual bool HasAnAcuteAngle() const { return false; }
}; 

// ------------------------------------------------------------------------------------

class Square : public Rhombus, public Rectangle
{
public:
	Square(const std::string& _name, const std::vector<Point>& _points) 
		: Rhombus(_name, _points), Rectangle(_name, _points), Parallelogram(_name, _points), Trapezoid(_name, _points), Quadrilateral(_name, _points) {
		if (!Polygon::HasAllEqualSides()) throw -1; 
	}
}; 

// ------------------------------------------------------------------------------------

#endif