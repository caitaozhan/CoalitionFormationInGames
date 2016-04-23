#pragma once
#include <ofMain.h>
#include "Tank.h"
#include "Global.h"

class Coalition
{
public:
	Coalition();                                       // 默认构造函数
	void setup_8(double abilityDistance, bool isEnemy, const Coalition &enemy);  // 初始化一个联盟，随机的8联通区域
	void setup_CR(double abilityDistance, bool isEnemy, const Coalition &enemy); // 初始化一个联盟，Complete Random
	void setColor(bool isEnemy);
	void draw();
	bool contain(const vector<ofVec2f> &vecArrayIndex, const ofVec2f &arrayIndex);
	bool contain(const Coalition &enemy, const ofVec2f &arrayIndex);
	
	const vector<Tank>& getCoalition() const;
	const double getSimpleEvaluate() const;
	void  setSimpleEvaluate(const double evaluate);

	static int simpleEvalute(const Coalition &enemy, const Coalition &me);  // 简单的评价一个联盟的好坏：依据双方的识别个数
	string toString(string evaluateKind);

private:
	vector<Tank> m_coalition;
	ofColor m_color;
	double  m_simpleEvaluate;

};