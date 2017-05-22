#pragma once
#include <iostream>
#include <vector>
#include <ctime>
#include <chrono>
#include "Coalition.h"
#include "../Apriori/Apriori.h"
#include "../Analyze/AnalyzeLog.h"
using namespace std;

/* 
    This is an abstract class, which is not a completely defined class and can not implement objects
*/
class PopulationBase
{
public:
	PopulationBase();

	virtual void initialize() = 0;  // pure virtual function TODO: 增加参数string filename，从这里读入程序需要的初始化参数
	virtual void update()     = 0;  // pure virtual function
	virtual void resetMe()    = 0;  // pure virtual function

	void setResetMe(const bool & resetMe);
	void setResetEnemy(const bool & resetEnemy);
	void setUpdate(const bool & update);

	const Coalition & getEnemy() const;
	int  getSize() const;
	bool getStop() const;           // 暂时没有 setStop() 函数
	bool getUpdate() const;
	bool getResetEnemy() const;
	bool getResetMe() const;

	void resetEnemy(const string & way);
	void resetExperVariable();
	void updateBestCoalitions(vector<Coalition> & bC);

protected:
	string getTimeNow();
	bool isZero(double d);
	void updateBestCoalitions();
	void writeLogMatrix(int updateCounter);         // 打印矩阵到log
	int  writeLogPopAverage(int updateCounter);     // 打印算法的分析
	void population2transaction(const vector<Coalition> & population, vector<vector<ItemSet>> & transaction);
	void printTransaction(vector<vector<ItemSet>> & transactions, int transactionID);
	void takeActionToKnowledge(const map<pair<ItemSet, ItemSet>, double> & associateRules);
	pair<ItemSet, ItemSet> matchRules(const Coalition & coalition, const map<pair<ItemSet, ItemSet>, double> & associateRules);
	ItemSet findDestination(const Coalition & coalition, const pair<ItemSet, ItemSet> & matchedRule);
	ItemSet findSource(size_t moveSize, const Coalition & c, const pair<ItemSet, ItemSet> & matchedRule, const map<pair<ItemSet, ItemSet>, double> & associateRules);

protected:
	vector<Coalition>      m_population;            // 一个种群, a group of Coalitions (个体, 又称solution)
	vector<vector<double>> m_probabilityMatrix;     // 概率矩阵
	vector<double>         m_sumOfRow;              // 概率矩阵所有元素之和，是PM的metadata
	vector<int> m_bestCoalitionIndex;               // The index of best coalitions in the population, there could be more than one 
	Coalition   m_enemy;                            // 敌军个体

	ofstream m_logPM;                               // 概率矩阵的日志
	ofstream m_logAnalyse;                          // 算法分析的日志
	ofstream m_logTransaction;                      // population的transaction日志

	static uniform_real_distribution<double> urd_0_1;

	// 构造函数里面初始化
	double m_PMtotal;                               // 概率矩阵所有元素之和，是PM的metadata
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
	string m_logNameTransaction;                    // 一个transaction就是一个population的数据
	int m_populationSize;                           // 种群规模的大小，即population里有多少个individual
	int m_bestEvaluation;                           // 当前种群里面的最好（大）适应值

	Apriori apriori;

};
