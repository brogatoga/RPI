// =======================================================================
//
// IMPORTANT NOTE: You should edit this file
//
//    This file provides the code for ASCII art printing of trains.
//    You should implement all of the functions prototyped in
//    "traincar_prototypes.h" in this file.
//
// =======================================================================


#include <iostream>
#include <iomanip>
#include <string>
#include <cassert>
#include <cmath>
#include <cstdlib>

#include "traincar.h"



// =======================================================================
// =======================================================================

// This helper function checks that the forward and backward pointers
// in a doubly-linked structure are correctly and consistently assigned.
void SanityCheck(TrainCar* train) {
  // an empty train is valid
  if (train == NULL) return;
  assert (train->prev == NULL);
  TrainCar *tmp = train;
  while (tmp->next != NULL) {
    // the next train better point back to me
    assert (tmp->next->prev == tmp);
    tmp = tmp->next;
  }
}


// This helper function prints one of the 5 rows of the TrainCar ASCII art
void PrintHelper(TrainCar* t, int which_row) {
  if (t == NULL) {
    // end of the line
    std::cout << std::endl;
    return;
  }

  if (which_row == 0) {
    // the top row only contains "smoke" for engine traincars
    if (t->isEngine()) {
      std::cout << "     ~~~~";
    } else {
      std::cout << "         ";
    }
  } else if (which_row == 1) {
    // the 2nd row only contains the smoke stack for engine traincars
    if (t->isEngine()) {
      std::cout << "    ||   ";
    } else {
      std::cout << "         ";
    }
  } else if (which_row == 2) {
    // the 3rd row contains the ID for each traincar
    // (and engine traincars are shaped a little differently)
    if (t->isEngine()) {
      std::cout << "   " << std::setw(6) << std::setfill('-') << t->getID();
    } else {
      std::cout << std::setw(9) << std::setfill('-') << t->getID();
    }
    std::cout << std::setfill(' ');
  } else if (which_row == 3) {
    // the 4th row is different for each TrainCar type
    if (t->isEngine()) {
      std::cout << " / ENGINE";
    } else if (t->isFreightCar()) {
      // freight cars display their weight
      std::cout << "|" << std::setw(5) << t->getWeight() << "  |";
    } else if (t->isPassengerCar()) {
      // passenger cars are simple empty boxes
      std::cout << "|       |";
    } else if (t->isDiningCar()) {
      std::cout << "|  dine |";
    } else {
      assert (t->isSleepingCar());
      std::cout << "| sleep |";
    }
  } else if (which_row == 4) {
    // final row is the same for all cars, just draw the wheels
    std::cout << "-oo---oo-";
  }

  // between cars display the '+' link symbol on the 5th row 
  // (only if there is a next car)
  if (t->next != NULL) {
    if (which_row == 4) {
      std::cout << " + ";
    } else {
      std::cout << "   ";
    }
  }

  // recurse to print the rest of the row
  PrintHelper(t->next, which_row);
}


