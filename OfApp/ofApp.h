#pragma once

#include "ofMain.h"
#include <mutex>
#include <thread>
#include "../Coalition/Coalition.h"
#include "../Coalition/Global.h"
#include "../CriticalSection/Buffer.h"
using namespace std;

extern Buffer BUFFER;

class ofApp : public ofBaseApp
{
public:
	void setup();
	void update();
	void draw();

	void keyPressed(int key);
	void keyReleased(int key);
	void mouseMoved(int x, int y );
	void mouseDragged(int x, int y, int button);
	void mousePressed(int x, int y, int button);
	void mouseReleased(int x, int y, int button);
	void mouseEntered(int x, int y);
	void mouseExited(int x, int y);
	void windowResized(int w, int h);
	void dragEvent(ofDragInfo dragInfo);
	void gotMessage(ofMessage msg);

	bool isZero(double d);

private:
	Coalition m_coalitionToDraw;
	Coalition m_enemyToDraw;

	int HEIGHT;
	int WIDTH;

	double EPSILON;

	ofMesh    m_mesh;
	ofEasyCam m_easyCam;
};

//Universal function which sets normals for the triangle mesh
void setNormals(ofMesh &mesh);

/*
do to:

һ��evaluate һ�����˵ĺû�
	1. enemy �� a ��Tank����ʶ���Ҿ����κ�һ�����㣩���Ҿ�ʶ�� enemy �� b ������� a < b�����Ҿ�������
		���ַ�ʽ�ǲ����е㡰�ַš���
	2. ģ����̣���Ҫ���롰Ѫ�����涨�����ڵ��ķ�ʽ������������̹��

������ probability matrix ��Ͻ�ȥ
	1. ��ΰ� evaluation �Ľ��ת���ɸ��ʣ�����֮�ͻ�Ҫ���� 1

*/