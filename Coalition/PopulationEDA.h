#pragma once
#include <iostream>
#include <vector>
#include "Coalition.h"
#include "../Analyze/AnalyzeLog.h"
using namespace std;

class PopulationEDA
{
public:
	PopulationEDA();
	void initialize(double selectRatio, int populationSize);

	void update();                                    // update the population
	
	void writeLogMatrix(int updateCounter);           // 打印矩阵到log
	int  writeLoganalyse(int updateCounter);		  // 打印算法的分析

	void setResetMe(const bool &resetMe);
	void setResetEnemy(const bool &resetEnemy);
	void setUpdate(const bool &update);
	
	bool getResetMe();
	bool getResetEnemy();
	bool getUpdate();
	bool getStop();
	int  getSize();
	void getBestCoalitions(vector<Coalition> &bC);
	Coalition & getEnemy();
	bool isZero(double d);
	
	void resetEnemy(string &way);
	void resetMe();
	void resetExperVariable();
	static uniform_real_distribution<double> urd_0_1;

private:
	void select();
	void estimateDistribution();
	void sample();
	void updateBestCoalitions();

private:
	vector<Coalition> m_population;        // a group of coalitions
	vector<Coalition> m_selectedPop;       // selected coalitions
	vector<int> m_bestCoalitionIndex;      // the index of best coalitions in the population, there could be more than one
	int m_populationSize;
	int m_bestEvaluation;                  // 记录当前种群最佳评估值
	Coalition m_enemy;
	
	int  m_updateCounter;
	bool m_stop;
	bool m_appearTarget;
	int  m_experimentTimes;

	double SELECT_RATIO;       // 从上一代种群里面，选择百分之多少，用来模拟概率分布 
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
	bool m_resetEnemy;		   // 是否重置地方坦克阵型
	bool m_update;			   // population是否继续更新
};