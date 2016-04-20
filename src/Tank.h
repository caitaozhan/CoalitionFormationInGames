#pragma once
#include "ofMain.h"
#include "Global.h"

class Tank
{
public:
	Tank();
	void setup(ofVec2f arrayPoint, double abilityDistance, bool isEnemy);
	void draw(const ofColor &color);
	void setTankPosition(double x, double y, double z);
	void setTankArrayIndex(int x, int y);
	const ofVec2f& getArrayIndex() const;

	static int    xCoordi2ArrayIndx(double xCoordi);   // 几何坐标 --> 数组下标
	static int    yCoordi2ArrayIndx(double yCoordi);
	static double xArrayIndx2Coordi(int x);            // 数组下标 --> 几何坐标
	static double yArrayIndx2Coordi(int y);
	static bool   checkInBoundary(ofVec2f arrayIndex); // 检查坦克的位置是否越界

private:
	ofPoint m_position;           // 几何坐标
	ofVec2f m_arrayIndex;         // 数组下标
	double  m_abilityDistance;    // 能力：射程
	bool    m_isEnemy;            // 是否为敌方坦克
};