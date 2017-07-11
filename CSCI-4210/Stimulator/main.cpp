#include <iostream>
#include <iomanip>
#include <fstream>
#include <cstdlib>
#include <cstring>
#include <vector>
#include <string>
#include <queue>
#include <map>
#include <utility>
#include <algorithm>
#include <ctime>
#include <cmath>
#include "constants.h"
#include "process.h"
#include "stat.h"

// #define DEBUG

// Given a process type in enum form, returns the corresponding string form
std::string processTypeToString(ProcessType type)
{
	switch (type) {
		case Interactive: return std::string("Interactive"); break;
		case CpuBound:    return std::string("CPU-bound");   break;
	}
	return std::string("");
}


// Converts a vector to a queue and returns the result
template<class T> std::queue<T> vectorToQueue(const std::vector<T>& input)
{
	std::queue<T> result;
	int size = input.size();
	
	for (int c = 0; c < size; c++)
		result.push(input[c]);
	return result;
}


//================================================================
// Display functions 
//================================================================

void context_switch(int csTime, int& time, int oldID, int newID)
{
	if (oldID != newID) {
		std::cout << "[time " << time << "ms] Context switch (swapping out process ID " << oldID
			<< " for process ID " << newID << ")" <<std::endl;
	}
	else time -= csTime;
}

void burst_complete(int time, const Process& p)
{
	int turnaroundTime = p.getWaitTime() + p.getBurstTime();
	std::cout << "[time " << time << "ms] " << processTypeToString(p.getType()) << " process ID " << p.getID() << 
		" CPU burst done (turnaround time "<< turnaroundTime << "ms, total wait time " << p.getWaitTime() << "ms)" << std::endl;
}

void termination(int time, const Process& p)
{
	std::cout << "[time " << time << "ms] " << processTypeToString(p.getType()) << " process ID " << p.getID() <<
		" terminated (avg turnaround time Xms, avg total wait time Xms)" << std::endl;
}


void termination1(int time, const Process& p, std::vector<int> t, std::vector<int> w)
{	
	int total_w=0;
	int total_t=0; 
	for (int i=0; i<(signed)w.size(); i++){
		total_w=total_w+w[i]; 
	}
	float avg_w=(float)total_w/w.size();
	
	for (int i=0; i<(signed)t.size(); i++){
		total_t=total_t+t[i];
	}

	float avg_t=(float)total_t/t.size();
	std::cout << "[time " << time << "ms] " << processTypeToString(p.getType()) << " process ID " << p.getID() <<
		" terminated (avg turnaround time "<<avg_t<<"ms, avg total wait time "<<avg_w<< "ms)" << std::endl;
}

void started_running(int time, const Process& p)
{
#ifdef DEBUG
	std::cout << "[time " << time << "ms] Process " << p.getID() << " has started running" << std::endl;
#endif
}

void entered_ready_queue(int time, const Process& p)
{
#ifdef DEBUG
	std::cout << "[time " << time << "ms] Process " << p.getID() << " has re-entered the ready queue (blocked for "
		<< p.getBlockTime() << "ms)" << std::endl;
#endif
}

//================================================================
//=======Analysis
void analysis(stat s, int cpuCount){
	int min_wait=s.getMinWait();
	int max_wait=s.getMaxWait();
	int min_turn=s.getMinTurn();
	int max_turn=s.getMaxTurn();

	std::cout << std::fixed << std::setprecision(3);
	std::cout << std::endl;
	std::cout<<"Turnaround time: "<<"min "<<min_turn<< " ms; ";
	std::cout<<"avg "<<s.avg_turn()<<" ms; max "<<max_turn<< " ms"<<std::endl;
	std::cout<<"Total wait time: min "<<min_wait<<" ms; "; 
	std::cout<<"avg "<<s.avg_wait()<<" ms; max "<<max_wait<<" ms"<<std::endl;
	std::cout<<"Average CPU utilization: "<< s.avg_cpu_util()<< "%"<<std::endl;
	s.avg_cpu_util_per_process();
	
}



