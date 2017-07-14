#include <iostream>
#include "time.h"

int main()
{
	Time t1(5, 30, 59);
	// Time t2();   
	Time t3; 

	std::cout << "Time #1: " << t1.getHour() << ":" << t1.getMinute() << ":" << t1.getSecond() << std::endl;
	std::cout << "Time #1: " << t3.getHour() << ":" << t3.getMinute() << ":" << t3.getSecond() << std::endl;

	return 0; 
}