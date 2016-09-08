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
	
	void writeLogMatrix(int updateCounter);           // ��ӡ����log
	int  writeLoganalyse(int updateCounter);		  // ��ӡ�㷨�ķ���

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
	int m_bestEvaluation;                  // ��¼��ǰ��Ⱥ�������ֵ
	Coalition m_enemy;
	
	int  m_updateCounter;
	bool m_stop;
	bool m_appearTarget;
	int  m_experimentTimes;

	double SELECT_RATIO;       // ����һ����Ⱥ���棬ѡ��ٷ�֮���٣�����ģ����ʷֲ� 
	double SMALL_NUMBER;

	int MAX_UPDATE;
	int MAX_EXPERIMENT;
	
	ofstream LOG_PM;           // ���ʾ������־
	ofstream LOG_ANALYSE;	   // �㷨������־
	
	string LOG_PM_NAME;        // ���Ǹ�������־���ļ��� 
	string LOG_ANALYSE_INPUT;
	string LOG_ANALYSE_OUTPUT;
	string ENEMY_INPUT;

	bool m_resetMe;            // �Ƿ������ҷ�̹������
	bool m_resetEnemy;		   // �Ƿ����õط�̹������
	bool m_update;			   // population�Ƿ��������
};