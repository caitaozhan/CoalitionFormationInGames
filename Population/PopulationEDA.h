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
	
	void initialize() override;          // ��ʼ���ҷ�����
	void update()     override;          // update the population
	void resetMe()    override;			 // �����ҷ�����

private:
	void selectIndividuals();
	void estimateDistribution();
	void sampleOneSolution(int index);
	void updateEvaluations();
	vector<int> generateRandomIndex();

private:
	vector<Coalition> m_selectedPop;     // selected coalitions
	uniform_int_distribution<int> uid_selectPop;

	double m_selectRatio;                 // ����һ����Ⱥ���棬ѡ��ٷ�֮���٣�����ģ����ʷֲ� 
	double m_bRatio;		            
	double m_e;                          // �������Ϊ�������
	int    m_dimension;                     // �����ά�ȣ����������һ��individual����̹�˵�����
	int    m_n;                          // template������������зֶ���
	int    m_selectNum;                  // ���� m_populationSize * m_selectRatio
};