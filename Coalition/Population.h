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
	void updatePopluation();                    // ������Ⱥ
	void updateBestCoalitions();                // update m_bestCoalitions
	void updateWeight();                        // �����ֵ --> ��Ӧֵ --> Ȩֵ
	void updatePMatrix();                       // update probability matrix
	void resetEnemy(string &way);
	
	void writeLogMatrix(int updateCounter);     // ��ӡ����log
	int  writeLogAnalyse(int updateCounter);    // ��ӡ�㷨�ķ���

	void setResetMe(const bool &resetMe);
	void setResetEnemy(const bool &resetEnemy);
	void setUpdate(const bool &update);
	void setLogExperEvaluate(int ID);

	const string getLogExperEvaluate()const;
	const string getLogAnalyzeOutput()const;
	bool getResetMe();                          // get �Ƿ�����Me
	bool getResetEnemy();                       // get �Ƿ�����Enemy
	bool getUpdate();                           // get �Ƿ����update population
	bool getStop();                             // get m_stop
	int  getSize();                             // get the size of m_populations
	void getBestCoalitions(vector<Coalition> & bC);
	void getEnemy(Coalition &e);
	bool isZero(double d);                      // TODO: put it in utility module
	double getPopAverageEvaluate();             // 
	void resetMe();                             // �����ҷ�̹����Ⱥ
	
	static string LOG_EXPER_EVALUATE;
	static string LOG_ANALYSE_OUTPUT;

private:
	vector<vector<double>> PROBABILITY_MATRIX;  // ���ʾ���
	vector<Coalition> m_population;			    // a group of coalitions�������е� archive
	int m_populationSize;
	vector<int> m_bestCoalitionIndex;		    // the index of best coalitions in the population, there could be more than one
	int m_bestEvaluation;
	Coalition m_enemy;                          // enemy
	bool m_stop;							    // whether population meets terminal condition
	int  m_updateCounter;
	int  m_evaluateCounter;
	int  SAMPLE_INTERVAL;                       // �������

	bool m_appearTarget;
	int  m_experimentTimes;

	double PL;            // Probability Learning
	double LS;			  // Local Search
	double SMALL_NUMBER;

	int MAX_UPDATE;       // todo: �����Ա��Ҫȥ��
	int MAX_EXPERIMENT;
	
	ofstream LOG_PM;           // ���ʾ������־
	ofstream LOG_ANALYSE;	   // �㷨������־

	string LOG_PM_NAME;        // ���Ǹ�������־���ļ���
	string ENEMY_INPUT;

	bool m_resetMe;            // �Ƿ������ҷ�̹������
	bool m_resetEnemy;         // �Ƿ����õط�̹������
	bool m_update;             // population�Ƿ��������

	uniform_real_distribution<double> urd_0_1;
	void resetExperVariables();

	bool m_isStagnate;         // �Ƿ�����ͣ��
	int  m_stagnateCriteria;   // ͣ�͵ı�׼����������۹�����Ⱥ��ƽ��fitness��Ȼû������
	int  m_latestPopAvg;       // ��¼��һ�ε���Ⱥƽ������ֵ
};
