#include <iostream>


int compute_squares(uintptr_t *a, uintptr_t *b, int n)
{
	for (int c = 0; c < n; c++)
		*(b+c) = *(a+c) * *(a+c); 
}


void print_array(uintptr_t *a, int n)
{
	for (int c = 0; c < n; c++) {
		uintptr_t t = *(a + c); 
		std::cout << t << ' ';
	} 
	std::cout << std::endl;
}

int main()
{
	const int SIZE = 5; 
	uintptr_t a[SIZE] = { 1, 2, 3, 4, 5 }; 
	uintptr_t b[SIZE]; 
	uintptr_t c[SIZE] = { 3, 8, 12, 15, 20 }; 
	uintptr_t d[SIZE]; 
	uintptr_t e[SIZE] = { 10, 20, 30, 40, 50 }; 
	uintptr_t f[SIZE]; 

	compute_squares(a, b, SIZE); 
	compute_squares(c, d, SIZE); 
	compute_squares(e, f, SIZE); 

	print_array(a, SIZE);
	print_array(b, SIZE);
	std::cout << std::endl;

	print_array(c, SIZE);
	print_array(d, SIZE);
	std::cout << std::endl;

	print_array(e, SIZE);
	print_array(f, SIZE);

	return 0; 
}