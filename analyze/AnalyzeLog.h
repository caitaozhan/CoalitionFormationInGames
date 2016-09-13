#pragma once
#include <iostream>
#include <sstream>
#include <fstream>
#include <string>
#include <vector>
#include <numeric>
#include <algorithm>
#include <iomanip>
#include "CounterValue.h"
using namespace std;

// ���� AnalyzeLog ��һ��������project
// ����filter(ɸѡ��), ������project�ϲ���һ����project���棬����˴�project�����һ��ģ��
class AnalyzeLog
{
public:
	AnalyzeLog();
	AnalyzeLog(const int totalExperiments, const string &ifileName, const string &ofileName);
	~AnalyzeLog();

	void setInputFile(const string &ifileName);
	void setOutputFile(const string &ofileName);
	//void setPattern(const string &pattern);
	
	void analyze();
	double calcuVariance(const vector<double> &v);
	double string2Double(const string &doubleStr);

private:
	int      m_totalExperNum;  // һ��ʵ���ID
	string   m_inputBase;      // һ��txt�ļ���Ϊ�����ı����˳�Ա����ͬ�Ĳ���
	string   m_outputBase;     // ���txt�ļ������֣�����ͬ�Ĳ���
	ifstream m_ifile;
	ofstream m_ofile;
	vector<CounterValue> m_results;
};