#pragma once
#include <iostream>
#include <vector>
#include "PopulationBase.h"
#include "Coalition.h"
#include "../Analyze/AnalyzeLog.h"
using namespace std;

class PopulationMPL : public PopulationBase
{
public:
	PopulationMPL();
	
	void initialize() override;        // 初始化我方阵型
	void update()     override;        // update the population
	void resetMe()    override;        // 重置我方阵型
	
private:
	void updatePopluation();           // 更新种群
	void updateWeight();               // 计算估值 --> 适应值 --> 权值
	void updatePMatrix();              // update probability matrix

private:
	double PL;                         // Probability Learning
	double LS;			               // Local Search
	
	double m_e;                        // 一个比较小的实数，用来填充
	double m_bRatio;                   
	double m_dimension;                // 维度，就是变量的个数，也就是个体的大小（一个 Coalition 的坦克个数）
};
