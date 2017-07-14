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

	if (h > 12) {
		h -= 12;
		pm = true; 
	}

	std::cout << h << ':';
	if (minute < 10) std::cout << '0'; 
	std::cout << minute << ':';
	if (second < 10) std::cout << '0';
	std::cout << second << ' '; 
	if (pm) std::cout << "pm"; 
	else std::cout << "am"; 
	std::cout << std::endl;
}	

bool IsEarlierThan(const Time &t1, const Time &t2)
{
	int h1 = t1.getHour(),   h2 = t2.getHour(),
		m1 = t1.getMinute(), m2 = t2.getMinute(),
		s1 = t1.getSecond(), s2 = t2.getSecond(); 

	return ((h1 < h2) ||
		 	(h1 == h2 && m1 < m2) ||
		 	(h1 == h2 && m1 == m2 && s1 < s2));
}