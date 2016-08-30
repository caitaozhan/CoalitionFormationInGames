#include "AnalyzeLog.h"

/*
	��ʹ�ôι��췽ʽ
*/
AnalyzeLog::AnalyzeLog()
{
}
/*
	@param experiments, һ��ʵ���ID��
	@param ifileName, һ��txt�ı����ļ�������ͬ�Ĳ��֣�txt�ı��а�����Ҫ���������ݵ�
	@param ofileName, �������������ı�
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

��һ�������������ļ�������
�ڶ�����ͳ��
*/

void AnalyzeLog::analyze()
{
	vector<vector<CounterValue>> inputData(m_totalExperNum);    // ������ʵ���txt�������뵽���vector��
	vector<double> resultValue(m_totalExperNum);                // ��¼ÿһ��ʵ�����������ֵ
	int maxExperimentID = 0;                                    // ��¼��һ��ʵ��Ľ����Ĵ������
	int maxEvolveNum = 0;                                       // ��¼����ʵ���е�����������
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
		resultValue[i] = value;               // ����ʵ�����������ֵ
		inputData[i] = move(oneExperiment);   // ����ʵ���������־��¼
		m_ifile.close();
	}

	double variance = calcuVariance(resultValue);  // �����׼��
	vector<CounterValue> avgInputData(inputData[maxExperimentID]);  // ������ʵ���н�����������Ϊ��ʼ��ֵ
	for (int i = 0; i < m_totalExperNum; ++i)
	{
		if (i != maxExperimentID)                  // ���������������Ѿ���ʼ�������ˣ������������ȥ
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
	�����׼��  // todo����׼��Ƿ��
	@param v, һ����ֵ
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


