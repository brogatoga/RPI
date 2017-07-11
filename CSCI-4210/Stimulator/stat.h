#ifndef __stat_h_
#define __stat_h_

#include <vector>
#include <map>
#include <iomanip>

class stat{
public:
	stat();
	
	float avg_turn(); 
	
	float avg_wait();
	
	float avg_cpu_util(); 
	void avg_cpu_util_per_process();

	void set_ts(std::vector<int> t){ts=t;}
	void set_ws(std::vector<int> w){ws=w;}


	void setBurst_per_CPU(std::map<int, std::vector<int> > cpu){bursts_per_cpu=cpu;}
	void setBurst_per_process(std::map<int, std::vector<int> > p){bursts_per_process=p;}
	void setTotalTime(int t) {total_time=t;}
	int getMinTurn(){return min_turn;}
	int getMinWait(){return min_wait;}
	int getMaxTurn(){return max_turn;}
	int getMaxWait(){return max_wait;}

	void setMinTurn(int m){min_turn=m;}
	void setMaxTurn(int m){max_turn=m;}
	void setMinWait(int m){min_wait=m;}
	void setMaxWait(int m){max_wait=m;}
	void setTurns(std::map<int, std::vector<int> >t) {tr_per_process=t;}

private:
	int total_time;
	
	std::vector<int> ts;
	std::vector<int> ws;
	std::map<int, std::vector<int> > bursts_per_cpu;
	std::map<int, std::vector<int> > bursts_per_process;
	std::map<int, std::vector<int> > tr_per_process;

	int min_wait;
	int max_wait;
	int min_turn;
	int max_turn;

};

#endif 