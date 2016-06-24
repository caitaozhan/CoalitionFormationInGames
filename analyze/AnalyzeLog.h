#pragma once
#include <iostream>
#include <sstream>
#include <fstream>
#include <string>
#include <regex>
using namespace std;

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