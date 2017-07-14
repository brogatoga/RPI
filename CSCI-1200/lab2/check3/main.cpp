#include <iostream>
#include <vector>
#include <algorithm>
#include "time.h"

int main()
{
	std::vector<Time> times; 

	times.push_back(Time(1, 20, 30)); 
	times.push_back(Time(23, 20, 25)); 
	times.push_back(Time(12, 15, 15));
	times.push_back(Time(21, 50, 45));
	times.push_back(Time(5, 15, 45));
	times.push_back(Time(18, 30, 30));
	times.push_back(Time(10, 0, 0));

	sort(times.begin(), times.end(), IsEarlierThan); 

	for (int c = 0; c < times.size(); c++)
		times[c].PrintAmPm(); 

	return 0; 
}