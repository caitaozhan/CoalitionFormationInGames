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
	const Coalition& getBestCoalition()const;

	void resetMe();

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

	//void updatePopluation();          // ������Ⱥ
	//void updateBestCoalition();       // ������õ�����
	//void updateWeight();              // �����ֵ --> ��Ӧֵ --> Ȩֵ
	//void updatePMatrix();             // ͳ�ƹ��������¸��ʾ���
	bool isZero(double d);
	//void writeLogMatrix(int updateCounter);      // ��ӡ����log
	int  writeLogAnalyse(int updateCounter);     // ��ӡ�㷨�ķ���

	//static const int MAX_UPDATE;
	//static const int MAX_EXPERIMENT;

private:
	//vector<Coalition> m_population;   // �����е� archive
	//Coalition m_bestCoalition;
	//Coalition m_enemy;
	Coalition m_coalitionToDraw;
	Coalition m_enemyToDraw;

	int HEIGHT;
	int WIDTH;

	double EPSILON;

	ofMesh    m_mesh;
	ofEasyCam m_easyCam;

	//bool m_update;
	//int  m_updateCounter;

	/*bool m_appearTarget;
	int  m_experimentTimes;*/
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