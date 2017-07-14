#include <vector> 

float CalcSpeed(float horsepower, float weight); 
float CalcHorsepower(float speed, float weight); 
float CalcWeight(float horsepower, float speed); 

void PrependCar(TrainCar*& train, TrainCar*& car);
void AppendCar(TrainCar*& train, TrainCar*& car);

void PushBack(TrainCar*& train, TrainCar* car);
void DeleteAllCars(TrainCar*& train);
void CountEnginesAndTotalWeight(const TrainCar* train, int& total_weight, int& num_engines, int& num_freight_cars, 
	int& num_passenger_cars, int &num_dining_cars, int &num_sleeping_cars);

float CalculateSpeed(const TrainCar* train); 
float AverageDistanceToDiningCar(const TrainCar* train); 
int ClosestEngineToSleeperCar(const TrainCar* train);

std::vector<TrainCar*> ShipFreight(TrainCar*& engines, TrainCar*& freight, float min_speed, int max_cars); 
void Separate(TrainCar*& train, TrainCar*& a, TrainCar*& b);