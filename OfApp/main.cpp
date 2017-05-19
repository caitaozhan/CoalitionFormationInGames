#pragma once
#include "ofMain.h"
#include "ofApp.h"
#include "../Population/PopulationBase.h"
#include "../Population/PopulationMPL.h"
#include "../Population/PopulationEDA.h"
using namespace std;

void consumerDraw()
{
	ofSetupOpenGL(1024, 768, OF_WINDOW);    // <-------- setup the GL context
	ofRunApp(new ofApp());
}

void producerCalculate()
{
	while (true)
	{
		PopulationBase * popBase = new PopulationMPL();

		//PopulationBase * popBase = new PopulationEDA();

		producerCalculating(popBase);
	}
}


//========================================================================
int main( )
{
	thread producer(producerCalculate);
	thread consumer(consumerDraw);

	producer.join();
	consumer.join();

	system("pause");
	return 0;
}
