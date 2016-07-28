#include "ofMain.h"
#include "ofApp.h"
#include "../CriticalSection/Buffer.h"


void consumerDraw()
{
	// this kicks off the running of my app
	// can be OF_WINDOW or OF_FULLSCREEN
	// pass in width and height too:
	ofSetupOpenGL(1024, 768, OF_WINDOW);    // <-------- setup the GL context
	ofRunApp(new ofApp());
}

void producerCalculate()
{

}

//========================================================================
int main( )
{

	



}
