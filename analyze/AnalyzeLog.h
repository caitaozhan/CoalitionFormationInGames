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

// 曾经 AnalyzeLog 是一个独立的project
// 建了filter(筛选器), 独立的project合并到一个大project里面，变成了大project里面的一个模块
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
	int      m_totalExperNum;  // 一批实验的ID
	string   m_inputBase;      // 一批txt文件作为输入文本，此成员是相同的部分
	string   m_outputBase;     // 输出txt文件的名字，的相同的部分
	ifstream m_ifile;
	ofstream m_ofile;
	vector<CounterValue> m_results;
};