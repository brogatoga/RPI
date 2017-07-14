class Time
{
private:
	int hour; 
	int minute; 
	int second; 

public:
	Time();
	Time(int, int, int); 
	
	int getHour() const; 
	int getMinute() const;
	int getSecond() const; 

	void setHour(int);
	void setMinute(int);
	void setSecond(int);

	void PrintAmPm(); 
}; 