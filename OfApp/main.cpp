#pragma once
#include "ofMain.h"
#include "ofApp.h"
#include "../Coalition/Population.h"
#include "../CriticalSection/Buffer.h"
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
		producerCalculating(Population(10));
	}
}


//========================================================================
int main( )
{
	thread producer(producerCalculate);
	thread consumer(consumerDraw);

	producer.join();
	consumer.join();

	return 0;
}
