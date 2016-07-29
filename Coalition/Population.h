#pragma once
#include <iostream>
#include <vector>
#include "Coalition.h"
using namespace std;

class Population
{
public:
	Population();
	Population(size_t size);
	void initialize();

	void update();                              // update the population
	//vector<
	bool getStop();                             // get m_stop
	int  getSize();
	vector<Coalition> && getBestCoalitions();


private:
	vector<Coalition> m_population;			    // a group of coalitions
	vector<Coalition> m_bestCoalitions;		    // best in the population, there could be more than one
	Coalition m_enemy;                          // enemy
	bool m_stop;							    // whether stop updating the population
};
