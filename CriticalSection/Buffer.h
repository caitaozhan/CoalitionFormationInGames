#pragma once
#include <vector>
#include <thread>
#include <mutex>
#include <condition_variable>
#include "../Coalition/Coalition.h"
using namespace std;

extern const int bufferSize;

// a vector of bestCoalitoins is the critical section
// producer adds bestCoalitions, while consumer reduces bestCoalitions
extern vector<Coalition> bestCoalitions;

// mutex for bestCoalition
extern mutex mtxBC;

// to wait the producer thread when the buffer is full
extern condition_variable cvProducer;

// to wait the consumer thread when the buffer is empty
extern condition_variable cvConsumer;

