#pragma once
#include "ofMain.h"
#include "ofApp.h"
#include "../Coalition/Population.h"
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
		producerCalculating(Population());
	}
}


//========================================================================
int main( )
{
	thread producer(producerCalculate);
	thread consumer(consumerDraw);

	producer.join();
	consumer.join();

	/*Population pop;
	pop.initialize(0.9, 0.9, 32);
	while (true)
	{
		pop.update();
	}*/

	system("pause");
	return 0;
}
