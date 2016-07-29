#pragma once
#include <iostream>
#include <queue>
#include <vector>
#include <thread>
#include <mutex>
#include <condition_variable>
#include "../Coalition/Population.h"
using namespace std;

class Buffer
{
public:
	Buffer(int size);

	int bufferSize;

	// a queue of bestCoalitoins is the critical section
	// producer adds bestCoalitions, while consumer reduces bestCoalitions
	queue<Coalition> bestCoalitions;

	// producer need it when calculating, consumer draws the enemy
	Coalition enemy;

	// mutex for bestCoalition
	mutex mtx;

	// to wait the producer thread when the buffer is full
	condition_variable cvProducer;

	// to wait the consumer thread when the buffer is empty
	condition_variable cvConsumer;

	void add(vector<Coalition> && newBestCoalitions);
};

extern Buffer BUFFER;

extern void producerCalculating(Population && population);