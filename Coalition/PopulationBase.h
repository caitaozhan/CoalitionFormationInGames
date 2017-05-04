#pragma once
#include <iostream>
#include <vector>
#include <ctime>
#include <chrono>
#include "Coalition.h"
#include "../Analyze/AnalyzeLog.h"
using namespace std;

// This is an abstract class, which is not a completely defined class and can not create objects
class PopulationBase
{
public:
	PopulationBase();

	virtual void initialize() = 0;  // pure virtual function
	virtual void update()     = 0;  // pure virtual function
	virtual void resetMe()    = 0;  // pure virtual function

	void writeLogMatrix(int updateCounter);   // ��ӡ����log
	int  writeLogAnalyse(int updateCounter);  // ��ӡ�㷨�ķ���

	void setResetMe(const bool & resetMe);
	void setResetEnemy(const bool & resetEnemy);
	void setUpdate(const bool & update);

	bool getResetMe() const;
	bool getResetEnemy() const;
	bool getUpdate() const;
	bool getStop() const;            // ��ʱû�� setStop() ����
	bool getSize() const;
	const Coalition & getEnemy() const;

	void resetEnemy(const string & way);
	void resetExperVariable();

protected:
	string getTimeNow();
	bool isZero(double d);
	void updateBestCoalitions(vector<Coalition> & bC);
	void updateBestCoalitions();

protected:
	vector<Coalition> m_population;                 // һ����Ⱥ, a group of Coalitions (����, �ֳ�solution)
	vector<vector<double>> PROBABILITY_MATRIX;      // ���ʾ���
	vector<double> SUM_OF_ROW;                      // ���ʾ�������Ԫ��֮�ͣ���PM��metadata
	vector<int> m_bestCoalitionIndex;               // The index of best coalitions in the population, there could be more than one 
	Coalition m_enemy;                              // �о�����

	ofstream LOG_PM;                                // ���ʾ������־
	ofstream LOG_ANALYSE;                           // �㷨��������־

	static uniform_real_distribution<double> urd_0_1;

	// ���캯�������ʼ��
	double m_total;                                 // ���ʾ�������Ԫ��֮�ͣ���PM��metadata
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
	int m_populationSize;                           // ��Ⱥ��ģ�Ĵ�С����population���ж��ٸ�individual
	int m_bestEvaluation;                           // ��ǰ��Ⱥ�������ã�����Ӧֵ

};
