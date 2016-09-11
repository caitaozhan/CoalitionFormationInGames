#include "Tank.h"

const int Tank::PIXEL_PER_INDEX = 5;
double Tank::ABILITY_DISTANCE = 2.9;

Tank::Tank()
{
	m_arrayIndex = ofVec2f(0, 0);
	m_position.set(0, 0, 0);
	m_abilityDistance = 0;
	m_isEnemy = false;
}

Tank::Tank(const Tank & t)
{
	m_position = t.getPosition();
	m_arrayIndex = t.getArrayIndex();
	m_abilityDistance = t.getAbilityDistance();
	m_isEnemy = t.getIsEnemy();
}

void Tank::setup(const ofVec2f &arrayIndex, double abilityDistance, bool isEnemy)
{
	m_arrayIndex = arrayIndex;
	m_position.set(xArrayIndx2Coordi(m_arrayIndex.x), yArrayIndx2Coordi(m_arrayIndex.y), 0);
	m_abilityDistance = abilityDistance;
	m_isEnemy = isEnemy;
}

void Tank::draw(const ofColor &color)
{
	ofSetColor(color);
	ofDrawSphere(m_position, 1.5);
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

void Tank::setAbilityDistance(double ability)
{
	m_abilityDistance = ability;
}

void Tank::setIsEnemy(bool isEnemy)
{
	m_isEnemy = isEnemy;
}

const ofPoint & Tank::getPosition() const
{
	return m_position;
}

const ofVec2f & Tank::getArrayIndex() const
{
	return m_arrayIndex;
}

const double Tank::getAbilityDistance() const
{
	return m_abilityDistance;
}

const bool Tank::getIsEnemy() const
{
	return m_isEnemy;
}

bool Tank::operator!=(const Tank & t) const
{
	return m_arrayIndex != t.m_arrayIndex;
}

bool Tank::operator==(const Tank & t) const
{
	return m_arrayIndex == t.m_arrayIndex;
}

// ���� inline Ϊʲô debug �ɹ����� release ʧ�ܣ�
int Tank::xCoordi2ArrayIndx(double xCoordi)
{
	if (xCoordi > -Global::EPSILON)
		return int(xCoordi + 0.1) / PIXEL_PER_INDEX + Global::WIDTH / 2; // bug������ĳ���10������֣���Ҫ��mesh�ġ��Ŵ��������Ķ�
	else
		return int(xCoordi - 0.1) / PIXEL_PER_INDEX + Global::WIDTH / 2;
}

int Tank::yCoordi2ArrayIndx(double yCoordi)
{
	if (yCoordi > -Global::EPSILON)
		return int(yCoordi + 0.1) / PIXEL_PER_INDEX + Global::HEIGHT / 2;
	else
		return int(yCoordi - 0.1) / PIXEL_PER_INDEX + Global::HEIGHT / 2;
}

double Tank::xArrayIndx2Coordi(int x)
{
	return (x - (double)Global::WIDTH / 2) * PIXEL_PER_INDEX;
}

double Tank::yArrayIndx2Coordi(int y)
{
	return (y - (double)Global::HEIGHT / 2) * PIXEL_PER_INDEX;
}

bool Tank::checkInBoundary(const ofVec2f &arrayIndex)
{
	if (arrayIndex.x < 0 || arrayIndex.x >= Global::WIDTH || arrayIndex.y < 0 || arrayIndex.y >= Global::HEIGHT)
	{
		return false;
	}
	return true;
}

bool Tank::ckeckInBF(const ofVec2f & arrayIndex)
{
	if (arrayIndex.x > Global::BF_LR.x || arrayIndex.x < Global::BF_UL.x || arrayIndex.y < Global::BF_LR.y || arrayIndex.y > Global::BF_UL.y)
	{// �޸�һ��BUG������ô���������֡��ܳ���if�����淸����
		return false;
	}
	return true;
}
