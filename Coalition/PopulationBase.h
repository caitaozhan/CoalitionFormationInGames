#pragma once
#include <iostream>
#include <vector>
#include <ctime>
#include <chrono>
#include "Coalition.h"
#include "../Analyze/AnalyzeLog.h"
using namespace std;

// This is an abstract class, which is not a completely defined class and can not create objects
class PopulationBase
{
public:
	PopulationBase();

	virtual void initialize() = 0;  // pure virtual function
	virtual void update()     = 0;  // pure virtual function
	virtual void resetMe()    = 0;  // pure virtual function

	void writeLogMatrix(int updateCounter);   // 打印矩阵到log
	int  writeLogAnalyse(int updateCounter);  // 打印算法的分析

	void setResetMe(const bool & resetMe);
	void setResetEnemy(const bool & resetEnemy);
	void setUpdate(const bool & update);

	bool getResetMe() const;
	bool getResetEnemy() const;
	bool getUpdate() const;
	bool getStop() const;            // 暂时没有 setStop() 函数
	bool getSize() const;
	const Coalition & getEnemy() const;

	void resetEnemy(const string & way);
	void resetExperVariable();

protected:
	string getTimeNow();
	bool isZero(double d);
	void updateBestCoalitions(vector<Coalition> & bC);
	void updateBestCoalitions();

protected:
	vector<Coalition> m_population;                 // 一个种群, a group of Coalitions (个体, 又称solution)
	vector<vector<double>> PROBABILITY_MATRIX;      // 概率矩阵
	vector<double> SUM_OF_ROW;                      // 概率矩阵所有元素之和，是PM的metadata
	vector<int> m_bestCoalitionIndex;               // The index of best coalitions in the population, there could be more than one 
	Coalition m_enemy;                              // 敌军个体

	ofstream LOG_PM;                                // 概率矩阵的日志
	ofstream LOG_ANALYSE;                           // 算法分析的日志

	static uniform_real_distribution<double> urd_0_1;

	// 构造函数里面初始化
	double m_total;                                 // 概率矩阵所有元素之和，是PM的metadata
	int m_maxUpdate;                                // 最大进化代数
	int m_maxExperimentTimes;                       // 最大实验重复运行次数    
	int m_experimentTimes;                          // 当前实验次数计数器
	int m_updateCounter;                            // 进化次数计数器
	bool m_update;                                  // population是否持续更新
	bool m_resetMe;                                 // 是否重置我方坦克阵型
	bool m_resetEnemy;                              // 是否重置敌方坦克阵型
	bool m_appearTarget;                            // 是否出现目标
	bool m_stop;                                    // 是否停止进化

	// initialize函数里面初始化
	string m_fileNameEnemyInput;                    // 敌军阵型输入文件的名字
	string m_logNamePM;                             // 概率矩阵日志的名字
	string m_logNameRunningResult;                  // 程序运行结果日志，同时也是日志分析的输入
	string m_logNameAnalyseResult;                  // 日志分析输出文件的名字
	int m_populationSize;                           // 种群规模的大小，即population里有多少个individual
	int m_bestEvaluation;                           // 当前种群里面的最好（大）适应值

};
