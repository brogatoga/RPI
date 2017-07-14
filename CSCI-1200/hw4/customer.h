#ifndef CUSTOMER_H
#define CUSTOMER_H

#include <iostream>
#include <fstream>
#include <string>
#include <list>

std::string grammar(const std::string& noun, int count); 


class Customer
{
private:
	std::string name; 
	std::list<std::string> movies;
	std::list<std::string> preferences; 

public:
	Customer(const std::string& n) : name(n) {}
	Customer(const Customer& c) { name = c.name; movies = c.movies; preferences = c.preferences; }

	std::string return_oldest() {
		if (movies.size() == 0) 
			return ""; 

		std::string m = *(movies.begin()); 
		movies.pop_front(); 
		return m; 
	}

	std::string return_newest() {
		if (movies.size() == 0)
			return "";
		
		std::list<std::string>::iterator it = movies.end(); 
		it--; 
		std::string m = *it;
		movies.pop_back(); 
		return m; 
	}

	void receives(const std::string& m) {

		// Add the DVD to the customer's movie list
		movies.push_back(m);

		// Now that the customer has this movie, remove it from his preferences list
		std::list<std::string>::iterator it; 
		for (it = preferences.begin(); it != preferences.end(); it++) {
			if (m == *it) 
				break;
		}
		preferences.erase(it); 
	}

	bool has_movie(const std::string& m) {
		for (std::list<std::string>::iterator it = movies.begin(); it != movies.end(); it++) {
			if (*it == m)
				return true; 
		}
		return false; 
	}

	bool add_preference(const std::string &p) {

		// Does the preference already exist?
		for (std::list<std::string>::iterator it = preferences.begin(); it != preferences.end(); it++) {
			if (p == *it)
				return false; 
		}
		preferences.push_back(p);
		return true;
	}	

	bool has_preference(const std::string &p) { 
		for (std::list<std::string>::iterator i = preferences.begin(); i != preferences.end(); i++) {
			if (p == *i)
				return true; 
		}
		return false; 
	}

	void print(std::ostream& str) {
		str << name << " has " << grammar("movie", movies.size()); 
		if (movies.size() > 0) str << ":";
		str << std::endl;

		for (std::list<std::string>::iterator it = movies.begin(); it != movies.end(); it++)
			str << "    " << *it << std::endl;

		if (preferences.size() > 0) {
			str << "  preference list:" << std::endl;
			for (std::list<std::string>::iterator it = preferences.begin(); it != preferences.end(); it++)
				str << "    " << *it << std::endl;
		}
	}

	bool has_max_num_movies() const { return movies.size() == 3; }
	bool preference_list_empty() const { return preferences.size() == 0; }

	const std::string& get_name() const { return name; }
	const std::list<std::string>& get_preferences() const { return preferences; }
	const unsigned int get_num_movies() const { return movies.size(); }
}; 


#endif