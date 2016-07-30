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
	m_population.resize(POPULATION_SIZE);                  // 初始化 m_population

	for (int i = 0; i < m_population.size(); ++i)
	{
		Coalition::logNumber++;
		m_population[i].initialize(INDIVIDUAL_SIZE);       // 修正BUG：之前 m_population[i] 调用重载的默认构造函数，导致vector大小=0
		m_population[i].setup_CR(ABILITY_DISTANCE, false, m_enemy);
	}

	updateBestCoalitions();                  // 从初始化的种群中获得最好的种群

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
