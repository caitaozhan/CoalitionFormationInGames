#include "PopulationBase.h"

uniform_real_distribution<double> PopulationBase::urd_0_1 = uniform_real_distribution<double>(0.0, 1.0);

PopulationBase::PopulationBase()
{
	m_total              = 0;
	m_maxUpdate          = 100;
	m_maxExperimentTimes = 15;
	m_experimentTimes    = 0;
	m_updateCounter      = 0;

	m_update             = true;
	m_resetMe            = false;
	m_resetEnemy         = false;
	m_appearTarget       = false;
	m_stop               = false;
}

void PopulationBase::writeLogMatrix(int updateCounter)
{
	LOG_PM << '\n' << "update counter: " << updateCounter << '\n';
	for (int y = Global::HEIGHT - 1; y >= 0; --y)
	{
		for (int x = 0; x < Global::WIDTH - 1; ++x)
		{
			if (isZero(PROBABILITY_MATRIX[y][x]))
			{
				LOG_PM << setprecision(0);
			}
			else
			{
				LOG_PM << setprecision(3);
			}
			LOG_PM << left << setw(7) << PROBABILITY_MATRIX[y][x];  // (x,y) --> [y][x]
		}
		LOG_PM << '\n';
	}
	LOG_PM << '\n' << "*******************" << endl;
}

/*
    @param:  ���µĴ���
    @return: ��ʱ����population��ƽ����ֵ
*/
int PopulationBase::writeLogAnalyse(int updateCounter)
{
	double sum = 0.0;
	double avg = 0.0;
	for (const Coalition &c : m_population)
	{
		sum += c.getSimpleEvaluate();
	}
	avg = sum / m_population.size();
	LOG_ANALYSE << updateCounter << ": " << avg << "\n\n";
	return avg;
}

void PopulationBase::setResetMe(const bool & resetMe)
{
	m_resetMe = resetMe;
}

void PopulationBase::setResetEnemy(const bool & resetEnemy)
{
	m_resetEnemy = resetEnemy;
}

void PopulationBase::setUpdate(const bool & update)
{
	m_update = update;
}

bool PopulationBase::getResetMe() const
{
	return m_resetMe;
}

bool PopulationBase::getResetEnemy() const
{
	return m_resetEnemy;
}

bool PopulationBase::getUpdate() const
{
	return m_update;
}

bool PopulationBase::getStop() const
{
	return m_stop;
}

bool PopulationBase::getSize() const
{
	return m_population.size();
}

/*
    ����population�ĳ�Ա����m_bestCoalitionIndex(��������ø����vector�±�)������������˸���
    ֱ�Ӹ��Ĵ�����������
    @param bC, ��������������˸����vector
*/
void PopulationBase::updateBestCoalitions(vector<Coalition>& bC)
{
	bC.clear();
	bC.reserve(m_bestCoalitionIndex.size());
	for (int & index : m_bestCoalitionIndex)
	{
		bC.push_back(m_population[index]);
	}
}

/*
    get coalitions with the highest evaluations, note that there could be tied best ones
*/
void PopulationBase::updateBestCoalitions()
{
	m_bestCoalitionIndex.clear();
	int n = getSize();
	double bestEvaluation = -n;
	for (int i = 0; i < n; ++i)
	{
		if (m_population[i].getSimpleEvaluate() > bestEvaluation + Global::EPSILON)
		{
			m_bestCoalitionIndex.clear();
			bestEvaluation = m_population[i].getSimpleEvaluate();
			m_bestCoalitionIndex.emplace_back(i);
		}
		else if (isZero(m_population[i].getSimpleEvaluate() - bestEvaluation))
		{
			m_bestCoalitionIndex.emplace_back(i);
		}
	}
	int newBestEvaluation;
	if (m_population[m_bestCoalitionIndex[0]].getSimpleEvaluate() < 0)
		newBestEvaluation = m_population[m_bestCoalitionIndex[0]].getSimpleEvaluate() - Global::EPSILON;
	else
		newBestEvaluation = m_population[m_bestCoalitionIndex[0]].getSimpleEvaluate() + Global::EPSILON;

	if (newBestEvaluation > m_bestEvaluation)  // �������ֵ�����
	{
		m_bestEvaluation = newBestEvaluation;
		cout << "Best @" << m_updateCounter << "  " << m_bestEvaluation << '\n';
		if (m_appearTarget == false && isZero(m_bestEvaluation - Coalition::target))
		{
			LOG_ANALYSE << "Best @" << m_updateCounter * m_populationSize << "  " << Coalition::target << '\n';
			m_appearTarget = true;
		}
	}
}

const Coalition & PopulationBase::getEnemy() const
{
	return m_enemy;
}

bool PopulationBase::isZero(double d)
{
	if (d<Global::EPSILON && d>-Global::EPSILON)
		return true;

	return false;
}

/*
    �õ����ڵ�ʱ�䣬C++�����ǰʱ�����ǷѾ�������
	return ʱ���ַ�������ʽ��Mmm dd hh:mm:ss������"May  3 18:10:34"
*/
string PopulationBase::getTimeNow()
{
	using namespace chrono;
	system_clock::time_point tp_now = system_clock::now();
	time_t tt_now = system_clock::to_time_t(tp_now);
	char str[26];
	ctime_s(str, sizeof str, &tt_now);
	string timeNow(str);
	return timeNow.substr(4, 15);
}

/*
    ���³�ʼ���з�̹������
*/
void PopulationBase::resetEnemy(const string & way)
{
	if (way == string("8"))
	{
		m_enemy.setup_8(Tank::ABILITY_DISTANCE, true, Coalition());
	}
	else if (way == string("CR"))
	{
		m_enemy.setup_CR(Tank::ABILITY_DISTANCE, true, Coalition());
	}
}

/*
    ����ʵ����Ʊ���
*/
void PopulationBase::resetExperVariable()
{
	m_bestEvaluation = -Coalition::INDIVIDUAL_SIZE;
	m_appearTarget = false;
	m_updateCounter = 0;
	m_experimentTimes++;
	Global::dre.seed(m_experimentTimes);
}
