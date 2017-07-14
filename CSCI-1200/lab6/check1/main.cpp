#include <iostream>
#include <vector>

void reverse_vector(std::vector<int>& v)
{
	if (v.size() <= 1)
		return;

	int counter = 0; 
	int size = v.size(); 

	for (int c = size-1; c >= size / 2; c--) {
		int temp = v[counter]; 
		v[counter] = v[c]; 
		v[c] = temp; 
		counter++; 
	}
}

void print_vector(const std::vector<int>& v)
{
	int size = v.size(); 
	std::cout << "Size of vector = " << size << std::endl;
	for (int c = 0; c < size; c++)
		std::cout << v[c] << " "; 
	std::cout << std::endl << std::endl;
}

int main()
{
	std::vector<int> normal;
	std::vector<int> empty; 
	std::vector<int> one_value(1, 1); 
	std::vector<int> two_values; 
	std::vector<int> even_size; 
	std::vector<int> odd_size; 

	for (int c = 0; c < 500; c += 20) normal.push_back(c*2); 
	for (int c = 1; c <= 100; c++) even_size.push_back(c); 
	for (int c = 1; c <= 99; c++) odd_size.push_back(c*3); 
	two_values.push_back(10); 
	two_values.push_back(20); 

	std::cout << "Before reverse:" << std::endl;
	print_vector(normal); 
	print_vector(empty); 
	print_vector(one_value);
	print_vector(two_values);
	print_vector(even_size);
	print_vector(odd_size); 

	reverse_vector(normal); 
	reverse_vector(empty); 
	reverse_vector(one_value);
	reverse_vector(two_values);
	reverse_vector(even_size);
	reverse_vector(odd_size);

	std::cout << "After reverse: " << std::endl;
	print_vector(normal); 
	print_vector(empty); 
	print_vector(one_value);
	print_vector(two_values);
	print_vector(even_size);
	print_vector(odd_size); 

	return 0; 
}