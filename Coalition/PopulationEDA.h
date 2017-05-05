#pragma once
#include <iostream>
#include <vector>
#include "PopulationBase.h"
#include "Coalition.h"
#include "../Analyze/AnalyzeLog.h"
using namespace std;

class PopulationEDA : public PopulationBase
{
public:
	PopulationEDA();
	
	void initialize() override;          // 初始化我方阵型
	void update()     override;          // update the population
	void resetMe()    override;			 // 重置我方阵型

private:
	void selectIndividuals();
	void estimateDistribution();
	void sampleOneSolution(int index);
	void updateEvaluations();
	vector<int> generateRandomIndex();

private:
	vector<Coalition> m_selectedPop;     // selected coalitions
	uniform_int_distribution<int> uid_selectPop;

	double m_selectRatio;                 // 从上一代种群里面，选择百分之多少，用来模拟概率分布 
	double m_bRatio;		            
	double m_e;                          // 可以理解为变异概率
	int    m_dimension;                     // 问题的维度，在这里等于一个individual里面坦克的数量
	int    m_n;                          // template采样法里面的切分段数
	int    m_selectNum;                  // 等于 m_populationSize * m_selectRatio
};