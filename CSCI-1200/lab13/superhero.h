#ifndef _SUPERHERO_H
#define _SUPERHERO_H

#include <iostream>
#include <string>


class Superhero
{
public:
	friend class Team;
	
	Superhero(const std::string& _name, const std::string& _identity, const std::string& _power)
		: name(_name), identity(_identity), power(_power), good(true) {}

	const std::string& getName() const { return name; }
	const std::string& getPower() const { return power; }
	

	bool isGood() const { return good; }

	bool operator==(const std::string& s) const { return identity == s; }
	bool operator!=(const std::string& s) const { return identity != s; }
	Superhero& operator-() {
		good = !good; 
		return *this; 
	}

	bool operator>(const Superhero& s) const { return comparePower(s.power) > 0; }
	bool operator<(const Superhero& s) const { return comparePower(s.power) < 0; }
	

private:
	std::string name;
	std::string identity;
	std::string power;
	bool good; 

	const std::string& getTrueIdentity() const { return identity; }

	int comparePower(const std::string& p) const {
		if (power == p)
			return 0; 

		if (power == "Fire") {
			if (p == "Wood") return 1; 
			else if (p == "Water") return -1; 
		}
		else if (power == "Wood") {
			if (p == "Water") return 1; 
			else if (p == "Fire") return -1; 
		}
		else if (power == "Water") {
			if (p == "Fire") return 1; 
			else if (p == "Wood") return -1; 
		}
		else if (power == "Invisible") {
			if (p == "Speed") return 1; 
			else if (p == "Laser") return -1; 
		}
		return 0; 
	}
};

#endif