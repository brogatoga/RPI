#include <iostream>
#include "time.h"

Time::Time()
{
	hour = 0; 
	minute = 0; 
	second = 0; 
}

Time::Time(uintptr_t _hour, uintptr_t _minute, uintptr_t _second)
{
	setHour(_hour);
	setMinute(_minute);
	setSecond(_second);
}

uintptr_t Time::getHour() const
{
	return hour; 
}

uintptr_t Time::getMinute() const
{
	return minute; 
}

uintptr_t Time::getSecond() const
{
	return second; 
}

void Time::setHour(uintptr_t _hour)
{
	hour = _hour;
}

void Time::setMinute(uintptr_t _minute)
{
	minute = _minute;
}

void Time::setSecond(uintptr_t _second)
{
	second = _second;
}

void Time::PrintAmPm()
{
	uintptr_t h = hour; 
	uintptr_t pm = false; 

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
	uintptr_t h1 = t1.getHour(),   h2 = t2.getHour(),
		m1 = t1.getMinute(), m2 = t2.getMinute(),
		s1 = t1.getSecond(), s2 = t2.getSecond(); 

	return ((h1 < h2) ||
		 	(h1 == h2 && m1 < m2) ||
		 	(h1 == h2 && m1 == m2 && s1 < s2));
}