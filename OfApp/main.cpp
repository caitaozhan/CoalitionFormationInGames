#pragma once
#include "ofMain.h"
#include "ofApp.h"
#include "../Coalition/Population.h"
using namespace std;

/*
	@param experiments, store a vector of experiment IDs
	Todo: 需要更改Population的更新逻辑
*/
void doExperiments(const vector<int> &experiments)
{
	Population population;
	population.initialize(0.1, 0.9, 500);
	for (int i = 0; i < experiments.size(); ++i)
	{
		population.setLogExperEvaluate(experiments[i]);  // 依据本次实验的ID号，设置log文件名
		population.run(experiments[i]);
	}
}

//========================================================================
int main( )
{
	int numThread = thread::hardware_concurrency();
	numThread = (numThread == 0 ? 2 : numThread);   // hardware_concurrency() 有可能返回 0
	//numThread = 7;

	// 把若干次实验，分为numThread个组，这些组同时进行实验。
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
	
	// 分组完毕之后，开启多线程同时进行实验，一个线程对应一个组。
	vector<thread> threads(numThread);
	for (i = 0; i < numThread; ++i)
	{
		threads[i] = thread(doExperiments, ref(groups[i]));
	}
	for (auto &t : threads)
	{
		t.join();
	}

	// todo: 调试
	AnalyzeLog analyzeLog(totalExperiments, Population::LOG_EXPER_EVALUATE, Population::LOG_ANALYSE_OUTPUT);
	analyzeLog.analyze();

	system("pause");
	return 0;
}
