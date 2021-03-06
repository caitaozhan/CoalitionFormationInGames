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

void producerCalculating(PopulationBase * popBase)
{
	popBase->initialize();         // 初始化参数

	{
		unique_lock<mutex> lock(BUFFER_R.mtx);
		BUFFER_R.enemy = popBase->getEnemy();     // 初始化BUFFER_R.enemy
	}
	{
		unique_lock<mutex> lock(BUFFER.mtx);     // RAII写法
		BUFFER.bufferSize = popBase->getSize() * 2;
	}
	vector<Coalition> newBestCoalitions;
	while (popBase->getStop() == false)        // when termination conditions are not satisfied
	{
		{// critical section
			unique_lock<mutex> lock(BUFFER_R.mtx);
			popBase->setResetEnemy(BUFFER_R.resetEnemy);   // 从 BUFFER_R 获取界面的响应信号
			popBase->setResetMe(BUFFER_R.resetMe);
			popBase->setUpdate(BUFFER_R.update);
		}

		if(popBase->getUpdate() == false)
		{
			this_thread::sleep_for(chrono::milliseconds(200));
			continue;
		}
		if (popBase->getResetEnemy() == true)
		{
			popBase->resetEnemy(string("8"));
			popBase->resetMe();
			unique_lock<mutex> lock(BUFFER_R.mtx);
			BUFFER_R.enemy = popBase->getEnemy();             // 更新 BUFFER.enemy

			BUFFER_R.resetEnemy = false;
		}
		if (popBase->getResetMe() == true)
		{
			popBase->resetExperVariable();
			popBase->resetMe();
			unique_lock<mutex> lock(BUFFER_R.mtx);
			BUFFER_R.resetMe = false;
		}

		popBase->update();                     // this line of code should be time-costy
		popBase->updateBestCoalitions(newBestCoalitions);

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