//=================================================================
// First come first serve scheduling algorithm
void FCFS(const std::vector<Process>& processList, int cpuCount, int csTime)
{
	if (processList.size() == 0 || cpuCount < 1 || csTime < 0)
		return;

	//For statistics/Analysis later 
	//=======================================
	stat s;
	std::vector<int> ts;
	std::vector<int> ws;
	int min_wait=9999999; 
	int max_wait=0; 
	int max_turn=-1; 
	int min_turn=9999999;
	
	std::map<int, std::vector<int> > Bursts_per_cpu;
	std::map<int, std::vector<int> > Bursts_per_process;
	std::map<int, std::vector<int> > Waits;
	std::map<int, std::vector<int> > Turns;


	//========================================

	
	std::queue<Process> readyQueue = vectorToQueue(processList);
	std::vector<Process> blockedList;
	std::map<int, Process> CPU;
	int processCount = processList.size();
	int numCPUProcesses = ceil(processCount * CPU_PROCESS_RATIO);
	int numCPUProcessesTerminated = 0;
	int currentTime = 0;
	bool run = true;
	
	// If there are X CPUs, then assign them the first X processes in the queue
	for (int c = 1; c <= cpuCount && c < processCount + 1; c++) {
		CPU[c] = readyQueue.front();
		CPU[c].startBurst();
		readyQueue.pop();
	}


	//=========================================
	//Initialize /basic setup for Burst map 
	for (int c = 1; c < processCount + 1; c++) {
		std::vector<int> x,y,z;
		Waits[c]=y; 
		Turns[c]=z;
		Bursts_per_process[c]=x;
	}

	for (int c=1; c<=cpuCount;c++){
		std::vector<int> x; 
		Bursts_per_cpu[c]=x;
	}
	//==========================================

	
	// Keep running until all CPU-bound processes have terminated
	while (run) {
		bool addedToBlock = false;
		//bool contextSwitched = false;
		
		// Move time forward
		currentTime++;
		
		
		// If a process is done with its CPU burst, then place it in the blocked list (or terminate it),
		// and then assign the CPU a new process.
		for (int c = 1; c <= cpuCount; c++) {
			CPU[c].tick();
			ProcessState state = CPU[c].getState();
			
			// If it's bursting, then let it do its thing
			if (state == Burst)
				continue;
			
			// If it's blocked now, then add it to the block list and do a context switch (only if there are more READY processes)
			else if (state == Blocked || state == CpuIdle) {
				
				// If a process is now in the blocked state, then this means that it has finished and now has to complete its I/O burst (or interative session0
				if (state == Blocked) {
					burst_complete(currentTime, CPU[c]);

					//==============================================
					// Store stats 

					int turnaround_time=CPU[c].getBurstTime()+ CPU[c].getWaitTime();
					ts.push_back(turnaround_time);
					int wait_time=CPU[c].getWaitTime();
					ws.push_back(CPU[c].getWaitTime());
					if (turnaround_time<min_turn){
						min_turn=turnaround_time; 
					}else if (turnaround_time>max_turn){
						max_turn=turnaround_time;
					}

					if (wait_time<min_wait){
						min_wait=wait_time;
					}else if (wait_time>max_wait){
						max_wait=wait_time;
					}

					Bursts_per_cpu[c].push_back(CPU[c].getBurstTime());
					
					int id=CPU[c].getID();
					int bt=CPU[c].getBurstTime();
				
					
					Waits[id].push_back(wait_time);
					Turns[id].push_back(turnaround_time);
					Bursts_per_process[id].push_back(bt);

					//===============================================


					CPU[c].resetWaitTime();
					blockedList.push_back(CPU[c]);
					addedToBlock = true;
				}
			 
				// If there is a new process in the ready queue, then perform a context switch and assign it to the CPU
				if (!readyQueue.empty()) {
					context_switch(csTime, currentTime, CPU[c].getID(), readyQueue.front().getID());
					// contextSwitched = true;
					currentTime += csTime;

					CPU[c] = readyQueue.front();
					CPU[c].tick();					// Increment the wait time of the new CPU process one more time
					CPU[c].startBurst();
					readyQueue.pop();
					started_running(currentTime, CPU[c]);
				}

				// If not, then this CPu core will be idle for now
				else CPU[c].setIdle();
			}
			
			// If it's terminated, then print the termination message and check if all CPU-bound
			// processes have been terminated.
			else if (state == Terminated) {
				burst_complete(currentTime, CPU[c]);

				//=======================================
				//store stats 
				int id=CPU[c].getID();
				// int b_time=CPU[c].getBurstTime();
				
					
				int wait_time=CPU[c].getWaitTime();
				
				ws.push_back(CPU[c].getWaitTime());
				int turnaround_time=CPU[c].getBurstTime()+ CPU[c].getWaitTime();
				
				ts.push_back(turnaround_time);
				Waits[id].push_back(wait_time);
				Turns[id].push_back(turnaround_time);
				if (turnaround_time<min_turn){
					min_turn=turnaround_time; 
				}else if (turnaround_time>max_turn){
					max_turn=turnaround_time;
				}

				if (wait_time<min_wait){
					min_wait=wait_time;
				}else if (wait_time>max_wait){
					max_wait=wait_time;
				}



				Bursts_per_cpu[c].push_back(CPU[c].getBurstTime());
				Bursts_per_process[id].push_back(CPU[c].getBurstTime());
				//=======================================
				int number=CPU[c].getID();

				termination1(currentTime, CPU[c], Turns[number], Waits[number]);
				CPU[c].setIdle();
				
				if (CPU[c].getType() == CpuBound) {
					numCPUProcessesTerminated++;
					if (numCPUProcessesTerminated == numCPUProcesses) {
						run = false;
						break;
					}
				}
			}
		}
		
		if (!run)
			break;
		
	
		// Update the wait times of processes in the READY queue
		int readyQueueSize = readyQueue.size();
		for (int c = 0; c < readyQueueSize; c++) {
			Process p = readyQueue.front();
			readyQueue.pop();
			p.tick();
			readyQueue.push(p);
		}
		
		
		// Update the processes in the blocked list. If they are no longer
		// blocked, then add them back into the READY queue.
		for (int c = blockedList.size() - 1; c >= 0; c--) {
			if ((addedToBlock && c < int(blockedList.size() - 1)) || !addedToBlock)
				blockedList[c].tick();
			
			if (blockedList[c].getState() == Ready) {
				entered_ready_queue(currentTime, blockedList[c]);
				readyQueue.push(blockedList[c]);
				blockedList.erase(blockedList.begin() + c);
			}
		}
	}

	s.set_ws(ws);
	s.set_ts(ts);
	s.setTotalTime(currentTime);
	s.setMinTurn(min_turn);
	s.setMinWait(min_wait);
	s.setMaxTurn(max_turn);
	s.setMaxWait(max_wait);
	
	s.setBurst_per_CPU(Bursts_per_cpu);
	s.setBurst_per_process(Bursts_per_process);
	s.setTurns(Turns);
	analysis(s, cpuCount);
}


