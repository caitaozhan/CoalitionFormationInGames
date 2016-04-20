#include "Tank.h"

Tank::Tank()
{
	m_arrayIndex = ofVec2f(0, 0);
	m_position.set(0, 0, 0);
	m_abilityDistance = 0;
	m_isEnemy = false;
}

void Tank::setup(ofVec2f arrayIndex, double abilityDistance, bool isEnemy)
{
	m_arrayIndex = arrayIndex;
	m_position.set(xArrayIndx2Coordi(m_arrayIndex.x), yArrayIndx2Coordi(m_arrayIndex.y), 0);
	m_abilityDistance = abilityDistance;
	m_isEnemy = isEnemy;
}

void Tank::draw(const ofColor &color)
{
	ofSetColor(color);
	ofDrawSphere(m_position, 3);
}

void Tank::setTankPosition(double x, double y, double z)
{
	m_position = ofPoint(x, y, z);
}

void Tank::setTankArrayIndex(int x, int y)
{
	m_arrayIndex.x = x;
	m_arrayIndex.y = y;
}

const ofVec2f & Tank::getArrayIndex() const
{
	return m_arrayIndex;
}

inline int Tank::xCoordi2ArrayIndx(double xCoordi)
{
	if (xCoordi > -EPSILON)
		return int(xCoordi + 0.1) / 10 + WIDTH / 2; // bug：这里的除以10这个数字，需要随mesh的“放大倍数”而改动
	else
		return int(xCoordi - 0.1) / 10 + WIDTH / 2;
}

inline int Tank::yCoordi2ArrayIndx(double yCoordi)
{
	if (yCoordi > -EPSILON)
		return int(yCoordi + 0.1) / 10 + HEIGHT / 2;
	else
		return int(yCoordi - 0.1) / 10 + HEIGHT / 2;
}

inline double Tank::xArrayIndx2Coordi(int x)
{
	return (x - WIDTH / 2) * 10.0;
}

inline double Tank::yArrayIndx2Coordi(int y)
{
	return (y - HEIGHT / 2) * 10.0;
}

bool Tank::checkInBoundary(ofVec2f arrayIndex)
{
	if (arrayIndex.x < 0 || arrayIndex.x >= WIDTH || arrayIndex.y < 0 || arrayIndex.y >= HEIGHT)
	{
		return false;
	}
	return true;
}
