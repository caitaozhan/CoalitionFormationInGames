#include "AnalyzeLog.h"
/*
target not found @500
500: 2.91667

Best @246  6
500: 2.95833
*/

AnalyzeLog::AnalyzeLog()
{
	m_patternNotFound = "target not found.*";     // û���ҵ�
	m_patternBest = "@([\\d]+)";				  // ����
	m_patternAvg = ": ([0-9]+\\.[0-9]+|[0-9]+)";  // ��ƥ��ʵ�������û����ƥ��������������̰�ĵ�˳�����⣩

	string ifileName("../log/32^2,pop=32,ind=32_param/log_analyze.txt");
	string ofileName("../log/32^2,pop=32,ind=32_param/6-25_0.9_0.8-.txt");

	m_ifile.open(ifileName);
	m_ofile.open(ofileName);

	if (!m_ifile)
		cout << "error with ifileName" << endl;

	if (!m_ofile)
		cout << "error with ofileName" << endl;
}

AnalyzeLog::AnalyzeLog(const string & ifileName, const string & ofileName)
{
	m_patternNotFound = "target not found.*";     // û���ҵ�
	m_patternBest = "@([\\d]+)";                  // ����
	m_patternAvg = ": ([0-9]+\\.[0-9]+|[0-9]+)";  // ��ƥ��ʵ�������û����ƥ��������������̰�ĵ�˳�����⣩

	m_ifile.open(ifileName);
	m_ofile.open(ofileName);
	
	if (!m_ifile)
		cout << "error with ifileName" << endl;

	if (!m_ofile)
		cout << "error with ofileName" << endl;
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

void AnalyzeLog::setPattern(const string & pattern)
{
	
}

/*
target not found @500
500: 2.91667

Best @246  6
500: 2.95833
*/
void AnalyzeLog::analyze()
{
	smatch sMatchNotFound;
	smatch sMatchBest;
	smatch sMatchAvg;
	string line;
	double sumTarget = 0.0;
	double sumAvg  = 0.0;
	int counterNotFound = 0;
	int counterFound = 0;
	while (getline(m_ifile, line))
	{
		if (regex_search(line, sMatchNotFound, m_patternNotFound))  // û���ڹ涨�����ҵ����Ž�
		{
			counterNotFound++;
		}
		else                                                        // �ڹ涨�����ҵ����Ž�
		{
			regex_search(line, sMatchBest, m_patternBest);
			sumTarget += string2Double(sMatchBest[1]);
			counterFound++;
		}

		getline(m_ifile, line);
		regex_search(line, sMatchAvg, m_patternAvg);
		sumAvg += string2Double(sMatchAvg[1]);

		getline(m_ifile, line);
	}
	m_ofile << "times that target is found:   " << counterFound << endl;
	m_ofile << "searched the first target after evaluating individuals how many times: " << sumTarget / counterFound << endl;
	m_ofile << "average evaluation value of popluation at present is: " << sumAvg / (counterFound + counterNotFound) << endl;

	m_ofile.close();
}

double AnalyzeLog::string2Double(const string & doubleStr)
{
	double x = 0;
	istringstream cur(doubleStr);
	cur >> x;
	return x;
}


