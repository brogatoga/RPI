#ifndef __process_h_
#define __process_h_

#include <cstdlib>
#include <iomanip>
#include "constants.h"


// Enumeration of all process types
enum ProcessType
{
	Interactive,
	CpuBound
};


// Enumeration of all process states
enum ProcessState
{
	Ready,				// Waiting in the READY queue
	Burst,				// Currently executing its CPU burst
	Blocked,			// Is blocked due to I/O or user interaction (after completing its burst)
	Preempted,			// Has been paused while in the middle of bursting
	Terminated,			// Terminated and is no longer running
	CpuIdle,			// Custom flag for external use only
};


// Process class
class Process
{
public:
	Process();
	Process(int _id, ProcessType _type);
	Process(const Process& p) { this->copy(p); }
	Process& operator=(const Process& p) { this->copy(p); return *this; }
		
	ProcessType getType() const   { return type;      }
	ProcessState getState() const { return state;     }
	int getID() const             { return id;        }
	int getBurstTime() const      { return burstTime; }
	int getBlockTime() const      { return blockTime; }
	int getWaitTime() const       { return waitTimer; }
	int getRemainingBurst() const { return (state == Burst) ? burstTime - burstTimer : 0; }
	
	void startBurst();
	void resetWaitTime();
	void preempt();
	void setIdle();
	void tick();

private:
	ProcessType type;
	ProcessState state;
	int id; 
	
	int burstTime; 
	int blockTime; 
	int maxBurstCount;
	
	int burstTimer;
	int blockTimer;
	int waitTimer;
	int burstCounter;
	
	void copy(const Process& p);
	int randInt(int low, int high);
};

#endif 