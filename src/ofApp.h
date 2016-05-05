#pragma once

#include "ofMain.h"
#include "Coalition.h"

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

	void updatePopluation();          // ������Ⱥ
	void updateWeight();              // �����ֵ --> ��Ӧֵ --> Ȩֵ
	void updatePMatrix();             // ͳ�ƹ��������¸��ʾ���
	bool isZero(double d);
	void writeLogMatrix();            // ��ӡ����log

private:
	vector<Coalition> m_population;   // �����е� archive
	Coalition m_bestCoalition;
	Coalition m_enemy;
	ofMesh    m_mesh;
	ofEasyCam m_easyCam;

	bool m_update;
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