#include "Population.h"

Population::Population()
{
}

Population::Population(size_t size)
{
	m_population.reserve(size);
}

void Population::initialize()
{
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

vector<Coalition>&& Population::getBestCoalitions()
{
	return move(m_bestCoalitions);
}