void PrintTrain(TrainCar* train) {
  
  if (train == NULL) { 
    std::cout << "PrintTrain: empty train!" << std::endl; 
    return; 
  }

  // Print each of the 5 rows of the TrainCar ASCII art
  PrintHelper(train, 0);
  PrintHelper(train, 1);
  PrintHelper(train, 2);
  PrintHelper(train, 3);
  PrintHelper(train, 4);

  // UNCOMMENT THESE ADDITIONAL STATISTICS AS YOU WORK

  int total_weight,num_engines,num_freight_cars,num_passenger_cars,num_dining_cars,num_sleeping_cars;
  CountEnginesAndTotalWeight
    (train, total_weight,
     num_engines, num_freight_cars, num_passenger_cars, num_dining_cars, num_sleeping_cars);
  int total_cars = num_engines+num_freight_cars+num_passenger_cars+num_dining_cars+num_sleeping_cars;
  float speed = CalculateSpeed(train);
  std::cout << "#cars = " << total_cars;
  std::cout << ", total weight = " << total_weight;
  std::cout << ", speed on 2% incline = " << std::setprecision(1) << std::fixed << speed;

  // If there is at least one passenger car, print the average
  // distance to dining car statistic
  if (num_passenger_cars > 0) {
    float dist_to_dining = AverageDistanceToDiningCar(train);
    if (dist_to_dining < 0) {
      // If one or more passenger cars are blocked from accessing the
      // dining car (by an engine car) then the distance is infinity!
      std::cout << ", avg distance to dining = inf";
    } else {
      std::cout << ", avg distance to dining = " << std::setprecision(1) << std::fixed << dist_to_dining;
    }
  }

  // If there is at least one sleeping car, print the closest engine
  // to sleeper car statistic
  if (num_sleeping_cars > 0) {
    int closest_engine_to_sleeper = ClosestEngineToSleeperCar(train);
    std::cout << ", closest engine to sleeper = " << closest_engine_to_sleeper;
  }

  std::cout << std::endl;
}


// =======================================================================
// =======================================================================



void PushBack(TrainCar* &train, TrainCar *car) {
  if (train == NULL) {
    // empty initial train (or at last link in chain)
    train = car;
  } else {
    // recurse down the chain (this will connect the forward link)
    PushBack(train->next,car);
    assert (train->next != NULL);
    // connect the backward link
    train->next->prev = train;
  }
}


void DeleteAllCars(TrainCar* car) {
  if (car != NULL) {
    // recurse down the chain first
    DeleteAllCars(car->next);
    // then delete this link
    delete car;
  }
}


// =======================================================================
// =======================================================================

void CountEnginesAndTotalWeight(TrainCar *t, 
                                int &total_weight,
                                int &num_engines, 
                                int &num_freight_cars,
                                int &num_passenger_cars,
                                int &num_dining_cars,
                                int &num_sleeping_cars) {
  // zero out the return values
  total_weight = 0;
  num_engines = 0;
  num_freight_cars = 0;
  num_passenger_cars = 0;
  num_dining_cars = 0;
  num_sleeping_cars = 0;
  // walk through the chain, incrementing the correct counters
  TrainCar *tmp = t;
  while (tmp != NULL) {
    total_weight += tmp->getWeight();
    if (tmp->isEngine()) {
      num_engines++;
    } else if (tmp->isFreightCar()) {
      num_freight_cars++;
    } else if (tmp->isPassengerCar()) {
      num_passenger_cars++;
    } else if (tmp->isDiningCar()) {
      num_dining_cars++;
    } else if (tmp->isSleepingCar()) {
      num_sleeping_cars++;
    }
    tmp = tmp->next;
  }
}


float SpeedFormula(int num_engines, float weight) {
  float horsepower = 3000 * num_engines;
  return (horsepower * 550 * 3600) / (20 * 2 * 5280 * weight);
}


float CalculateSpeed(TrainCar* t) {
  assert (t != NULL);
  int total_weight,num_engines,num_freight_cars,num_passenger_cars,num_dining_cars,num_sleeping_cars;
  CountEnginesAndTotalWeight(t, total_weight, num_engines, num_freight_cars, 
                             num_passenger_cars, num_dining_cars, num_sleeping_cars);              
  assert (total_weight > 0);
  return SpeedFormula(num_engines,total_weight);
}

// =======================================================================
// =======================================================================

// distance to dining for a single car
int DistanceToDining(TrainCar* train) {
  assert (train != NULL);
  assert (train->isPassengerCar());
  TrainCar* forward,*backward;
  forward = train->prev;
  backward = train->next;
  int answer = 1;
  // walk forward and backward until a dining car or a deadend is reached
  while(1) {
    if ((forward == NULL || forward->isEngine() || forward->isFreightCar()) &&
        (backward == NULL || backward->isEngine() || backward->isFreightCar())) {
      return -1;
    }
    if (forward != NULL && !forward->isEngine() && !forward->isFreightCar()) {
      if (forward->isDiningCar()) return answer;
      forward = forward->prev;
    }
    if (backward != NULL && !backward->isEngine() && !backward->isFreightCar()) {
      if (backward->isDiningCar()) return answer;
      backward = backward->next;
    }
    answer++;
  }
}

