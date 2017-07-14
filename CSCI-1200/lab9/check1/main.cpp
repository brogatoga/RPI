#include <iostream>
#include <vector>
#include <map>

int main()
{
	std::map<int,int> freq; 
	std::vector<int> highest; 
	int max_freq = -1;  
	int n; 

	while (std::cin >> n)
		freq[n]++; 

	for (std::map<int,int>::const_iterator it = freq.begin(); it != freq.end(); it++) {
		if (max_freq == -1 || it->second > max_freq) {
			highest.clear();
			highest.push_back(it->first);
			max_freq = it->second; 
		}
		else if (it->second == max_freq) {
			highest.push_back(it->first); 
		}
	}

	std::cout << "The modes are: "; 
	for (int c = 0; c < highest.size(); c++) {
		std::cout << highest[c] << "  "; 
	}
	std::cout << std::endl;

	return 0; 
}