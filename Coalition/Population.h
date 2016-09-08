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
	
	void writeLogMatrix(int updateCounter);     // ��ӡ����log
	int  writeLogAnalyse(int updateCounter);    // ��ӡ�㷨�ķ���

	void setResetMe(const bool &resetMe);
	void setResetEnemy(const bool &resetEnemy);
	void setUpdate(const bool &update);

	bool getResetMe();                          // get �Ƿ�����Me
	bool getResetEnemy();                       // get �Ƿ�����Enemy
	bool getUpdate();                           // get �Ƿ����update population
	bool getStop();                             // get m_stop
	int  getSize();                             // get the size of m_populations
	void updateBestCoalitions(vector<Coalition> &bC);
	Coalition & getEnemy();
	bool isZero(double d);                      // TODO: put it in utility module

	void resetEnemy(string &way);
	void resetMe();
	void resetExperVariable();
	static uniform_real_distribution<double> urd_0_1;

private:
	void updatePopluation();                    // ������Ⱥ
	void updateBestCoalitions();                // update m_bestCoalitions
	void updateWeight();                        // �����ֵ --> ��Ӧֵ --> Ȩֵ
	void updatePMatrix();                       // update probability matrix

private:
	vector<Coalition> m_population;			    // a group of coalitions
	int m_populationSize;
	vector<int> m_bestCoalitionIndex;		    // the index of best coalitions in the population, there could be more than one
	int m_bestEvaluation;
	Coalition m_enemy;                          // enemy
	bool m_stop;							    // whether population meets terminal condition
	
	int  m_updateCounter;
	bool m_appearTarget;
	int  m_experimentTimes;

	double PL;            // Probability Learning
	double LS;			  // Local Search
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
	bool m_resetEnemy;         // �Ƿ����õط�̹������
	bool m_update;             // population�Ƿ��������

};