// Shortest job first (nonpreemptive) scheduling algorithm
int shortestProcessFirst(const Process& a, const Process& b)
{
	return a.getBurstTime() < b.getBurstTime();
}

void SJF_NP(const std::vector<Process>& processList, int cpuCount, int csTime)
{
	if (processList.size() == 0 || cpuCount < 1 || csTime < 0)
		return;

	//For statistics/Analysis later 
	//=======================================
	stat s;
	std::vector<int> ts;
	std::vector<int> ws;
	int min_wait=9999999; 
	int max_wait=0; 
	int max_turn=-1; 
	int min_turn=9999999;
	
	std::map<int, std::vector<int> > Bursts_per_cpu;
	std::map<int, std::vector<int> > Bursts_per_process;
	std::map<int, std::vector<int> > Waits;
	std::map<int, std::vector<int> > Turns;
	//========================================






	std::vector<Process> processListCopy = processList;
	std::queue<Process> readyQueue;
	std::vector<Process> blockedList;
	std::map<int, Process> CPU;
	int processCount = processList.size();
	int numCPUProcesses = ceil(processCount * CPU_PROCESS_RATIO);
	int numCPUProcessesTerminated = 0;
	int currentTime = 0;
	bool run = true;

	// Sort the processes from shortest to longest before adding them into the queue
	std::sort(processListCopy.begin(), processListCopy.end(), shortestProcessFirst);
	readyQueue = vectorToQueue(processListCopy);

	// If there are X CPUs, then assign them the first X processes in the queue
	for (int c = 1; c <= cpuCount && c < processCount + 1; c++) {
		CPU[c] = readyQueue.front();
		CPU[c].startBurst();
		readyQueue.pop();
	}


		//=========================================
	//Initialize /basic setup for Burst map 
	for (int c = 1; c < processCount + 1; c++) {
		std::vector<int> x,y,z;
		Waits[c]=y; 
		Turns[c]=z;
		Bursts_per_process[c]=x;
	}

	for (int c=1; c<=cpuCount;c++){
		std::vector<int> x; 
		Bursts_per_cpu[c]=x;
	}
	//==========================================

	// Keep running until all CPU-bound processes have terminated
	while (run) {
		bool addedToBlock = false;
		//bool contextSwitched = false;

		// Move time forward
		currentTime++;


		// If a process is done with its CPU burst, then place it in the blocked list (or terminate it),
		// and then assign the CPU a new process.
		for (int c = 1; c <= cpuCount; c++) {
			CPU[c].tick();
			ProcessState state = CPU[c].getState();

			// If it's bursting, then let it do its thing
			if (state == Burst)
				continue;

			// If it's blocked now, then add it to the block list and do a context switch (only if there are more READY processes)
			else if (state == Blocked || state == CpuIdle) {

				// If a process is now in the blocked state, then this means that it has finished and now has to complete its I/O burst (or interative session0
				if (state == Blocked) {
					burst_complete(currentTime, CPU[c]);



					//==============================================
					// Store stats 

					int turnaround_time=CPU[c].getBurstTime()+ CPU[c].getWaitTime();
					ts.push_back(turnaround_time);
					int wait_time=CPU[c].getWaitTime();
					ws.push_back(CPU[c].getWaitTime());
					if (turnaround_time<min_turn){
						min_turn=turnaround_time; 
					}else if (turnaround_time>max_turn){
						max_turn=turnaround_time;
					}

					if (wait_time<min_wait){
						min_wait=wait_time;
					}else if (wait_time>max_wait){
						max_wait=wait_time;
					}

					Bursts_per_cpu[c].push_back(CPU[c].getBurstTime());
					
					int id=CPU[c].getID();
					int bt=CPU[c].getBurstTime();
				
					
					Waits[id].push_back(wait_time);
					Turns[id].push_back(turnaround_time);
					Bursts_per_process[id].push_back(bt);

					//===============================================



					CPU[c].resetWaitTime();
					blockedList.push_back(CPU[c]);
					addedToBlock = true;
				}

				// If there is a new process in the ready queue, then perform a context switch and assign it to the CPU
				if (!readyQueue.empty()) {
					context_switch(csTime, currentTime, CPU[c].getID(), readyQueue.front().getID());
					// contextSwitched = true;
					currentTime += csTime;

					CPU[c] = readyQueue.front();
					CPU[c].tick();					// Increment the wait time of the new CPU process one more time
					CPU[c].startBurst();
					readyQueue.pop();
					started_running(currentTime, CPU[c]);
				}

				// If not, then this CPu core will be idle for now
				else CPU[c].setIdle();
			}

			// If it's terminated, then print the termination message and check if all CPU-bound
			// processes have been terminated.
			else if (state == Terminated) {
				burst_complete(currentTime, CPU[c]);
				

				//=======================================
				//store stats 
				int id=CPU[c].getID();
				// int b_time=CPU[c].getBurstTime();
				
					
				int wait_time=CPU[c].getWaitTime();
				
				ws.push_back(CPU[c].getWaitTime());
				int turnaround_time=CPU[c].getBurstTime()+ CPU[c].getWaitTime();
				
				ts.push_back(turnaround_time);
				Waits[id].push_back(wait_time);
				Turns[id].push_back(turnaround_time);
				if (turnaround_time<min_turn){
					min_turn=turnaround_time; 
				}else if (turnaround_time>max_turn){
					max_turn=turnaround_time;
				}

				if (wait_time<min_wait){
					min_wait=wait_time;
				}else if (wait_time>max_wait){
					max_wait=wait_time;
				}



				Bursts_per_cpu[c].push_back(CPU[c].getBurstTime());
				Bursts_per_process[id].push_back(CPU[c].getBurstTime());
				//=======================================

				int number=CPU[c].getID();

				termination1(currentTime, CPU[c], Turns[number], Waits[number]);

				CPU[c].setIdle();

				if (CPU[c].getType() == CpuBound) {
					numCPUProcessesTerminated++;
					if (numCPUProcessesTerminated == numCPUProcesses) {
						run = false;
						break;
					}
				}
			}
		}

		if (!run)
			break;


		// Update the wait times of processes in the READY queue
		int readyQueueSize = readyQueue.size();
		for (int c = 0; c < readyQueueSize; c++) {
			Process p = readyQueue.front();
			readyQueue.pop();
			p.tick();
			readyQueue.push(p);
		}


		// Update the processes in the blocked list. If they are no longer
		// blocked, then add them back into the READY queue.
		for (int c = blockedList.size() - 1; c >= 0; c--) {
			if ((addedToBlock && c < int(blockedList.size() - 1)) || !addedToBlock)
				blockedList[c].tick();

			if (blockedList[c].getState() == Ready) {
				entered_ready_queue(currentTime, blockedList[c]);
				readyQueue.push(blockedList[c]);
				blockedList.erase(blockedList.begin() + c);
			}
		}


		// Resort the processes in the ready queue to ensure that the shortest process always goes first
		std::sort(processListCopy.begin(), processListCopy.end(), shortestProcessFirst);
	}
	
	s.set_ws(ws);
	s.set_ts(ts);
	s.setTotalTime(currentTime);
	s.setMinTurn(min_turn);
	s.setMinWait(min_wait);
	s.setMaxTurn(max_turn);
	s.setMaxWait(max_wait);
	s.setTurns(Turns);
	s.setBurst_per_CPU(Bursts_per_cpu);
	s.setBurst_per_process(Bursts_per_process);
	analysis(s, cpuCount);
}


