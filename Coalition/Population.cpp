#include "Population.h"

Population::Population()
{
}

Population::Population(size_t size)
{
	m_population.reserve(size);
	m_bestCoalitionIndex.reserve(size);
}

void Population::initialize()
{
	m_population.resize(POPULATION_SIZE);                  // ��ʼ�� m_population

	for (int i = 0; i < m_population.size(); ++i)
	{
		Coalition::logNumber++;
		m_population[i].initialize(INDIVIDUAL_SIZE);       // ����BUG��֮ǰ m_population[i] �������ص�Ĭ�Ϲ��캯��������vector��С=0
		m_population[i].setup_CR(ABILITY_DISTANCE, false, m_enemy);
	}

	updateBestCoalitions();                  // �ӳ�ʼ������Ⱥ�л����õ���Ⱥ

}

void Population::update()
{


}

bool Population::getStop()
{
	return m_stop;
}

int Population::getSize()
{
	return m_population.size();
}

/*
	get coalitions with the highest evaluations, note that there could be tied best ones
*/
void Population::updateBestCoalitions()
{
	m_bestCoalitionIndex.clear();
	int n = getSize();
	double bestEvaluation = -n;
	for (int i = 0; i < n; ++i)
	{
		if (m_population[i].getSimpleEvaluate() > bestEvaluation)
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

}

void Population::getBestCoalitions(vector<Coalition>& bC)
{
	vector<Coalition> temp;
	temp.reserve(m_bestCoalitionIndex.size());
	for (int & index : m_bestCoalitionIndex)
	{
		temp.push_back(m_population[index]);
	}

	bC = move(temp);
	return;
}

bool Population::isZero(double d)
{
	if (d<EPSILON && d>-EPSILON)
		return true;

	return false;
}
