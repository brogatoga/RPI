#ifndef __constants_h
#define __constants_h

#define PROCESS_COUNT  12
#define CPUCORE_COUNT  4
#define BURST_COUNT	   6
#define RR_TIMESLICE   80
#define CS_DURATION	   4

#define INT_PROCESS_RATIO  		0.8
#define INT_PROCESS_BURST_LOW	20
#define INT_PROCESS_BURST_HIGH  200
#define INT_PROCESS_WAIT_LOW	1000
#define INT_PROCESS_WAIT_HIGH   4500

#define CPU_PROCESS_RATIO 		0.2
#define CPU_PROCESS_BURST_LOW 	200
#define CPU_PROCESS_BURST_HIGH  3000
#define CPU_PROCESS_WAIT_LOW	1000
#define CPU_PROCESS_WAIT_HIGH	4500

#endif