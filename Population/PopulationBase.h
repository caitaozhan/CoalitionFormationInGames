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

	virtual void initialize() = 0;  // pure virtual function TODO: ���Ӳ���string filename����������������Ҫ�ĳ�ʼ������
	virtual void update()     = 0;  // pure virtual function
	virtual void resetMe()    = 0;  // pure virtual function

	void setResetMe(const bool & resetMe);
	void setResetEnemy(const bool & resetEnemy);
	void setUpdate(const bool & update);

	const Coalition & getEnemy() const;
	int  getSize() const;
	bool getStop() const;           // ��ʱû�� setStop() ����
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
	void writeLogMatrix(int updateCounter);         // ��ӡ����log
	int  writeLogPopAverage(int updateCounter);     // ��ӡ�㷨�ķ���
	void population2transaction(const vector<Coalition> & population, vector<vector<ItemSet>> & transaction);
	void printTransaction(vector<vector<ItemSet>> & transactions, int transactionID);
	void takeActionToKnowledge(const map<pair<ItemSet, ItemSet>, double> & associateRules);
	pair<ItemSet, ItemSet> matchRules(const Coalition & coalition, const map<pair<ItemSet, ItemSet>, double> & associateRules);
	ItemSet findDestination(const Coalition & coalition, const pair<ItemSet, ItemSet> & matchedRule);
	ItemSet findSource(size_t moveSize, const Coalition & c, const pair<ItemSet, ItemSet> & matchedRule, const map<pair<ItemSet, ItemSet>, double> & associateRules);

protected:
	vector<Coalition>      m_population;            // һ����Ⱥ, a group of Coalitions (����, �ֳ�solution)
	vector<vector<double>> m_probabilityMatrix;     // ���ʾ���
	vector<double>         m_sumOfRow;              // ���ʾ�������Ԫ��֮�ͣ���PM��metadata
	vector<int> m_bestCoalitionIndex;               // The index of best coalitions in the population, there could be more than one 
	Coalition   m_enemy;                            // �о�����

	ofstream m_logPM;                               // ���ʾ������־
	ofstream m_logAnalyse;                          // �㷨��������־
	ofstream m_logTransaction;                      // population��transaction��־

	static uniform_real_distribution<double> urd_0_1;

	// ���캯�������ʼ��
	double m_PMtotal;                               // ���ʾ�������Ԫ��֮�ͣ���PM��metadata
	int m_maxUpdate;                                // ����������
	int m_maxExperimentTimes;                       // ���ʵ���ظ����д���    
	int m_experimentTimes;                          // ��ǰʵ�����������
	int m_updateCounter;                            // ��������������
	bool m_update;                                  // population�Ƿ��������
	bool m_resetMe;                                 // �Ƿ������ҷ�̹������
	bool m_resetEnemy;                              // �Ƿ����õз�̹������
	bool m_appearTarget;                            // �Ƿ����Ŀ��
	bool m_stop;                                    // �Ƿ�ֹͣ����

	// initialize���������ʼ��
	string m_fileNameEnemyInput;                    // �о����������ļ�������
	string m_logNamePM;                             // ���ʾ�����־������
	string m_logNameRunningResult;                  // �������н����־��ͬʱҲ����־����������
	string m_logNameAnalyseResult;                  // ��־��������ļ�������
	string m_logNameTransaction;                    // һ��transaction����һ��population������
	int m_populationSize;                           // ��Ⱥ��ģ�Ĵ�С����population���ж��ٸ�individual
	int m_bestEvaluation;                           // ��ǰ��Ⱥ�������ã�����Ӧֵ

	Apriori apriori;

};
