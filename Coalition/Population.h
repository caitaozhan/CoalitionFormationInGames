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
	void run(int ID);                           // run one experiment, set default_random_engine's seed with ID
	void updatePopluation();                    // 更新种群
	void updateBestCoalitions();                // update m_bestCoalitions
	void updateWeight();                        // 计算估值 --> 适应值 --> 权值
	void updatePMatrix();                       // update probability matrix
	void resetEnemy(string &way);
	
	void writeLogMatrix(int updateCounter);     // 打印矩阵到log
	int  writeLogAnalyse(int updateCounter);    // 打印算法的分析

	void setResetMe(const bool &resetMe);
	void setResetEnemy(const bool &resetEnemy);
	void setUpdate(const bool &update);
	void setLogExperEvaluate(int ID);

	const string getLogExperEvaluate()const;
	const string getLogAnalyzeOutput()const;
	bool getResetMe();                          // get 是否重置Me
	bool getResetEnemy();                       // get 是否重置Enemy
	bool getUpdate();                           // get 是否继续update population
	bool getStop();                             // get m_stop
	int  getSize();                             // get the size of m_populations
	void getBestCoalitions(vector<Coalition> & bC);
	void getEnemy(Coalition &e);
	bool isZero(double d);                      // TODO: put it in utility module
	double getPopAverageEvaluate();             // 
	void resetMe();                             // 重置我方坦克种群
	
	static string LOG_EXPER_EVALUATE;
	static string LOG_ANALYSE_OUTPUT;

private:
	vector<vector<double>> PROBABILITY_MATRIX;  // 概率矩阵
	vector<Coalition> m_population;			    // a group of coalitions，论文中的 archive
	int m_populationSize;
	vector<int> m_bestCoalitionIndex;		    // the index of best coalitions in the population, there could be more than one
	int m_bestEvaluation;
	Coalition m_enemy;                          // enemy
	bool m_stop;							    // whether population meets terminal condition
	int  m_updateCounter;
	int  m_evaluateCounter;
	int  SAMPLE_INTERVAL;                       // 采样间隔

	bool m_appearTarget;
	int  m_experimentTimes;

	double PL;            // Probability Learning
	double LS;			  // Local Search
	double SMALL_NUMBER;

	int MAX_UPDATE;       // todo: 这个成员需要去掉
	int MAX_EXPERIMENT;
	
	ofstream LOG_PM;           // 概率矩阵的日志
	ofstream LOG_ANALYSE;	   // 算法分析日志

	string LOG_PM_NAME;        // 这是个都是日志的文件名
	string ENEMY_INPUT;

	bool m_resetMe;            // 是否重置我方坦克阵型
	bool m_resetEnemy;         // 是否重置地方坦克阵型
	bool m_update;             // population是否继续更新

	uniform_real_distribution<double> urd_0_1;
	void resetExperVariables();

	bool m_isStagnate;         // 是否陷入停滞
	int  m_stagnateCriteria;   // 停滞的标准，即多次评价过后，种群的平均fitness依然没有提升
	int  m_latestPopAvg;       // 记录上一次的种群平均评估值
};
