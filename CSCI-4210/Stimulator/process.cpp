#include "process.h"


// Default constructor
Process::Process()
{
	state = Ready;
	id = blockTime = burstTime = maxBurstCount = burstTimer = blockTimer = waitTimer = burstCounter = 0;
}


// Constructor to initialize a new Process instance
Process::Process(int _id, ProcessType _type) : state(Ready), burstTimer(0), blockTimer(0), waitTimer(0), burstCounter(0)
{
	id = _id;			// Must do this in order to avoid -Wreorder warnings
	type = _type;
	
	switch (type) {
		case Interactive:
			burstTime = randInt(INT_PROCESS_BURST_LOW, INT_PROCESS_BURST_HIGH);
			blockTime = randInt(INT_PROCESS_WAIT_LOW, INT_PROCESS_WAIT_HIGH);
			maxBurstCount = -1;
			break;
			
		case CpuBound:
			burstTime = randInt(CPU_PROCESS_BURST_LOW, CPU_PROCESS_BURST_HIGH);
			blockTime = randInt(CPU_PROCESS_WAIT_LOW, CPU_PROCESS_WAIT_HIGH);
			maxBurstCount = BURST_COUNT;
			break;
	}
}


// Deep copy function
void Process::copy(const Process& p)
{
	this->type = p.type;
	this->state = p.state;
	this->id = p.id;
	this->burstTime = p.burstTime;
	this->blockTime = p.blockTime;
	this->maxBurstCount = p.maxBurstCount;
	this->burstTimer = p.burstTimer;
	this->blockTimer = p.blockTimer;
	this->waitTimer = p.waitTimer;
	this->burstCounter = p.burstCounter;
}


// Generates a random integer between low and high, inclusive
int Process::randInt(int low, int high)
{	
	return (rand() % (high - low + 1)) + low;
}


// Start the CPU burst
void Process::startBurst()
{
	if (state == Terminated || state == Burst)
		return;
	
	if (state != Preempted)
		burstTimer = 0;
	state = Burst;
}


// Preempt this process
void Process::preempt()
{
	if (state == Burst)
		state = Preempted;
}


// Reset the wait timer back to zero
void Process::resetWaitTime()
{
	waitTimer = 0;
}


// Set this process to the CpuIdle state (** Note that this is NOT a normal state **)
void Process::setIdle()
{
	state = CpuIdle;
}


// This function should be called every time the clock ticks (1ms per tick)
void Process::tick()
{
	switch (state) {
		case Preempted:
		case Ready: {
			waitTimer++;
			break;
		}
		
		case Blocked: {
			blockTimer++;
			if (blockTimer == blockTime) {
				blockTimer = 0;
				state = Ready;
			}
			break;
		}
			
		case Burst: {
			burstTimer++;
			if (burstTimer == burstTime) {
				burstTimer = 0;
				burstCounter++;
				state = (burstCounter == maxBurstCount && maxBurstCount > 0) ? Terminated : Blocked;
			}
			break;
		}
			
		case Terminated: break;
		case CpuIdle:    break;
	}
}