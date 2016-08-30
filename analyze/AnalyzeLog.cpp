#include "AnalyzeLog.h"

/*
	不使用次构造方式
*/
AnalyzeLog::AnalyzeLog()
{
}
/*
	@param experiments, 一批实验的ID号
	@param ifileName, 一批txt文本的文件名中相同的部分，txt文本中包含需要分析的数据的
	@param ofileName, 分析结果保存的文本
*/
AnalyzeLog::AnalyzeLog(const int totalExperNum, const string & ifileName, const string & ofileName)
{
	m_totalExperNum = totalExperNum;
	m_inputBase = ifileName;
	m_outputBase = ofileName;
}

AnalyzeLog::~AnalyzeLog()
{
}

void AnalyzeLog::setInputFile(const string & ifileName)
{
	m_ifile.open(ifileName);

	if (!m_ifile)
		cout << "error with ifileName" << endl;
}

void AnalyzeLog::setOutputFile(const string & ofileName)
{
	m_ofile.open(ofileName);

	if (!m_ofile)
		cout << "error with ifileName" << endl;
}
/*
32    -2
64    -1
96    1
128   1
160   5
192   5
224   5
256   5
288   6
320   6
352   6
400   7
500   7

第一步：读入所有文件的数据
第二步：统计
*/

void AnalyzeLog::analyze()
{
	vector<vector<CounterValue>> inputData(m_totalExperNum);    // 把所有实验的txt数据输入到这个vector里
	vector<double> resultValue(m_totalExperNum);                // 记录每一次实验的最终收敛值
	int maxExperimentID = 0;                                    // 记录哪一组实验的进化的代数最大
	int maxEvolveNum = 0;                                       // 记录所有实验中的最大进化代数
	for (int i = 0; i < m_totalExperNum; ++i)
	{
		vector<CounterValue> oneExperiment;
		string fileName = m_inputBase + to_string(i) + ".txt";
		m_ifile.open(fileName);
		int counter, value;
		while (m_ifile >> counter >> value)
		{
			oneExperiment.emplace_back(counter, value);
		}
		if (counter > maxEvolveNum)
		{
			maxEvolveNum = counter;
			maxExperimentID = i;
		}
		resultValue[i] = value;               // 本次实验的最终收敛值
		inputData[i] = move(oneExperiment);   // 本次实验的所有日志记录
		m_ifile.close();
	}

	double variance = calcuVariance(resultValue);  // 计算标准差
	vector<CounterValue> avgInputData(inputData[maxExperimentID]);  // 以所有实验中进化次数最大的为初始化值
	for (int i = 0; i < m_totalExperNum; ++i)
	{
		if (i != maxExperimentID)                  // 进化次数最大的在已经初始化里面了，在这里就跳过去
		{
			int j = 0;
			int size = inputData[i].size();
			while (j < size)
			{
				avgInputData[j].m_value += inputData[i][j].m_value;
				++j;
			}
			while (j < inputData[maxExperimentID].size())
			{
				avgInputData[j].m_value += inputData[i][size - 1].m_value;
				++j;
			}
		}
	}
	for (CounterValue &cv : avgInputData)
	{
		cv.m_value /= m_totalExperNum;
	}
	/*for_each(avgInputData.begin(), avgInputData.end(), [this](CounterValue &cv)
	{
		cv.m_value /= this->m_totalExperNum;
	});*/
	m_ofile.open(m_outputBase + "avg.txt");
	for (const CounterValue &cv : avgInputData)
	{
		m_ofile << setw(8) << left << cv.m_counter << cv.m_value << endl;
	}
	m_ofile.close();
	m_ofile.open(m_outputBase + "variance.txt");
	m_ofile << variance << endl;
}

/*
	计算标准差  // todo：标准差还是方差？
	@param v, 一组数值
*/
double AnalyzeLog::calcuVariance(const vector<double>& v)
{
	double sum = accumulate(v.begin(), v.end(), 0.0);
	double avg = sum / v.size();
	double numerator = 0.0;
	for_each(v.begin(), v.end(), [&](const double x) 
	{
		numerator += (x - avg)*(x - avg);
	});
	return sqrt(numerator / (v.size() - 1));
}

double AnalyzeLog::string2Double(const string & doubleStr)
{
	double x = 0;
	istringstream cur(doubleStr);
	cur >> x;
	return x;
}


