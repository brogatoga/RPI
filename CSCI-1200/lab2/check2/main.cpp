#include <iostream>
#include "time.h"

int main()
{
	Time t1(12, 24, 39);
	Time t2(0, 0, 1); 
	Time t3; 

	t1.PrintAmPm(); 
	t2.PrintAmPm();
	t3.PrintAmPm(); 

	return 0; 
}