#pragma once
#include "ofMain.h"
#include "ofApp.h"
#include "../Coalition/Population.h"
using namespace std;

/*
	@param experiments, store a vector of experiment IDs
	Todo: ��Ҫ����Population�ĸ����߼�
*/
void doExperiments(const vector<int> &experiments)
{
	Population population;
	population.initialize(0.1, 0.9, 500);
	for (int i = 0; i < experiments.size(); ++i)
	{
		population.setLogExperEvaluate(experiments[i]);  // ���ݱ���ʵ���ID�ţ�����log�ļ���
		population.run(experiments[i]);
	}
}

//========================================================================
int main( )
{
	int numThread = thread::hardware_concurrency();
	numThread = (numThread == 0 ? 2 : numThread);   // hardware_concurrency() �п��ܷ��� 0
	//numThread = 7;

	// �����ɴ�ʵ�飬��ΪnumThread���飬��Щ��ͬʱ����ʵ�顣
	vector<vector<int>> groups(numThread);       
	int totalExperiments = 30;
	int num = totalExperiments / numThread;
	int i;
	for (i = 0; i < totalExperiments; ++i)
	{
		int k = i / num;
		if (k == numThread)
			break;
		groups[k].emplace_back(i);
	}
	for (int j = 0; i < totalExperiments; ++i, ++j)
	{
		groups[j].emplace_back(i);
	}
	
	// �������֮�󣬿������߳�ͬʱ����ʵ�飬һ���̶߳�Ӧһ���顣
	vector<thread> threads(numThread);
	for (i = 0; i < numThread; ++i)
	{
		threads[i] = thread(doExperiments, ref(groups[i]));
	}
	for (auto &t : threads)
	{
		t.join();
	}

	// todo: ����
	AnalyzeLog analyzeLog(totalExperiments, Population::LOG_EXPER_EVALUATE, Population::LOG_ANALYSE_OUTPUT);
	analyzeLog.analyze();

	system("pause");
	return 0;
}
