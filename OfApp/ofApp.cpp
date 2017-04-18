#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){

	//ofSetFrameRate(1000);

	HEIGHT = Global::HEIGHT;
	WIDTH = Global::WIDTH;
	EPSILON = Global::EPSILON;

	ofTranslate(ofGetWidth() / 2, ofGetHeight() / 2);

	//Set up vertices and colors
	for (int y = 0; y < HEIGHT; ++y)
	{
		for (int x = 0; x < WIDTH; ++x)
		{
			m_mesh.addVertex(ofPoint(Tank::xArrayIndx2Coordi(x), Tank::yArrayIndx2Coordi(y) , 0));  //数组下标 --> 几何坐标
			m_mesh.addColor(ofColor(128, 128, 255));
		}
	}
	//Set up triangles' indices
	for (int y = 0; y < HEIGHT - 1; ++y)
	{
		for (int x = 0; x < WIDTH - 1; ++x)
		{
			int i1 = x + WIDTH*y;
			int i2 = x + 1 + WIDTH*y;
			int i3 = x + WIDTH*(y + 1);
			int i4 = x + 1 + WIDTH*(y + 1);
			m_mesh.addTriangle(i1, i2, i3);
			m_mesh.addTriangle(i2, i4, i3);
		}
	}
	setNormals(m_mesh);
	ofEnableDepthTest();

}

//--------------------------------------------------------------
void ofApp::update(){
	
}

//--------------------------------------------------------------
void ofApp::draw(){
	
	{// critical section
		unique_lock<mutex> lock(BUFFER.mtx);
		// block(wait) when BUFFER.bC.size() == 0
		BUFFER.cvConsumer.wait(lock, [] {return BUFFER.bestCoalitions.size() != 0; });
		if (BUFFER.bestCoalitions.size() > 0)
		{
			m_coalitionToDraw = BUFFER.bestCoalitions.front();
		}
		if (BUFFER.bestCoalitions.size() > 1)
		{
			BUFFER.bestCoalitions.pop();           // 只有缓冲区的元素个数 > 1 的时候，才弹出。当生产者过慢的时候，这样做可以防止消费者卡顿
		}
		// cout << BUFFER.bestCoalitions.size() << "\n";  早期，queue很空；后期，很多的联盟都是最优解，一次性往queue加入很多，于是queue很满
		// notiry(wake up) when BUFFER.bC.size() + newBC.size() <= BUFFER.bufferSize
		BUFFER.cvProducer.notify_one();
	}
	{
		unique_lock<mutex> lock(BUFFER_R.mtx);
		m_enemyToDraw = BUFFER_R.enemy;
	}

	m_easyCam.begin();

	string msg = "fps: " + ofToString(ofGetFrameRate(), 2);
	ofDrawBitmapString(msg, ofPoint(100, 100));
	
	m_mesh.drawWireframe();
	
	m_enemyToDraw.draw();
	m_coalitionToDraw.draw();
	ofDrawBitmapString(m_coalitionToDraw.toString("simpleEvaluate"), -100, 100);
	
	m_easyCam.end();
	
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){

	if (key == 'm')   // coalition Setup
	{
		unique_lock<mutex> lock(BUFFER_R.mtx);
		BUFFER_R.resetMe = true;
	}
	else if (key == 'e')
	{
		unique_lock<mutex> lock(BUFFER_R.mtx);
		BUFFER_R.resetEnemy = true;
	}
	else if (key == 'u')
	{
		unique_lock<mutex> lock(BUFFER_R.mtx);
		BUFFER_R.update ^= 1;
	}
}

//--------------------------------------------------------------
void ofApp::keyReleased(int key){

}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y ){

}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mouseEntered(int x, int y){

}

//--------------------------------------------------------------
void ofApp::mouseExited(int x, int y){

}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h){

}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg){

}

bool ofApp::isZero(double d)
{
	if (d<EPSILON && d>-EPSILON)
		return true;
	
	return false;
}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo){ 

}

//Universal function which sets normals for the triangle mesh
void setNormals(ofMesh &mesh)
{
	//The number of the vertices
	int nV = mesh.getNumVertices();

	//The number of the triangles
	int nT = mesh.getNumIndices() / 3;

	//cout << nV << "  " << nT << endl;  //10,000 & 19,602

	vector<ofPoint> norm(nV);   //Array for the normals

								//Scan all the triangles. For each triangle add its
								//normal to norm's vectors of triangle's vertices
	for (int t = 0; t < nT; ++t)
	{
		//Get indices of the triangle t
		int i1 = mesh.getIndex(3 * t);
		int i2 = mesh.getIndex(3 * t + 1);
		int i3 = mesh.getIndex(3 * t + 2);

		//Get vertices of the triangle
		const ofPoint& v1 = mesh.getVertex(i1);
		const ofPoint& v2 = mesh.getVertex(i2);
		const ofPoint& v3 = mesh.getVertex(i3);

		//Compute the triangle's normal
		ofPoint dir = ((v2 - v1).crossed(v3 - v1)).normalized();

		//Accumulate it to norm array for i1, i2, i3
		norm[i1] += dir;
		norm[i2] += dir;
		norm[i3] += dir;
	}

	//Normalize the normal's length
	for (int i = 0; i < nV; ++i)
	{
		norm[i].normalize();
	}

	//Set the normals to mesh
	mesh.clearNormals();
	mesh.addNormals(norm);
}
