#pragma once
#include <iostream>
#include <vector>
#include "Coalition.h"
#include "../Analyze/AnalyzeLog.h"
using namespace std;

class Population
{
public:
	Population();
	Population(size_t size);
	void initialize(double pl, double ls, int populationSize, int individualSize);

	void update();                              // update the population
	void updatePopluation();                    // 更新种群
	void updateBestCoalitions();                // update m_bestCoalitions
	void updateWeight();                        // 计算估值 --> 适应值 --> 权值
	void updatePMatrix();                       // update probability matrix
	void resetEnemy(string &way);
	
	void writeLogMatrix(int updateCounter);     // 打印矩阵到log
	int  writeLogAnalyse(int updateCounter);    // 打印算法的分析

	bool getStop();                             // get m_stop
	int  getSize();                             // get the size of m_populations
	void getBestCoalitions(vector<Coalition> & bC);
	void getEnemy(Coalition &e);
	bool isZero(double d);                      // TODO: put it in utility module

	void resetMe();

	//used to be global variables//
	
	ofstream LOG_PM;           // 概率矩阵的日志
	ofstream LOG_ANALYSE;	   // 算法分析日志
	//        ...              //
private:
	vector<Coalition> m_population;			    // a group of coalitions
	int m_populationSize;
	vector<int> m_bestCoalitionIndex;		    // the index of best coalitions in the population, there could be more than one
	Coalition m_enemy;                          // enemy
	bool m_stop;							    // whether stop updating the population
	int  m_updateCounter;
	
	bool m_update;
	bool m_appearTarget;
	int  m_experimentTimes;

	double PL;            // Probability Learning
	double LS;			  // Local Search
	double SMALL_NUMBER;

	int MAX_UPDATE;
	int MAX_EXPERIMENT;
};