// Preemptive shortest response time (SRT) scheduling algorithm
void SRT_P(const std::vector<Process>& processList, int cpuCount, int csTime)
{
	if (processList.size() == 0 || cpuCount < 1 || csTime < 0)
		return;


	//For statistics/Analysis later 
	//=======================================
	stat s;
	std::vector<int> ts;
	std::vector<int> ws;
	int min_wait=9999999; 
	int max_wait=0; 
	int max_turn=-1; 
	int min_turn=9999999;
	
	std::map<int, std::vector<int> > Bursts_per_cpu;
	std::map<int, std::vector<int> > Bursts_per_process;
	std::map<int, std::vector<int> > Waits;
	std::map<int, std::vector<int> > Turns;
	//========================================


	std::vector<Process> processListCopy = processList;
	std::queue<Process> readyQueue;
	std::vector<Process> blockedList;
	std::map<int, Process> CPU;
	int processCount = processList.size();
	int numCPUProcesses = ceil(processCount * CPU_PROCESS_RATIO);
	int numCPUProcessesTerminated = 0;
	int currentTime = 0;
	bool run = true;

	// Sort the processes from shortest to longest before adding them into the queue
	std::sort(processListCopy.begin(), processListCopy.end(), shortestProcessFirst);
	readyQueue = vectorToQueue(processListCopy);

	// If there are X CPUs, then assign them the first X processes in the queue
	for (int c = 1; c <= cpuCount && c < processCount + 1; c++) {
		CPU[c] = readyQueue.front();
		CPU[c].startBurst();
		readyQueue.pop();
	}

		//=========================================
	//Initialize /basic setup for Burst map 
	for (int c = 1; c < processCount + 1; c++) {
		std::vector<int> x,y,z;
		Waits[c]=y; 
		Turns[c]=z;
		Bursts_per_process[c]=x;
	}

	for (int c=1; c<=cpuCount;c++){
		std::vector<int> x; 
		Bursts_per_cpu[c]=x;
	}
	//==========================================


	// Keep running until all CPU-bound processes have terminated
	while (run) {
		bool addedToBlock = false;
		// bool contextSwitched = false;

		// Move time forward
		currentTime++;


		// If a process is done with its CPU burst, then place it in the blocked list (or terminate it),
		// and then assign the CPU a new process.
		for (int c = 1; c <= cpuCount; c++) {
			CPU[c].tick();
			ProcessState state = CPU[c].getState();

			// If it's bursting, then let it do its thing
			if (state == Burst)
				continue;

			// If it's blocked now, then add it to the block list and do a context switch (only if there are more READY processes)
			else if (state == Blocked || state == CpuIdle) {

				// If a process is now in the blocked state, then this means that it has finished and now has to complete its I/O burst (or interative session0
				if (state == Blocked) {
					burst_complete(currentTime, CPU[c]);

					//==============================================
					// Store stats 

					int turnaround_time=CPU[c].getBurstTime()+ CPU[c].getWaitTime();
					ts.push_back(turnaround_time);
					int wait_time=CPU[c].getWaitTime();
					ws.push_back(CPU[c].getWaitTime());
					if (turnaround_time<min_turn){
						min_turn=turnaround_time; 
					}else if (turnaround_time>max_turn){
						max_turn=turnaround_time;
					}

					if (wait_time<min_wait){
						min_wait=wait_time;
					}else if (wait_time>max_wait){
						max_wait=wait_time;
					}

					Bursts_per_cpu[c].push_back(CPU[c].getBurstTime());
					
					int id=CPU[c].getID();
					int bt=CPU[c].getBurstTime();
				
					
					Waits[id].push_back(wait_time);
					Turns[id].push_back(turnaround_time);
					Bursts_per_process[id].push_back(bt);

					//===============================================


					CPU[c].resetWaitTime();
					blockedList.push_back(CPU[c]);
					addedToBlock = true;
				}

				// If there is a new process in the ready queue, then perform a context switch and assign it to the CPU
				if (!readyQueue.empty()) {
					context_switch(csTime, currentTime, CPU[c].getID(), readyQueue.front().getID());
					// contextSwitched = true;
					currentTime += csTime;

					CPU[c] = readyQueue.front();
					CPU[c].tick();					// Increment the wait time of the new CPU process one more time
					CPU[c].startBurst();
					readyQueue.pop();
					started_running(currentTime, CPU[c]);
				}

				// If not, then this CPu core will be idle for now
				else CPU[c].setIdle();
			}

			// If it's terminated, then print the termination message and check if all CPU-bound
			// processes have been terminated.
			else if (state == Terminated) {
				burst_complete(currentTime, CPU[c]);


				//=======================================
				//store stats 
				int id=CPU[c].getID();
				// int b_time=CPU[c].getBurstTime();
				
					
				int wait_time=CPU[c].getWaitTime();
				
				ws.push_back(CPU[c].getWaitTime());
				int turnaround_time=CPU[c].getBurstTime()+ CPU[c].getWaitTime();
				
				ts.push_back(turnaround_time);
				Waits[id].push_back(wait_time);
				Turns[id].push_back(turnaround_time);
				if (turnaround_time<min_turn){
					min_turn=turnaround_time; 
				}else if (turnaround_time>max_turn){
					max_turn=turnaround_time;
				}

				if (wait_time<min_wait){
					min_wait=wait_time;
				}else if (wait_time>max_wait){
					max_wait=wait_time;
				}



				Bursts_per_cpu[c].push_back(CPU[c].getBurstTime());
				Bursts_per_process[id].push_back(CPU[c].getBurstTime());
				//=======================================

				int number=CPU[c].getID();

				termination1(currentTime, CPU[c], Turns[number], Waits[number]);


				//termination(currentTime, CPU[c]);
				CPU[c].setIdle();

				if (CPU[c].getType() == CpuBound) {
					numCPUProcessesTerminated++;
					if (numCPUProcessesTerminated == numCPUProcesses) {
						run = false;
						break;
					}
				}
			}
		}

		if (!run)
			break;


		// Update the wait times of processes in the READY queue
		int readyQueueSize = readyQueue.size();
		for (int c = 0; c < readyQueueSize; c++) {
			Process p = readyQueue.front();
			readyQueue.pop();
			p.tick();
			readyQueue.push(p);
		}


		// Update the processes in the blocked list. If they are no longer
		// blocked, then add them back into the READY queue.
		for (int c = blockedList.size() - 1; c >= 0; c--) {
			if ((addedToBlock && c < int(blockedList.size() - 1)) || !addedToBlock)
				blockedList[c].tick();

			// When a process has completed its I/O (or interactive) session and is ready to be put back
			// into the ready queue, check and see if it's burst time is shorter than the remaining times
			// of any process currently assigned to any of the CPU cores. If it is, then assign that CPU
			// this process and put its current process back into the ready queue.
			if (blockedList[c].getState() == Ready) {
				bool preempted = false;

				for (int d = 1; d <= cpuCount; d++) {
					if (blockedList[c].getBurstTime() < CPU[d].getRemainingBurst()) {
						
						// Preemption
						if (CPU[d].getState() != CpuIdle && CPU[d].getState() != Terminated) {
							CPU[d].preempt();
							readyQueue.push(CPU[d]);
						}
						
						context_switch(csTime, currentTime, CPU[d].getID(), blockedList[c].getID());
						// ccontextSwitched = true;
						currentTime += csTime;
						
						CPU[d] = blockedList[c];
						CPU[d].startBurst();
						blockedList.erase(blockedList.begin() + c);
						preempted = true;
						break;
					}
				}

				// If we are unable to preempt this process, then just add it to the ready queue
				if (!preempted) {
					entered_ready_queue(currentTime, blockedList[c]);
					readyQueue.push(blockedList[c]);
					blockedList.erase(blockedList.begin() + c);
				}
			}
		}


		// Resort the processes in the ready queue to ensure that the shortest process always goes first
		std::sort(processListCopy.begin(), processListCopy.end(), shortestProcessFirst);
	}
	
	s.set_ws(ws);
	s.set_ts(ts);
	s.setTotalTime(currentTime);
	s.setMinTurn(min_turn);
	s.setMinWait(min_wait);
	s.setMaxTurn(max_turn);
	s.setMaxWait(max_wait);
	s.setTurns(Turns);
	s.setBurst_per_CPU(Bursts_per_cpu);
	s.setBurst_per_process(Bursts_per_process);
	analysis(s, cpuCount);
}


