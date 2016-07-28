#include "Buffer.h"

// the size of the buffer is 200
vector<Coalition> bestCoalitions(200);
condition_variable cvProducer;
condition_variable cvConsumer;

