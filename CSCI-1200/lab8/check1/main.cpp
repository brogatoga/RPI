#include <iostream>
#include <cstdlib>

// The rules only allow horizontal and vertical movement, which means that
// diagonal movement is forbidden. Also, when you move you cannot increase
// the distance from the origin, so every move made must move the current 
// position closer to the origin. 

int calculatePaths(int x, int y)
{
	if (x == 0 && y == 0)
		return 0; 
	else if ((abs(x) == 1 && y == 0) || (x == 0 && abs(y) == 1))
		return 1; 

	int dx = (x < 0) ? 1 : -1,
		dy = (y < 0) ? 1 : -1;

	int sum_horizontal = 0, 
		sum_vertical   = 0; 

	if (x != 0)
		sum_horizontal = calculatePaths(x+dx, y); 
	if (y != 0)
		sum_vertical = calculatePaths(x, y+dy); 
	
	return sum_horizontal + sum_vertical; 
}


int main(int argc, char **argv)
{
	if (argc != 3) {
		std::cerr << "Usage: " << argv[0] << " x y " << std::endl;
		return 1; 
	}

	int x = atoi(argv[1]),
		y = atoi(argv[2]); 

	std::cout << "There are " << calculatePaths(x, y) << " paths from (" << x << "," << y << ") to the origin" << std::endl;
	return 0; 
}