// Round robin (RR) scheduling algorithm
void RR(const std::vector<Process>& processList, int cpuCount, int csTime, int rrTime)
{
	if (processList.size() == 0 || cpuCount < 1 || csTime < 0 || rrTime <= 0)
		return;


	//For statistics/Analysis later 
	//=======================================
	stat s;
	std::vector<int> ts;
	std::vector<int> ws;
	int min_wait=9999999; 
	int max_wait=0; 
	int max_turn=-1; 
	int min_turn=9999999;
	
	std::map<int, std::vector<int> > Bursts_per_cpu;
	std::map<int, std::vector<int> > Bursts_per_process;
	std::map<int, std::vector<int> > Waits;
	std::map<int, std::vector<int> > Turns;
	//========================================


	std::queue<Process> readyQueue = vectorToQueue(processList);
	std::vector<Process> blockedList;
	std::map<int, Process> CPU;
	int processCount = processList.size();
	int numCPUProcesses = ceil(processCount * CPU_PROCESS_RATIO);
	int numCPUProcessesTerminated = 0;
	int currentTime = 0;
	bool run = true;

	// If there are X CPUs, then assign them the first X processes in the queue
	for (int c = 1; c <= cpuCount && c < processCount + 1; c++) {
		CPU[c] = readyQueue.front();
		CPU[c].startBurst();
		readyQueue.pop();
	}


		//=========================================
	//Initialize /basic setup for Burst map 
	for (int c = 1; c < processCount + 1; c++) {
		std::vector<int> x,y,z;
		Waits[c]=y; 
		Turns[c]=z;
		Bursts_per_process[c]=x;
	}

	for (int c=1; c<=cpuCount;c++){
		std::vector<int> x; 
		Bursts_per_cpu[c]=x;
	}
	//==========================================

	// Keep running until all CPU-bound processes have terminated
	while (run) {
		bool addedToBlock = false;
		// bool contextSwitched = false;

		// Move time forward
		currentTime++;


		// If a process is done with its CPU burst, then place it in the blocked list (or terminate it),
		// and then assign the CPU a new process.
		for (int c = 1; c <= cpuCount; c++) {
			CPU[c].tick();
			ProcessState state = CPU[c].getState();

			// If it's bursting, then let it do its thing
			if (state == Burst || state == CpuIdle)
				continue;

			// If it's blocked now, then add it to the block list and make the current CPU core idle
			else if (state == Blocked) {
				burst_complete(currentTime, CPU[c]);

				//==============================================
					// Store stats 

					int turnaround_time=CPU[c].getBurstTime()+ CPU[c].getWaitTime();
					ts.push_back(turnaround_time);
					int wait_time=CPU[c].getWaitTime();
					ws.push_back(CPU[c].getWaitTime());
					if (turnaround_time<min_turn){
						min_turn=turnaround_time; 
					}else if (turnaround_time>max_turn){
						max_turn=turnaround_time;
					}

					if (wait_time<min_wait){
						min_wait=wait_time;
					}else if (wait_time>max_wait){
						max_wait=wait_time;
					}

					Bursts_per_cpu[c].push_back(CPU[c].getBurstTime());
					
					int id=CPU[c].getID();
					int bt=CPU[c].getBurstTime();
				
					
					Waits[id].push_back(wait_time);
					Turns[id].push_back(turnaround_time);
					Bursts_per_process[id].push_back(bt);

					//===============================================




				CPU[c].resetWaitTime();
				blockedList.push_back(CPU[c]);
				addedToBlock = true;
				CPU[c].setIdle();
			}

			// If it's terminated, then print the termination message and check if all CPU-bound
			// processes have been terminated.
			else if (state == Terminated) {
				burst_complete(currentTime, CPU[c]);
				
				//=======================================
				//store stats 
				int id=CPU[c].getID();
				// int b_time=CPU[c].getBurstTime();
				
					
				int wait_time=CPU[c].getWaitTime();
				
				ws.push_back(CPU[c].getWaitTime());
				int turnaround_time=CPU[c].getBurstTime()+ CPU[c].getWaitTime();
				
				ts.push_back(turnaround_time);
				Waits[id].push_back(wait_time);
				Turns[id].push_back(turnaround_time);
				if (turnaround_time<min_turn){
					min_turn=turnaround_time; 
				}else if (turnaround_time>max_turn){
					max_turn=turnaround_time;
				}

				if (wait_time<min_wait){
					min_wait=wait_time;
				}else if (wait_time>max_wait){
					max_wait=wait_time;
				}



				Bursts_per_cpu[c].push_back(CPU[c].getBurstTime());
				Bursts_per_process[id].push_back(CPU[c].getBurstTime());
				//=======================================

				int number=CPU[c].getID();

				termination1(currentTime, CPU[c], Turns[number], Waits[number]);



				CPU[c].setIdle();

				if (CPU[c].getType() == CpuBound) {
					numCPUProcessesTerminated++;
					if (numCPUProcessesTerminated == numCPUProcesses) {
						run = false;
						break;
					}
				}
			}
		}

		if (!run)
			break;


		// Update the wait times of processes in the READY queue
		int readyQueueSize = readyQueue.size();
		for (int c = 0; c < readyQueueSize; c++) {
			Process p = readyQueue.front();
			readyQueue.pop();
			p.tick();
			readyQueue.push(p);
		}


		// Update the processes in the blocked list. If they are no longer
		// blocked, then add them back into the READY queue.
		for (int c = blockedList.size() - 1; c >= 0; c--) {
			if ((addedToBlock && c < int(blockedList.size() - 1)) || !addedToBlock)
				blockedList[c].tick();

			if (blockedList[c].getState() == Ready) {
				entered_ready_queue(currentTime, blockedList[c]);
				readyQueue.push(blockedList[c]);
				blockedList.erase(blockedList.begin() + c);
			}
		}


		// If the time slice period has passed, then replace all the current CPU processes
		// with new processes from the ready queue.
		if (currentTime % rrTime == 0) {
			for (int c = 1; c <= cpuCount; c++) {

				// Preemption
				if (CPU[c].getState() != CpuIdle && CPU[c].getState() != Terminated) {
					CPU[c].preempt();
					readyQueue.push(CPU[c]);
				}

				// Either assign the CPU a new process, or make it idle for the next time slice
				if (!readyQueue.empty()) {
					context_switch(csTime, currentTime, CPU[c].getID(), readyQueue.front().getID());
					// contextSwitched = true;
					currentTime += csTime;

					CPU[c] = readyQueue.front();
					CPU[c].tick();					// Increment the wait time of the new CPU process one more time
					CPU[c].startBurst();
					readyQueue.pop();
					started_running(currentTime, CPU[c]);
				}
				else CPU[c].setIdle();
			}
		}
	}
	s.set_ws(ws);
	s.set_ts(ts);
	s.setTotalTime(currentTime);
	s.setMinTurn(min_turn);
	s.setMinWait(min_wait);
	s.setMaxTurn(max_turn);
	s.setMaxWait(max_wait);
	s.setTurns(Turns);
	s.setBurst_per_CPU(Bursts_per_cpu);
	s.setBurst_per_process(Bursts_per_process);
	analysis(s, cpuCount);
}


