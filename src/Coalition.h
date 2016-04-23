#pragma once
#include <ofMain.h>
#include "Tank.h"
#include "Global.h"

class Coalition
{
public:
	Coalition();                                       // Ĭ�Ϲ��캯��
	void setup_8(double abilityDistance, bool isEnemy, const Coalition &enemy);  // ��ʼ��һ�����ˣ������8��ͨ����
	void setup_CR(double abilityDistance, bool isEnemy, const Coalition &enemy); // ��ʼ��һ�����ˣ�Complete Random
	void setColor(bool isEnemy);
	void draw();
	bool contain(const vector<ofVec2f> &vecArrayIndex, const ofVec2f &arrayIndex);
	bool contain(const Coalition &enemy, const ofVec2f &arrayIndex);
	
	const vector<Tank>& getCoalition() const;
	const double getSimpleEvaluate() const;
	void  setSimpleEvaluate(const double evaluate);

	static int simpleEvalute(const Coalition &enemy, const Coalition &me);  // �򵥵�����һ�����˵ĺû�������˫����ʶ�����
	string toString(string evaluateKind);

private:
	vector<Tank> m_coalition;
	ofColor m_color;
	double  m_simpleEvaluate;

};