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
	
	void initialize() override;        // ��ʼ���ҷ�����
	void update()     override;        // update the population
	void resetMe()    override;        // �����ҷ�����
	
private:
	void updatePopluation();           // ������Ⱥ
	void updateWeight();               // �����ֵ --> ��Ӧֵ --> Ȩֵ
	void updatePMatrix();              // update probability matrix

private:
	double PL;                         // Probability Learning
	double LS;			               // Local Search
	double m_smallNumber;              // һ���Ƚ�С��ʵ�����������

};
