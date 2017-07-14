class Time
{
private:
	uintptr_t hour; 
	uintptr_t minute; 
	uintptr_t second; 

public:
	Time();
	Time(uintptr_t, uintptr_t, uintptr_t); 
	
	uintptr_t getHour() const; 
	uintptr_t getMinute() const;
	uintptr_t getSecond() const; 

	void setHour(uintptr_t);
	void setMinute(uintptr_t);
	void setSecond(uintptr_t);

	void PrintAmPm(); 
}; 

bool IsEarlierThan(const Time &, const Time &); 