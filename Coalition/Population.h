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
	bool getStop();                             // get m_stop
	int  getSize();                             // get the size of m_populations
	void updateBestCoalitions();                // update m_bestCoalitions
	void getBestCoalitions(vector<Coalition> & bC);

	bool isZero(double d);                      // TODO: put it in utility module

private:
	vector<Coalition> m_population;			    // a group of coalitions
	vector<int> m_bestCoalitionIndex;		    // the index of best coalitions in the population, there could be more than one
	Coalition m_enemy;                          // enemy
	bool m_stop;							    // whether stop updating the population
};