float AverageDistanceToDiningCar(TrainCar* train) {
  float answer = 0;
  int numPassengerCars = 0;
  // for each passenger car, calculate the distance to dining
  while (train != 0) {
    if (train->isPassengerCar()) {
      int tmp = DistanceToDining(train);
      if (tmp == -1) return -1;
      answer += tmp;
      numPassengerCars++;
    }
    train = train->next;
  }
  if (numPassengerCars == 0) return 0;
  return answer / numPassengerCars;
}

// similar to distance to dining
int DistanceToEngine(TrainCar* train) {
  assert (train != NULL);
  assert (train->isSleepingCar());
  TrainCar* forward,*backward;
  forward = train->prev;
  backward = train->next;
  int answer = 1;
  // walk forward and backward until an engine or a deadend is reached
  while(1) {
    if (forward == NULL && backward == NULL) return -1;
    if (forward != NULL) {
      if (forward->isEngine()) return answer;
      forward = forward->prev;
    }
    if (backward != NULL) {
      if (backward->isEngine()) return answer;
      backward = backward->next;
    }
    answer++;
  }
}


// minimum of all sleeping cars
int ClosestEngineToSleeperCar(TrainCar* train) {
  int answer = -1;
  while (train != 0) {
    int tmp = -1;
    if (train->isSleepingCar())
      tmp = DistanceToEngine(train);
    if (answer == -1 || (tmp != -1 && tmp < answer)) {
      answer = tmp;
    }
    train = train->next;
  }
  return answer;
}

// =======================================================================
// =======================================================================

std::vector<TrainCar*> ShipFreight(TrainCar* &engines, TrainCar* &cargo, float minimum_speed, int max_cars) {
  std::vector<TrainCar*> answer;

  // while there is at least one engine and at least one cargo, let's ship a train!
  while (cargo != NULL && engines != NULL) {

    // how many engines & how many cars should this train contain?
    int num_engines = 1;
    int num_cars = 0;
    int weight = engines->getWeight();
    // the remaining cargo & engines after this train is sent
    TrainCar *tmp_cargo = cargo;
    TrainCar *tmp_engines = engines->next;

    // if the train has not reached the maximum length
    while (num_engines + num_cars < max_cars && tmp_cargo != NULL) {
      // consider adding the next freight car....
      int tmp_weight = weight + tmp_cargo->getWeight();
      float speed = SpeedFormula(num_engines,tmp_weight);
      if (speed >= minimum_speed) {
        tmp_cargo = tmp_cargo->next;
        num_cars++;
        weight = tmp_weight;
      } else {
        // need to add an engine before adding this freight car
        float avg_cars_per_engine = num_cars / float(num_engines);
        int exp_num = (num_engines+1)*(avg_cars_per_engine*1);
        // if the train is close to the max length, it isn't worth
        // adding an engine (better to save for another train)
        if (tmp_engines == NULL || exp_num > max_cars) break;
        num_engines++;
        weight+=engines->getWeight();
        tmp_engines = tmp_engines->next;
        continue;
      }
    }

    // build the train
    assert (num_engines > 0 && num_cars > 0);
    TrainCar *last_engine;
    TrainCar *t = engines;
    for (int i = 0; i < num_engines; i++) {
      last_engine = engines;
      engines = engines->next;
    }
    assert (last_engine->next == tmp_engines);
    if (engines != NULL)
      engines->prev = NULL;
    last_engine->next = cargo;
    cargo->prev = last_engine;
    cargo = tmp_cargo;
    if (cargo != NULL) {
      cargo->prev->next = NULL;
      cargo->prev = NULL;
    }
    SanityCheck(t);
    answer.push_back(t);
  }

  return answer;
}

