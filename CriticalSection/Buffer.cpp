#include "Buffer.h"

Buffer BUFFER(100);

Buffer::Buffer(int size)
{
	bufferSize = size;
}

void Buffer::add(vector<Coalition>&& newBestCoalitions)
{
	for (int i = 0; i < newBestCoalitions.size(); ++i)
	{
		BUFFER.bestCoalitions.push(move(newBestCoalitions[i]));
	}
}


void producerCalculating(Population && population)
{
	population.initialize();  // 这里可以有很多参数
	BUFFER.bufferSize = population.getSize() * 2;
	vector<Coalition> newBestCoalitions;
	while (population.getStop() == false)        // when termination conditions are not satisfied
	{
		population.update();                     // this line of code should be time-costy
		newBestCoalitions = population.getBestCoalitions();
		{
			unique_lock<mutex> lock(BUFFER.mtx);
			// block when BUFFER.bC.size() + newBC.size() > BUFFER.bufferSize
			BUFFER.cvProducer.wait(lock, [&] {BUFFER.bestCoalitions.size() + newBestCoalitions.size() <= BUFFER.bufferSize; });
			
			BUFFER.add(move(newBestCoalitions));

			BUFFER.cvConsumer.notify_one();

		}
	}
}

