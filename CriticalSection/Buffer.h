#pragma once
#include <iostream>
#include <queue>
#include <vector>
#include <thread>
#include <mutex>
#include <condition_variable>
#include "../Coalition/Population.h"
#include "../Coalition/Global.h"
using namespace std;

/*
	this file includes buffer and global const values
*/

// producer produce items to the queue, while consumer consume items from it.
class Buffer
{
public:
	Buffer(int size);

	int bufferSize;

	// a queue of bestCoalitoins is the critical section
	// producer adds bestCoalitions, while consumer reduces bestCoalitions
	queue<Coalition> bestCoalitions;

	// add new items to queue
	void add(vector<Coalition> && newBestCoalitions);

	// mutex for bestCoalition
	mutex mtx;

	// to wait the producer thread when the buffer is full
	condition_variable cvProducer;

	// to wait the consumer thread when the buffer is empty
	condition_variable cvConsumer;
};


// consumer response the keyboard input
// then pass the information to producer through this buffer
class BufferResponse
{
public:
	BufferResponse(bool resetMe, bool resetEnemy, bool update);
	
	// producer need it when calculating, consumer draws the enemy
	Coalition enemy;

	bool resetMe;        // whether reset my tanks
	bool resetEnemy;     // whether reset enemy tanks
	bool update;         // whether update population

	mutex mtx;
};


extern Buffer BUFFER;

extern BufferResponse BUFFER_R;

extern void producerCalculating(Population && population);

//extern const int HEIGHT;
//extern const int WIDTH;
//extern const double EPSILON;

