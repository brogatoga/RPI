#include <iostream>
#include <cassert>
#include <string>
#include <set>
#include "smart_pointers.h"

#define MAX_NUM_ROPES 10


class Balloon 
{
public:
  Balloon(const std::string& name_) : name(name_) {
    std::cout << "Balloon constructor " << name << std::endl;
    num_ropes = 0;
    ropes = new Balloon*[MAX_NUM_ROPES];
  }
  ~Balloon() {
    std::cout << "Balloon destructor " << name << std::endl;
    delete [] ropes;
  }

  // ACCESSORS
  const std::string& getName() const { return name; }
  int numRopes() const { return num_ropes; }
  Balloon* getRope(int i) const { return ropes[i]; }

  // print the balloons we are attached to
  void print() { 
    std::cout << "Balloon " << name << " is connected to: ";
    for (int i = 0; i < num_ropes; i++) {
      std::cout << ropes[i]->name << " ";
    }
    if (num_ropes == 0) std::cout << "nothing";
    std::cout << std::endl;
  }
  // add a rope connecting this balloon to another
  void addRope(Balloon* b) { 
    assert (num_ropes < MAX_NUM_ROPES);
    ropes[num_ropes] = b;
    num_ropes++;
  }
  // detach a rope connecting this balloon to another
  void removeRope(Balloon* b) { 
    for (int i = 0; i < MAX_NUM_ROPES; i++) {
      if (ropes[i] == b) { ropes[i] = ropes[num_ropes-1]; num_ropes--; break; }
    }
  }

private:
  std::string name;
  int num_ropes;
  Balloon** ropes; 
};


// ====================================================
// ====================================================

void deleteAll(Balloon* p); 
void deleteAllHelper(Balloon* p, std::set<Balloon*>& nodes);

int main() 
{
    std::cout << "start of main" << std::endl;

    // ====================================================
    // CYCLIC STRUCTURES
    // ====================================================

    Balloon* jacob(new Balloon("Dora the Explorer"));
    Balloon* katherine(new Balloon("Kung Fu Panda"));
    Balloon* larry(new Balloon("Scooby Doo"));
    Balloon* miranda(new Balloon("SpongeBob SquarePants"));
    Balloon* nicole(new Balloon("Papa Smurf"));

    jacob->addRope(katherine);
    katherine->addRope(larry);
    larry->addRope(jacob);
    miranda->addRope(jacob);
    nicole->addRope(miranda);
    larry->addRope(nicole);

    katherine = NULL;
    larry = NULL;
    miranda = NULL;
    nicole = NULL;

    deleteAll(jacob);
    jacob = NULL;

    std::cout << "end of main" << std::endl;
    return 0;
}

// ====================================================
// ====================================================

void deleteAll(Balloon* p)
{
    std::set<Balloon*> nodes;
    deleteAllHelper(p, nodes); 

    for (std::set<Balloon*>::iterator it = nodes.begin(); it != nodes.end(); it++) {
        delete *it; 
    }
}

void deleteAllHelper(Balloon* p, std::set<Balloon*>& nodes)
{
    if (p == NULL)
        return;

    for (int c = 0; c < p->numRopes(); c++) {
        Balloon* next = p->getRope(c); 
        if (nodes.find(next) != nodes.end())
            continue; 

        nodes.insert(next); 
        deleteAllHelper(next, nodes); 
    } 
}