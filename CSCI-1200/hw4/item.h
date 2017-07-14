#ifndef ITEM_H
#define ITEM_H

#include <iostream>
#include <fstream>
#include <string>
#include <list>

std::string grammar(const std::string& noun, int count); 


class Item
{
private:
	std::string name; 
	unsigned int quantity; 
	unsigned int in_stock; 

public:
	Item(const std::string& n, unsigned int q = 1) : name(n), quantity(q), in_stock(q) {}
	Item(const Item& i) { name = i.name; quantity = i.quantity; in_stock = i.in_stock; }

	void add_copy(unsigned int q = 1) { quantity += q; }
	void returned() { in_stock++; }
	void shipped() { 
		if (in_stock > 0)
			in_stock--;
	}

	void print(std::ostream& str) const {
		int checkedout = quantity - in_stock; 
		str << name << ":" << std::endl;

		if (checkedout > 0) {	
			str << "  " << grammar("copy", checkedout) << " checked out"; 
			if (in_stock == 0)
				str << std::endl;
		}

		if (in_stock > 0) {
			if (checkedout > 0)
				str << " and "; 
			else 
				str << "  "; 
			
			str << grammar("copy", in_stock) << " available" << std::endl;
		}
	}

	const std::string& get_name() const { return name; }
	bool available() const { return in_stock > 0 && quantity > 0; }
}; 


#endif