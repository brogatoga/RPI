#include <iostream>
#include <vector>
#include <string>
using namespace std;

/**
 * let n = # of assigned phone numbers
 * let N = largest possible phone number
**/

// add a number, name pair to the phonebook
// O(1) -- constant time operation
void add(vector<string> &phonebook, int number, const string& name) {
  phonebook[number] = name;
}

// given a phone number, determine who is calling
// O(1) -- constant time operation
void identify(const vector<string> & phonebook, int number) {
  if (phonebook[number] == "UNASSIGNED") 
    cout << "unknown caller!" << endl;
  else 
    cout << phonebook[number] << " is calling!" << endl;
}


int main() {
  // create the phonebook -- O(N)
  // memory usage = O(Ns), where s = average length of each caller's name (including "UNASSIGNED")
  // 7 digit number = (1000000/10000) = 100 times as much memory
  // 10 digit number = (1000000000/10000) = 100,000 times as much memory
  vector<string> phonebook(10000, "UNASSIGNED");

  // add several names to the phonebook
  add(phonebook, 1111, "fred");
  add(phonebook, 2222, "sally");
  add(phonebook, 3333, "george");

  // test the phonebook
  identify(phonebook, 2222);
  identify(phonebook, 4444);
  identify(phonebook, 1111); 
  identify(phonebook, 3333); 
  identify(phonebook, 0); 
  identify(phonebook, 9999); 
}
