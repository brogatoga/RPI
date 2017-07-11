#include "stat.h"
#include <vector>
#include <map>
#include <iostream>
#include <iomanip>
stat::stat(){
}

float stat::avg_turn(){
	int sum; 
	for (int i=0; i<(signed)ts.size(); i++){
		sum=sum+ts[i];
	}
	float avg=(float)sum/(ts.size());
	return avg;
} 
float stat::avg_wait(){
	int sum; 
	for (int i=0; i<(signed)ws.size(); i++){
		sum=sum+ws[i];
	}
	float avg=(float)sum/(ws.size());
	return avg;
}
float stat::avg_cpu_util(){
	int final_sum=0; 
	for (int c=1; c<=(signed)bursts_per_cpu.size(); c++){
		int sum=0;
		std::vector<int> v=bursts_per_cpu[c]; 
		for (int i=0; i<(signed)v.size();i++){
			sum=sum+v[i];
		}
		final_sum=sum+final_sum;
	}
	int number=bursts_per_cpu.size();
	//(cpu1burst+cpu2burst+cpu3burst+cpu4burst)/number of cpu/total_time 
	//equals [(cpu1burst/total_time)+(cpu2burst/total_time)+(cpu3burst/total_time)+(cpu4burst/total_time)]/number of cpu
	float avg_final=(float)final_sum/total_time;
	avg_final=avg_final/number*100;

	return avg_final;
}


void stat::avg_cpu_util_per_process(){

	std::cout<<"Average CPU utilization per process:"<<std::endl;
	
	//store total_turn around times for each process 
	std::vector<int> total_per_process; 

	//for each process, sum up the turn around times 
	for (int i=1; i<(signed)tr_per_process.size()+1; i++){
		std::vector<int> v=tr_per_process[i];
		int process_t=0; 
		for (int j=0; j<(signed)v.size(); j++){
			process_t=v[j]+process_t; 
		}
		total_per_process.push_back(process_t);
	}


	//use the total_turn around time process as the total time for each process 

	for (int i=1; i<(signed)bursts_per_process.size()+1; i++){
		std::vector<int> g=bursts_per_process[i];
		int sum=0; 
		for (int y=0; y<(signed)g.size(); y++){
			sum=sum+g[y];
		}

		float avg=(float)sum/total_per_process[i-1]; 
		float percent=avg*100;
		
		std::cout << std::fixed << std::setprecision(3);
		std::cout<<"process "<<i<<": "<<percent<<"%"<<std::endl;
	}


	std::cout<<std::endl;

}