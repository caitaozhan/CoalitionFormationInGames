#pragma once
#include <iostream>
#include <vector>
#include "Coalition.h"
using namespace std;

class Population
{
private:
	Population();
	Population(size_t size);

public:
	vector<Coalition> m_population;
};
