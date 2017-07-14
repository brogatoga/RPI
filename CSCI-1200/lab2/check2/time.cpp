#include <iostream>
#include "time.h"

Time::Time()
{
	hour = 0; 
	minute = 0; 
	second = 0; 
}

Time::Time(int _hour, int _minute, int _second)
{
	setHour(_hour);
	setMinute(_minute);
	setSecond(_second);
}

int Time::getHour() const
{
	return hour; 
}

int Time::getMinute() const
{
	return minute; 
}

int Time::getSecond() const
{
	return second; 
}

void Time::setHour(int _hour)
{
	hour = _hour;
}

void Time::setMinute(int _minute)
{
	minute = _minute;
}

void Time::setSecond(int _second)
{
	second = _second;
}

void Time::PrintAmPm()
{
	int h = hour; 
	int pm = false; 

	if (h >= 12)
		pm = true; 

	if (h == 0) {
		h = 12; 

		//if (minute > 0 || second > 0)
			//pm = true; 
	}

	if (h > 12) 
		h -= 12;

	std::cout << h << ':';
	if (minute < 10) std::cout << '0'; 
	std::cout << minute << ':';
	if (second < 10) std::cout << '0';
	std::cout << second << ' '; 

	if (pm) std::cout << "pm"; 
	else std::cout << "am"; 
	std::cout << std::endl;
}	