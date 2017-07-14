#include <iostream>
#include <list>

void reverse_list(std::list<int>& n)
{
	std::list<int>::iterator it; 
	std::list<int>::reverse_iterator rit; 
	int size = n.size(); 
	int counter; 

	for (it = n.begin(), rit = n.rbegin(), counter = 0; counter < size / 2; it++, rit++, counter++) {
		int temp = *rit; 
		*rit = *it; 
		*it = temp; 
	}
}

void print_list(const std::list<int>& n)
{
	std::cout << "Size of list = " << n.size() << std::endl;
	for (std::list<int>::const_iterator it = n.begin(); it != n.end(); it++)
		std::cout << *it << " "; 
	std::cout << std::endl << std::endl;
}

int main()
{
	std::list<int> numbers;

	for (int c = 1; c <= 9; c++)
		numbers.push_back(c*c); 

	std::cout << "Before reversing:" << std::endl;
	print_list(numbers);
	std::cout << "After reversing: " << std::endl; 
	reverse_list(numbers);
	print_list(numbers);

	return 0; 
}