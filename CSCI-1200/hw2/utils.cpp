#include "include.h"


static std::string day_strings[] = {
	"Monday", "Tuesday", "Wednesday", "Thursday",
	"Friday", "Saturday", "Sunday"
};

static std::string month_strings[] = {
	"January", "February", "March", "April",
	"May", "June", "July", "August",
	"September", "October", "November", "December"
};


// Returns day of the week in integer form (eg: "Friday" = 5)
int dowToInt(const std::string &s)
{
	for (int c = 0; c < 7; c++) {
		if (day_strings[c] == s)
			return c+1; 
	}
	return -1;
}


// Returns month in integer form (eg: "March" = 3)
int monthToInt(const std::string &s)
{
	for (int c = 0; c < 12; c++) {
		if (month_strings[c] == s)
			return c+1; 
	}
	return -1; 
}


// Returns day of the week as a word (eg: 4 = "Thursday")
std::string dowToString(int n)
{
	return (n >= 1 && n <= 7) ? day_strings[n-1] : ""; 
}


// Returns month as a word (eg: 8 = "August")
std::string monthToString(int n)
{
	return (n >= 1 && n <= 12) ? month_strings[n-1] : ""; 
}


// Returns whether a string is a valid time or not (in the format MM:SS)
bool validTime(const std::string &s)
{
	size_t colon = s.find_first_of(":"); 
	int minutes = -1; 
	int seconds = -1; 

	if (colon == std::string::npos)
		return false; 

	minutes = atoi(s.substr(0, colon).c_str()); 
	seconds = atoi(s.substr(colon+1).c_str()); 

	return (minutes >= 0 && minutes <= 20) && (seconds >= 0 && seconds <= 59)
		&& (minutes < 20 || (minutes == 20 && seconds == 0));  
}


// Parses a time-string with format MM:SS and saves the minute and second portions
void getTime(const std::string &s, int &minutes, int &seconds)
{
	if (!validTime(s))
		return;

	size_t colon = s.find_first_of(":");
	minutes = atoi(s.substr(0, colon).c_str());
	seconds = atoi(s.substr(colon+1).c_str());
}


// Return a new string with the first character omitted
std::string removeFirst(const std::string &s)
{
	return (s.length() >= 2) ? 
		s.substr(1, s.length() - 1) : ""; 
}


// Return a new string with the last character omitted
std::string removeLast(const std::string &s)
{
	return (s.length() >= 2) ? 
		s.substr(0, s.length() - 1) : ""; 
}


// Returns an int in string format
std::string toString(int n)
{
	std::ostringstream stream;
	stream << n; 
	return stream.str(); 
}


// Returns a float in string format
std::string toString(float f)
{
	std::ostringstream stream;
	stream << f; 
	return stream.str(); 
}


// Converts an std::string to integer form, better than writing .c_str() all the time
int stringToInt(const std::string &s) 
{
	return atoi(s.c_str()); 
}


// Same as above except for floats
float stringToFloat(const std::string &s) 
{
	return atof(s.c_str()); 
}


// Formats and writes the provided table to the given output stream
void writeTable(const table &t, std::ostream &out)
{
	// Compute the width for each column
	int rows = t.size();
	int columns = t[0].size();
	int length, maxLength;
	std::vector<int> widths(columns);

	if (rows == 0 || columns == 0)
		return;

	// Loop through every row of each column and find the string with largest length
	for (int x = 0; x < columns; x++) {
		maxLength = 0; 
		for (int y = 0; y < rows; y++) {
			if ((length = t[y][x].length()) > maxLength)
				maxLength = length;
		}
		widths[x] = maxLength;
	}

	// Write the formatted table to the output stream
	for (int y = 0; y < rows; y++) {
		for (int x = 0; x < columns; x++) { 
			std::string s = t[y][x];

			// Numbers are right aligned, everything else is left aligned 
			if (s == "0" || s == "0.00" || stringToInt(s) > 0 || stringToFloat(s) > 0.0f)
				out << std::right; 
			else 
				out << std::left; 

			out << std::setw(widths[x]) << s << "  "; 
		}
		out << std::endl;
	}
} 


// Adds a new row of text to the table given an array of columns in the row
void addRowToTable(table &t, std::string *data, int columns)
{
	if (data == NULL || columns <= 0)
		return;

	std::vector<std::string> row;
	for (int c = 0; c < columns; c++)
		row.push_back(data[c]);
	t.push_back(row);
}