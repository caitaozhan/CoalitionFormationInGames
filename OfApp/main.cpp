#pragma once
#include "ofMain.h"
#include "ofApp.h"
#include "../Coalition/PopulationMPL.h"
#include "../Coalition/PopulationEDA.h"
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
		producerCalculating(PopulationMPL());
		//producerCalculating(PopulationEDA());
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
