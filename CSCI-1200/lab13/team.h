#ifndef _TEAM_H_
#define _TEAM_H_

#include <string>
#include <iostream>
#include <list>

#include "superhero.h"

class Team {

public:
 

  // accessor function
  std::string getName() const;

  // adding and removing a single superhero
  void operator+=(const Superhero &a) {
    superheroes.push_back(a); 
  }
  void operator-=(const Superhero &a) {
     for (std::list<Superhero>::iterator it = superheroes.begin(); it != superheroes.end(); it++) {
       if (it->getName() == a.getName()) {
          superheroes.erase(it); 
          break;
       }
     }
  }

  // adding two teams together or a team and a superhero together
  // makes a team.  these functions must be member functions to allow
  // access to the list of superheroes.
  friend Team operator+(const Team &a, const Team &b);
  friend Team operator+(const Team &a, const Superhero &b);
  
private:
  // REPRESENATION
  std::list<Superhero> superheroes;
};


#endif
