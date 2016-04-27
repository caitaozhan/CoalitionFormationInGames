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

	static int    xCoordi2ArrayIndx(double xCoordi);   // �������� --> �����±�
	static int    yCoordi2ArrayIndx(double yCoordi);
	static double xArrayIndx2Coordi(int x);            // �����±� --> ��������
	static double yArrayIndx2Coordi(int y);
	static bool   checkInBoundary(const ofVec2f &arrayIndex); // ���̹�˵�λ���Ƿ�Խ��
	static bool   ckeckInBF(const ofVec2f &arrayIndex);

private:
	ofPoint m_position;           // ��������
	ofVec2f m_arrayIndex;         // �����±�
	double  m_abilityDistance;    // ���������
	bool    m_isEnemy;            // �Ƿ�Ϊ�з�̹��
};