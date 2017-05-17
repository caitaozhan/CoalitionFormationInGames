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
	
	double m_e;                        // һ���Ƚ�С��ʵ�����������
	double m_bRatio;                   
	double m_dimension;                // ά�ȣ����Ǳ����ĸ�����Ҳ���Ǹ���Ĵ�С��һ�� Coalition ��̹�˸�����
};
