#include <iostream>
#define MAX_SIZE 100

int main()
{
	float numbers[MAX_SIZE]; 
	float average = 0.0f; 
	int size = 0; 

	std::cin >> size; 
	if (size > MAX_SIZE) 
		size = MAX_SIZE; 

	for (int c = 0; c < size; c++)
		std::cin >> numbers[c]; 

	for (int c = 0; c < size; c++)
		average += numbers[c]; 
	average /= (float)size; 

	std::cout << average; 

	for (int c = 0; c < size; c++) {
		if (numbers[c] > average)
			std::cout << " " << numbers[c]; 
	}

	return 0; 
}