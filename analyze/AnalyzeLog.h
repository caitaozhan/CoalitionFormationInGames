#pragma once
#include <iostream>
#include <sstream>
#include <fstream>
#include <string>
#include <regex>
using namespace std;

// 曾经 AnalyzeLog 是一个独立的project
// 建了filter(筛选器), 独立的project合并到一个大project里面，变成了大project里面的一个模块
class AnalyzeLog
{
public:
	AnalyzeLog();
	AnalyzeLog(const string &ifileName, const string &ofileName);
	~AnalyzeLog();

	void setInputFile(const string &ifileName);
	void setOutputFile(const string &ofileName);
	void setPattern(const string &pattern);
	
	void analyze();
	double string2Double(const string &doubleStr);

private:
	ifstream m_ifile;
	ofstream m_ofile;

	regex m_patternNotFound;
	regex m_patternBest;
	regex m_patternAvg;
};