// =======================================================================
// =======================================================================

void Separate(TrainCar* &input, TrainCar* &left, TrainCar* &right) {
  
  // acquire data on the train
  int total_weight,num_engines,num_freight_cars,num_passenger_cars,num_dining_cars,num_sleeping_cars;
  CountEnginesAndTotalWeight(input, total_weight, num_engines, num_freight_cars,
                             num_passenger_cars, num_dining_cars, num_sleeping_cars);              
  assert (num_engines >= 2);

  // decide how many non engine cars are needed in each train to make them roughly equal
  int target_num_engines_a = num_engines/2;
  int target_num_engines_b = num_engines - target_num_engines_a;
  int weight_without_engines = total_weight-num_engines*150;
  float target_weight_a = weight_without_engines * (target_num_engines_a / float(num_engines));
  float target_weight_b = weight_without_engines * (target_num_engines_b / float(num_engines));
  assert (fabs(target_weight_a + target_weight_b + num_engines*150 - total_weight) < 0.001); 

  // try each split point, searching for the best split
  TrainCar* best_split = NULL;
  int best_weight_error;
  int best_engines_to_move;
  TrainCar* current_split = input->next;
  int current_weight=(input->isEngine()) ? 0 : input->getWeight();
  int current_engines=(input->isEngine()) ? 1 : 0;

  // consider each splot in the chain
  while (current_split != NULL) {
    float weight_error;
    int engines_to_move;
    // determine how close this weight split is to the ideal weight
    // split and how many engines must be moved
    if (fabs(current_weight-target_weight_a) <= fabs(current_weight-target_weight_b) ) {
      weight_error = fabs(current_weight-target_weight_a);
      engines_to_move = current_engines-target_num_engines_a;
    } else {
      weight_error = fabs(current_weight-target_weight_b);
      engines_to_move = current_engines-target_num_engines_b;
    }
    // keep track of the best splot
    if (best_split == NULL ||
        weight_error < best_weight_error ||
        ( fabs(weight_error - best_weight_error) < 0.001 &&
          abs(engines_to_move) < abs(best_engines_to_move) ) ) {
      best_split = current_split;
      best_weight_error = weight_error;
      best_engines_to_move = engines_to_move;
    }
    if (current_split->isEngine()) {
      current_engines++;
    } else {
      current_weight+=current_split->getWeight();
    }
    current_split = current_split->next;
  }


  // cut the train in half
  left = input;
  right = best_split;
  TrainCar *left_end = best_split->prev;
  left_end->next = NULL;
  right->prev = NULL;
  input = NULL;

  // move the necessary number of engines the least total distance
  while (best_engines_to_move > 0) {
    // find right most engine...
    TrainCar* tmp = left_end;
    while (!tmp->isEngine()) { tmp = tmp->prev; assert (tmp != NULL); }
    assert (tmp->prev != NULL);
    assert (tmp->next != NULL);
    tmp->prev->next = tmp->next;
    tmp->next->prev = tmp->prev;
    right->prev = tmp;
    tmp->prev = NULL;
    tmp->next = right;
    right = tmp;
    best_engines_to_move -= 1;
  }
  while (best_engines_to_move < 0) {
    // find left most engine...
    TrainCar* tmp = right;
    while (!tmp->isEngine()) { tmp = tmp->next; assert (tmp != NULL); }
    assert (tmp->prev != NULL);
    assert (tmp->next != NULL);
    tmp->prev->next = tmp->next;
    tmp->next->prev = tmp->prev;
    left_end->next = tmp;
    tmp->prev = left_end;
    left_end = tmp;
    tmp->next = NULL;
    best_engines_to_move += 1;
  }

  assert (best_engines_to_move == 0);
}

// =======================================================================
// =======================================================================
