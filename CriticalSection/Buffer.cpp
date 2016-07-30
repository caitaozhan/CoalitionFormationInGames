#include "Buffer.h"

Buffer BUFFER(100);

BufferResponse BUFFER_R(false);

Buffer::Buffer(int size)
{
	bufferSize = size;
	
	enemy.initialize(INDIVIDUAL_SIZE);                         // 修正BUG：之前 m_enemy 调用重载的默认构造函数，导致vector大小=0
	//m_enemy.setup_8(ABILITY_DISTANCE, true, Coalition()); 
	//m_enemy.writeLog();
	//m_enemy.setup_CR(ABILITY_DISTANCE, true, Coalition());
	
	enemy.setup_file(ABILITY_DISTANCE, true, "../sample/4_case_20.txt");

}

void Buffer::add(vector<Coalition>&& newBestCoalitions)
{
	for (int i = 0; i < newBestCoalitions.size(); ++i)
	{
		BUFFER.bestCoalitions.push(move(newBestCoalitions[i]));
	}
}

BufferResponse::BufferResponse(bool b)
{
	resetMe = b;
	resetEnemy = b;
	update = b;
}


void producerCalculating(Population && population)
{
	population.initialize();  // 这里可以有很多参数
	BUFFER.bufferSize = population.getSize() * 2;
	vector<Coalition> newBestCoalitions;
	while (population.getStop() == false)        // when termination conditions are not satisfied
	{
		population.update();                     // this line of code should be time-costy
		population.getBestCoalitions(newBestCoalitions);

		{// critical section
			unique_lock<mutex> lock(BUFFER.mtx);
			// block when BUFFER.bC.size() + newBC.size() > BUFFER.bufferSize
			BUFFER.cvProducer.wait(lock, [&] {BUFFER.bestCoalitions.size() + newBestCoalitions.size() <= BUFFER.bufferSize; });
			
			BUFFER.add(move(newBestCoalitions));

			// unblock when BUFFER.bC.size() != 0
			BUFFER.cvConsumer.notify_one();
		}
	}
}

