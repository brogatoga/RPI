// =======================================================================
//
// IMPORTANT NOTE: You should edit this file
//                 This file is #include'd from traincar.h
//
// =======================================================================
//
// There are a number of additional functions used in main.cpp that
// you need to declare and implement.  Study main.cpp and the provided
// output to determine the arguments, return type, and overall
// behavior.
//
// add all required additional function prototypes here
// (you may also add your own helper function prototypes here too)
//
// implement these functions in traincar.cpp
//

#include <vector>

void PushBack(TrainCar* &train, TrainCar *car);

void DeleteAllCars(TrainCar* train);

void CountEnginesAndTotalWeight
(TrainCar *t, int &total_weight, int &num_engines, int &num_freight_cars,
 int &num_passenger_cars, int &num_dining_cars, int &num_sleeping_cars);

float CalculateSpeed(TrainCar* train);

float AverageDistanceToDiningCar(TrainCar* train);

int ClosestEngineToSleeperCar(TrainCar* train);

double MaxDistanceToDiningCar(TrainCar* train);

std::vector<TrainCar*> ShipFreight
(TrainCar* &engines, TrainCar* &freight_cars, float minimum_speed, int max_cars);

void Separate(TrainCar* &input, TrainCar* &train_a, TrainCar* &train_b);

