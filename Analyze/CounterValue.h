#pragma once

class CounterValue
{
public:
	CounterValue() :m_counter(0), m_value(0) {};
	CounterValue(int counter, int value) :m_counter(counter), m_value(value) {};

	int    m_counter;
	double m_value;
};
