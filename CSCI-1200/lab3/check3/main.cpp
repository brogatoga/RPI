#include <iostream>
#include "time.h"


// Function declarations for functions in print_stack.cpp
void print_stack();
void label_stack(uintptr_t *, const std::string &); 
void clear_stack_labels(); 


// Add 2:30:15 to t1 and t2
void change_times(Time &t1, Time t2)
{
	// some random variables to add to the stack frame
	uintptr_t d = 300; 
	uintptr_t e = 400; 
	uintptr_t f = 443; 

	t1.setHour(t1.getHour() + 2); 
	t1.setMinute(t1.getMinute() + 30); 
	t1.setSecond(t1.getSecond() + 15); 

	t2.setHour(t2.getHour() + 2); 
	t2.setMinute(t2.getMinute() + 30); 
	t2.setSecond(t2.getSecond() + 15); 


	std::cout << "inside of change_times: " << std::endl;
	std::cout << "Address of t1 = " << &t1 << std::endl;
	std::cout << "Address of t2 = " << &t2 << std::endl;
	label_stack((uintptr_t *) &t1, "t1");
	label_stack((uintptr_t *) &t2, "t2"); 
	label_stack(&d, "d"); 
	label_stack(&e, "e"); 
	label_stack(&f, "f"); 
	print_stack(); 
}


// Main function
int main(int argc, char *argv[])
{
	uintptr_t a = 100;   // random variables!

	Time t1(10, 0, 0); 
	Time t2(18, 0, 0); 

	// more random variables!
	uintptr_t b = 200; 
	uintptr_t c = 210; 

	std::cout << "Size of Time class = " << sizeof(Time) << " bytes" << std::endl << std::endl;
	std::cout << "Address of t1 = " << &t1 << std::endl;
	std::cout << "Address of t2 = " << &t2 << std::endl;
	std::cout << "before calling change_times: " << std::endl;
	label_stack((uintptr_t *) &t1, "t1");
	label_stack((uintptr_t *) &t2, "t2");
	label_stack(&a, "a"); 
	label_stack(&b, "b"); 
	label_stack(&c, "c"); 
	print_stack(); 

	change_times(t1, t2); 
	return 0; 
}