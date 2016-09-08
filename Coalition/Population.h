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
	void initialize(double pl, double ls, int populationSize);

	void update();                              // update the population
	
	void writeLogMatrix(int updateCounter);     // 打印矩阵到log
	int  writeLogAnalyse(int updateCounter);    // 打印算法的分析

	void setResetMe(const bool &resetMe);
	void setResetEnemy(const bool &resetEnemy);
	void setUpdate(const bool &update);

	bool getResetMe();                          // get 是否重置Me
	bool getResetEnemy();                       // get 是否重置Enemy
	bool getUpdate();                           // get 是否继续update population
	bool getStop();                             // get m_stop
	int  getSize();                             // get the size of m_populations
	void updateBestCoalitions(vector<Coalition> &bC);
	Coalition & getEnemy();
	bool isZero(double d);                      // TODO: put it in utility module

	void resetEnemy(string &way);
	void resetMe();
	void resetExperVariable();
	static uniform_real_distribution<double> urd_0_1;

private:
	void updatePopluation();                    // 更新种群
	void updateBestCoalitions();                // update m_bestCoalitions
	void updateWeight();                        // 计算估值 --> 适应值 --> 权值
	void updatePMatrix();                       // update probability matrix

private:
	vector<Coalition> m_population;			    // a group of coalitions
	int m_populationSize;
	vector<int> m_bestCoalitionIndex;		    // the index of best coalitions in the population, there could be more than one
	int m_bestEvaluation;
	Coalition m_enemy;                          // enemy
	bool m_stop;							    // whether population meets terminal condition
	
	int  m_updateCounter;
	bool m_appearTarget;
	int  m_experimentTimes;

	double PL;            // Probability Learning
	double LS;			  // Local Search
	double SMALL_NUMBER;

	int MAX_UPDATE;
	int MAX_EXPERIMENT;
	
	ofstream LOG_PM;           // 概率矩阵的日志
	ofstream LOG_ANALYSE;	   // 算法分析日志

	string LOG_PM_NAME;        // 这是个都是日志的文件名
	string LOG_ANALYSE_INPUT;
	string LOG_ANALYSE_OUTPUT;
	string ENEMY_INPUT;

	bool m_resetMe;            // 是否重置我方坦克阵型
	bool m_resetEnemy;         // 是否重置地方坦克阵型
	bool m_update;             // population是否继续更新

};
