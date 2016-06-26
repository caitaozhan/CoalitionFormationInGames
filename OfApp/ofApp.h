#pragma once

#include "ofMain.h"
#include "../Coalition/Coalition.h"
#include "../Analyze/AnalyzeLog.h"

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

	void updatePopluation();          // 更新种群
	void updateBestCoalition();       // 更新最好的联盟
	void updateWeight();              // 计算估值 --> 适应值 --> 权值
	void updatePMatrix();             // 统计工作，更新概率矩阵
	bool isZero(double d);
	void writeLogMatrix(int updateCounter);      // 打印矩阵到log
	int  writeLogAnalyse(int updateCounter);     // 打印算法的分析

	static const int MAX_UPDATE;
	static const int MAX_EXPERIMENT;

private:
	vector<Coalition> m_population;   // 论文中的 archive
	Coalition m_bestCoalition;
	Coalition m_enemy;
	ofMesh    m_mesh;
	ofEasyCam m_easyCam;

	bool m_update;
	int  m_updateCounter;

	bool m_appearTarget;
	int  m_experimentTimes;
};

//Universal function which sets normals for the triangle mesh
void setNormals(ofMesh &mesh);

/*
do to:

一：evaluate 一个联盟的好坏
	1. enemy 有 a 个Tank可以识别我军（任何一个就算），我军识别 enemy 有 b 个。如果 a < b，则我军有利。
		这种方式是不是有点“粗放”：
	2. 模拟打仗：需要加入“血”，规定发射炮弹的方式：比如打最近的坦克

二：把 probability matrix 结合进去
	1. 如何把 evaluation 的结果转换成概率，概率之和还要等于 1

*/