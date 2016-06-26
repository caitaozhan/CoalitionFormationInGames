#pragma once
#include "ofMain.h"
#include "Global.h"

class Tank
{
public:
	Tank();
	Tank(const Tank &t);
	void setup(const ofVec2f &arrayPoint, double abilityDistance, bool isEnemy);
	void draw(const ofColor &color);
	void setTankPosition(double x, double y, double z);
	void setTankArrayIndex(int x, int y);
	void setAbilityDistance(double ability);
	void setIsEnemy(bool isEnemy);
	const ofPoint& getPosition()const;
	const ofVec2f& getArrayIndex() const;
	const double   getAbilityDistance()const;
	const bool     getIsEnemy()const;


	bool operator!=(const Tank &t)const;
	bool operator==(const Tank &t)const;

	static int    xCoordi2ArrayIndx(double xCoordi);   // 几何坐标 --> 数组下标
	static int    yCoordi2ArrayIndx(double yCoordi);
	static double xArrayIndx2Coordi(int x);            // 数组下标 --> 几何坐标
	static double yArrayIndx2Coordi(int y);
	static bool   checkInBoundary(const ofVec2f &arrayIndex); // 检查坦克的位置是否越界
	static bool   ckeckInBF(const ofVec2f &arrayIndex);

private:
	ofPoint m_position;           // 几何坐标
	ofVec2f m_arrayIndex;         // 数组下标
	double  m_abilityDistance;    // 能力：射程
	bool    m_isEnemy;            // 是否为敌方坦克
};