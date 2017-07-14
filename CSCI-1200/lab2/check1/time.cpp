#include "time.h"

Time::Time()
{
	hour = 0; 
	minute = 0; 
	second = 0; 
}

Time::Time(int _hour, int _minute, int _second)
{
	hour = _hour;
	minute = _minute;
	second = _second;
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