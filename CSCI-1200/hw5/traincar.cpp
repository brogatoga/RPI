#include <iostream>
#include <iomanip>
#include <string>
#include <cmath>
#include <cassert>
#include "traincar.h"


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


// Calculates and returns the speed given horsepower and weight
float CalcSpeed(float horsepower, float weight)
{
  // ** need to calculate in multiple steps to prevent overflowing
  float result = float(horsepower * 550 / 40); 
  result *= 3600; 
  result /= (5280 * weight); 
  return result; 
}



// Calculates and returns the horsepower needed to move the train at given speed and weight
float CalcHorsepower(float speed, float weight)
{
  float result = (speed * weight * 20 * 2) / 3600; 
  result *= 5280; 
  result /= 550; 
  return result; 
} 


// Calculates and returns the maximum weight allowed given the current horsepower and speed
float CalcWeight(float horsepower, float speed)
{
  float result = (horsepower * 550) / (20 * 2); 
  result *= 3600; 
  result /= 5280; 
  result /= speed; 
  return result; 
}


// Adds the first car in the train to the beginning of the train, updates pointers accordingly
void PrependCar(TrainCar*& train, TrainCar*& car)
{
  if (car == NULL)
    return;

  TrainCar* next_car = car->next;    // next car after the first one (which will be taken out)

  // If this is the first node, then we need to do things a bit differently
  if (train == NULL) {
    train = car; 
    train->next = NULL; 
    train->prev = NULL; 
  }
  else {
    TrainCar* second = train;     // used to be the first car in the train, now it will be the second
    train = car; 
    train->prev = NULL;
    train->next = second; 

    if (second != NULL)
      second->prev = train; 
  }

  if (next_car != NULL) {
    next_car->prev = NULL; 
    car = next_car;
  } 
  else car = NULL; 
}


// Adds the first car in the list to the end of the train, and updates pointers accordingly
void AppendCar(TrainCar*& train, TrainCar*& car)
{
  if (car == NULL)
    return;

  // If this is the first car in the train, then same behavior as prepending the car
  if (train == NULL) {
    PrependCar(train, car);
    return;
  }

  TrainCar* next_car = car->next;   // the car after the first car that will be taken out
  PushBack(train, car); 

  if (next_car != NULL) {
    next_car->prev = NULL; 
    car = next_car;
  } 
  else car = NULL; 
}


// Pushes a new car onto the back of the train
void PushBack(TrainCar*& train, TrainCar* car)
{
  if (train == NULL) {
    train = car; 
    train->prev = train->next = NULL; 
    return;
  }

  TrainCar* last = train; 
  while (last->next != NULL)
    last = last->next; 

  last->next = car; 
  last->next->next = NULL; 
  last->next->prev = last; 
}


// Counts the weight, total engines (and different types of cars too) of a train overall
void CountEnginesAndTotalWeight(const TrainCar* train, int& total_weight, int& num_engines, int& num_freight_cars, 
  int& num_passenger_cars, int &num_dining_cars, int &num_sleeping_cars)
{
  const TrainCar* car = train; 
  int tw = 0, ne = 0, nf = 0, np = 0, nd = 0, ns = 0; 

  while (car != NULL) {
    if (car->isEngine()) ne++;
    else if (car->isFreightCar()) nf++; 
    else if (car->isPassengerCar()) np++; 
    else if (car->isDiningCar()) nd++; 
    else if (car->isSleepingCar()) ns++; 
    tw += car->getWeight(); 
    car = car->next; 
  }

  total_weight = tw, num_engines = ne, num_freight_cars = nf, 
  num_passenger_cars = np, num_dining_cars = nd, num_sleeping_cars = ns; 
}


// Deletes all cars on a train and deallocates all allocated memory
void DeleteAllCars(TrainCar*& train)
{
  if (train == NULL)
    return;

  TrainCar* car = train; 
  TrainCar* next = NULL; 

  while (car != NULL) {
    next = car->next; 
    delete car; 
    car = next; 
  }
}


// Calculates the speed of the given train on a 2% incline
float CalculateSpeed(const TrainCar* train)
{
  int horsepower = 0; 
  int weight = 0; 

  for (const TrainCar* car = train; car != NULL; car = car->next) {
    if (car->isEngine())
      horsepower += 3000;  
    weight += car->getWeight(); 
  }
  return CalcSpeed(horsepower, weight);
}


// Calculates and returns the average distance to the dining car
float AverageDistanceToDiningCar(const TrainCar* train)
{
  int total_distance = 0; 
  int num_passenger_cars = 0; 
  int passenger_index = -1;
  int counter = 0; 

  for (const TrainCar* a = train; a != NULL; a = a->next) {

    // If this is a passenger car, then calculate the distance to nearest dining car and record it
    if (a->isPassengerCar()) {
      num_passenger_cars++; 
      passenger_index = counter;
      int closest_distance = -1, distance = 0; 

      // Search backward to find dining car
      int back_counter = counter; 
      for (const TrainCar* b = a; b != train && b != NULL && !b->isEngine(); b = b->prev) {
         if (b->isDiningCar()) {
            distance = passenger_index - back_counter; 
            if (closest_distance > distance || closest_distance == -1)
              closest_distance = distance; 
         }
         back_counter--; 
      }

      // Search forward to find dining car
      int forward_counter = counter; 
      for (const TrainCar* b = a; b != NULL && !b->isEngine(); b = b->next) {
        if (b->isDiningCar()) {
            distance = forward_counter - passenger_index; 
            if (closest_distance > distance || closest_distance == -1)
              closest_distance = distance; 
         }
         forward_counter++; 
      }

      // If dining car not found or blocked by engine, then return error code to signal infinity
      if (closest_distance <= 0) 
        return -1; 

      // Otherwise, add it to the total distance
      else 
        total_distance += closest_distance; 
    }
    counter++; 
  }
  return (total_distance > 0 && num_passenger_cars > 0) ? (float)total_distance / num_passenger_cars : -1; 
}


