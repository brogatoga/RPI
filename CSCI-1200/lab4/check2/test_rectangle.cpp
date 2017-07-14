#include <iostream>
#include <vector>
#include <string>
#include "Point2D.h"
#include "Rectangle.h"

int main()
{
	Rectangle a(Point2D(0, 0), Point2D(5, 5));
	Rectangle b(Point2D(3, 3), Point2D(6, 6));
	Rectangle c(Point2D(1, 5), Point2D(8, 5));

	std::cout << "Rectangle A:" << std::endl;
	print_rectangle(a);			// print_rectangle already tests lower_left_corner(), lower_right_corner(), points_contained()
								// so there is no need to test them individually. 

	std::cout << std::endl << "Rectangle B:" << std::endl;
	print_rectangle(b);

	std::cout << std::endl << "Rectangle C." << std::endl;
	print_rectangle(c);

	for (float x = 0; x <= 9; x++) {
		for (float y = 0; y <= 9; y++) {
			Point2D p(x, y);
			bool af = a.add_point(p);      // add_point returns false if is_point_within() returns true, otherwise true
			bool bf = b.add_point(p);
			bool cf = c.add_point(p);

			std::cout << "(" << p.x() << ", " << p.y() <<  ") ";
			if (af) std::cout << "ADDED to rectangle A" << std::endl;
			else std::cout << "NOT within rectangle A" << std::endl;
			if (bf) std::cout << "ADDED to rectangle B" << std::endl;
			else std::cout << "NOT within rectangle B" << std::endl;
			if (cf) std::cout << "ADDED to rectangle C" << std::endl;
			else std::cout << "NOT within rectangle C" << std::endl;
			std::cout << std::endl;
		}
	}

	std::cout << "----------- SUMMARY ---------------" << std::endl;
	print_rectangle(a);
	print_rectangle(b);
	print_rectangle(c);

	return 0; 
}	