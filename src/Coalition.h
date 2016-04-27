#pragma once
#include <ofMain.h>
#include "Tank.h"
#include "Global.h"

class Coalition
{
public:
	Coalition();                                       // 重载默认构造函数
	Coalition(const Coalition &c);                           // 重载拷贝构造函数
	void setup_8(double abilityDistance, bool isEnemy, const Coalition &enemy);  // 初始化一个联盟，随机的8联通区域
	void setup_CR(double abilityDistance, bool isEnemy, const Coalition &enemy); // 初始化一个联盟，Complete Random
	const vector<Tank>& getCoalition() const;
	
	static int simpleEvalute(const Coalition &enemy, const Coalition &me);  // 简单的评价一个联盟的好坏：依据双方的识别个数
	double calculateFitness(double evaluate, double maxEvaluate, double minEvaluate);
	double calculateWeight(double fitness);
	
	void  setSimpleEvaluate(const double evaluate);
	void  setFitness(double fit);
	void  setWeight(double weight);
	void  setColor(bool isEnemy);
	const double getSimpleEvaluate() const;
	const double getFitness()const;
	const double getWeight()const;
	const ofColor& getColor()const;

	void draw();
	bool contain(const vector<ofVec2f> &vecArrayIndex, const ofVec2f &arrayIndex);
	bool contain(const Coalition &enemy, const ofVec2f &arrayIndex);

	string toString(string evaluateKind);
	static void update_BF(vector<ofVec2f> vecArrayIndex);

	static ofVec2f getPlaceFromPMatrix();

private:
	vector<Tank> m_coalition;
	ofColor m_color;
	double  m_simpleEvaluate;
	double  m_fitness;
	double  m_weight;

};