// Calculates and returns the closest distance between an engine and any sleeping car
int ClosestEngineToSleeperCar(const TrainCar* train)
{
  int closest_distance = -1; 
  int engine_location = -1, sleeper_location = -1;
  int counter = 0; 

  for (const TrainCar* car = train; car != NULL; car = car->next) {
    if (car->isEngine())
      engine_location = counter; 
    else if (car->isSleepingCar())
      sleeper_location = counter; 

    if (engine_location >= 0 && sleeper_location >= 0 && engine_location != sleeper_location) {
      int distance = std::abs(engine_location - sleeper_location);
      if (distance < closest_distance || closest_distance == -1)
        closest_distance = distance; 
    }
    counter++; 
  }
  return closest_distance; 
}


// Returns a list of trains that carry all the freight on them
std::vector<TrainCar*> ShipFreight(TrainCar*& engines, TrainCar*& freight, float min_speed, int max_cars)
{
  if (engines == NULL || freight == NULL || min_speed <= 0 || max_cars <= 0)
    return std::vector<TrainCar*>();

  std::vector<TrainCar*> trains; 


  // Perform some calculations on weight, engines, weight-per-engine, freight count, etc. 
  float total_weight = 0; 
  float weight_per_engine = CalcWeight(3000, min_speed);
  for (const TrainCar* car = freight; car != NULL; car = car->next)
    total_weight += car->getWeight();


  // Create the trains to hold the freight (one train per iteration)
  while (engines != NULL && freight != NULL) {
    TrainCar* current_train = NULL;
    float available_weight = (weight_per_engine - engines->getWeight());
    int current_length = 1; 

    PrependCar(current_train, engines);

    // Add the freight to the current train
    while (current_length < max_cars && freight != NULL) {
      // If current freight can be added to the train, then add it
      if (freight->getWeight() <= available_weight) {
        available_weight -= freight->getWeight(); 
        current_length++;
        AppendCar(current_train, freight);
      }

      // If not, then if we have room for two more cars (an engine and the freight), then add it anyway
      else {
        if (max_cars - current_length >= 2 && engines != NULL) {
          current_length += 2; 
          available_weight += (weight_per_engine - freight->getWeight() - engines->getWeight());
          PrependCar(current_train, engines);
          AppendCar(current_train, freight);
        }
        else break;     // If not, then terminate loop and add this train, it's done
      }
    }

    // Add train to list of trains returned
    trains.push_back(current_train); 
  }
  return trains; 
}


// Separates a long train into two smaller trains A and B
void Separate(TrainCar*& train, TrainCar*& a, TrainCar*& b)
{
  if (train == NULL || train->next == NULL) {
    a = b = NULL; 
    return;
  }

  /*
  int num_engines = 0;
  int train_length = 0; 
  int total_weight = 0; 

  // Calculate number of engines and train length
  for (TrainCar* car = train; car != NULL; car = car->next) {
    if (car->isEngine())
      num_engines++; 
    train_length++; 
    total_weight += car->getWeight(); 
  }

  // Pre-initialize child trains
  a = NULL; 
  b = NULL; 

  // If there are only two engines, then place one engine in each child train
  if (num_engines == 2) {
    TrainCar* car; 
    TrainCar* second_engine = NULL; 
    bool found_first_engine = false; 
    int count; 

    for (car = train, count = 0; car != NULL && count <= train_length / 2; car = car->next, count++) {
      if (car->isEngine()) {
        if (!found_first_engine) {
          PrependCar(a, car);
          found_first_engine = true; 
        }
        else second_engine = car; 
      }
      else 
        AppendCar(a, car); 
    }

    if (second_engine != NULL)
      PrependCar(b, second_engine); 
    
    for (; car != NULL && count < train_length; car = car->next, count++) {
      if (car->isEngine())
        PrependCar(b, car); 
      else
        AppendCar(b, car);
    }
  }


  // If there are more than two engines, then (???)
  else {

  }
  */

  // lol...
  TrainCar* car = train->next; 
  TrainCar* prev = train; 

  while (car != NULL) {
    if (car->isEngine()) {
      a = train; 
      prev->next = NULL; 
      b = car; 
      b->prev = NULL; 
      train = NULL;  
      return;
    }
    prev = car; 
    car = car->next; 
  }
}


// Prints the details and summary statistics of a train
void PrintTrain(TrainCar* train)
{ 
  if (train == NULL) { 
    std::cout << "PrintTrain: empty train!" << std::endl; 
    return; 
  }

  PrintHelper(train, 0);
  PrintHelper(train, 1);
  PrintHelper(train, 2);
  PrintHelper(train, 3);
  PrintHelper(train, 4);

  int total_weight = 0;
  int num_engines = 0;
  int num_freight_cars = 0;
  int num_passenger_cars = 0;
  int num_dining_cars = 0;
  int num_sleeping_cars = 0;

  CountEnginesAndTotalWeight(train, total_weight, num_engines, num_freight_cars, num_passenger_cars, 
    num_dining_cars, num_sleeping_cars);
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