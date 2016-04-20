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

	static int    xCoordi2ArrayIndx(double xCoordi);   // �������� --> �����±�
	static int    yCoordi2ArrayIndx(double yCoordi);
	static double xArrayIndx2Coordi(int x);            // �����±� --> ��������
	static double yArrayIndx2Coordi(int y);
	static bool   checkInBoundary(ofVec2f arrayIndex); // ���̹�˵�λ���Ƿ�Խ��

private:
	ofPoint m_position;           // ��������
	ofVec2f m_arrayIndex;         // �����±�
	double  m_abilityDistance;    // ���������
	bool    m_isEnemy;            // �Ƿ�Ϊ�з�̹��
};