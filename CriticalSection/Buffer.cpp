#include "Buffer.h"

Buffer BUFFER(100);

BufferResponse BUFFER_R(false, false, true);

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

BufferResponse::BufferResponse(bool resetMe, bool resetEnemy, bool update)
{
	this->resetMe = resetMe;
	this->resetEnemy = resetEnemy;
	this->update = update;
}


void producerCalculating(Population && population)
{
	population.initialize(0.9, 0.9, 32, 32);     // 初始化参数
	{
		unique_lock<mutex> lock(BUFFER_R.mtx);
		population.getEnemy(BUFFER_R.enemy);     // 初始化BUFFER_R.enemy
	}
	BUFFER.bufferSize = population.getSize() * 2;
	vector<Coalition> newBestCoalitions;
	while (population.getStop() == false)        // when termination conditions are not satisfied
	{
		{
			unique_lock<mutex> lock(BUFFER_R.mtx);
			if (BUFFER_R.resetEnemy == true)
			{
				population.resetEnemy(string("8"));
				population.getEnemy(BUFFER_R.enemy);             // 更新 BUFFER.enemy
			    // m_enemy.writeLog();
				BUFFER_R.resetEnemy = false;
			}
			if (BUFFER_R.resetMe == true)
			{
				population.resetMe();
				BUFFER_R.resetMe = false;
			}
			if (BUFFER_R.update == false)
			{
				this_thread::sleep_for(chrono::seconds(3));
				BUFFER_R.update = true;
			}
		}

		population.update();                     // this line of code should be time-costy
		population.getBestCoalitions(newBestCoalitions);

		{// critical section
			unique_lock<mutex> lock(BUFFER.mtx);
			// block when BUFFER.bC.size() + newBC.size() > BUFFER.bufferSize
			BUFFER.cvProducer.wait(lock, [&] {return BUFFER.bestCoalitions.size() + newBestCoalitions.size() <= BUFFER.bufferSize; });
			
			BUFFER.add(move(newBestCoalitions));

			// unblock when BUFFER.bC.size() != 0
			BUFFER.cvConsumer.notify_one();
		}
	}
}

