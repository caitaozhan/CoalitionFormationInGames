#include "Buffer.h"

const int bufferSize = 100;
// the size of the buffer is 100
vector<Coalition> bestCoalitions(bufferSize);
condition_variable cvProducer;
condition_variable cvConsumer;

