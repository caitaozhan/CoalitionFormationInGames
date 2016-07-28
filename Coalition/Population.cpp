#include "Population.h"

Population::Population()
{
}

Population::Population(size_t size)
{
	m_population.reserve(size);
}