// Writes the header with the given title to the output stream 
void writeHeader(const char* title, int length = 72)
{
	if (title == NULL || strlen(title) == 0 || length <= 1)
		return;
	
	std::cout << std::string(length, '=') << std::endl;
	std::cout << std::string((length - strlen(title)) / 2, ' ') << title << std::endl;
	std::cout << std::string(length, '=') << std::endl;
}


// Generates a new set of processes and returns them
std::vector<Process> generateProcesses(int count)
{
	if (count <= 0)
		return std::vector<Process>();
	
	std::vector<Process> result;
	int numCPU = ceil(count * CPU_PROCESS_RATIO);
	
	for (int c = 1; c <= count; c++) {
		if (c <= count - numCPU)	result.push_back(Process(c, Interactive));
		else						result.push_back(Process(c, CpuBound));
	}
	return result;
}


// Prints the current list of processes
void printProcesses(const std::vector<Process>& processList)
{
	int size = processList.size();
	for (int c = 0; c < size; c++) {
		std::cout << "[time 0ms] " << processTypeToString(processList[c].getType()) << " process ID " << processList[c].getID() 
			<< " entered ready queue (requires " << processList[c].getBurstTime() << "ms CPU time)" << std::endl;
	}
}





// Main function, entrypoint of the application
int main(int argc, char *argv[])
{
	// Seed the random number generator with a random value
	srand(time(NULL));
	
	// Initialize stimulation settings to default values
	int numProcesses = PROCESS_COUNT;
	int numCPUs 	 = CPUCORE_COUNT;
	int csTime 		 = CS_DURATION;
	int rrTime		 = RR_TIMESLICE;
	
	// Process command line arguments
	for (int c = 1; c < argc; c++) {
		if (argv[c][0] == '-' && strlen(argv[c]) == 2 && c+1 < argc) {
			int value = atoi(argv[c+1]);
			if (value < 0)
				continue;
		
			switch (argv[c][1]) {
				case 'n': numProcesses = value;  break;
				case 'm': numCPUs 	   = value;  break;
				case 'c': csTime 	   = value;  break;
				case 't': rrTime 	   = value;  break;
			}
		}
	}
	
	// Generate the list of processes
	std::vector<Process> processList = generateProcesses(numProcesses);
	
	// Run FCFS algorithm
	writeHeader("First Come First Serve (FCFS)");
	printProcesses(processList);
	FCFS(processList, numCPUs, csTime);
	std::cout << std::endl << std::endl;
	
	// Run SJF algorithm
	writeHeader("Shortest Job First (SJF) -- Nonpreemptive");
	printProcesses(processList);
	SJF_NP(processList, numCPUs, csTime);
	std::cout << std::endl << std::endl;

	// Run SRT algorithm
	writeHeader("Shortest Response Time (SRT) -- Preemptive");
	printProcesses(processList);
	SRT_P(processList, numCPUs, csTime);
	std::cout << std::endl << std::endl;
	
	
	// Run RR algorithm
	writeHeader("Round Robin (RR) -- 80ms Timeslice");
	printProcesses(processList);
	RR(processList, numCPUs, csTime, rrTime);
	
	return 0;
}