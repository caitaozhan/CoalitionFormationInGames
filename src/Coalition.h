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
	const vector<Tank>& getCoalition() const;
	
	static int simpleEvalute(const Coalition &enemy, const Coalition &me);  // �򵥵�����һ�����˵ĺû�������˫����ʶ�����
	double calculateFitness(double evaluate, double maxEvaluate, double minEvaluate);
	double calculateWeight(double fitness);
	
	void  setSimpleEvaluate(const double evaluate);
	void  setFitness(double fit);
	void  setWeight(double weight);
	const double getSimpleEvaluate() const;
	const double getFitness()const;
	const double getWeight()const;
	
	void setColor(bool isEnemy);
	void draw();
	bool contain(const vector<ofVec2f> &vecArrayIndex, const ofVec2f &arrayIndex);
	bool contain(const Coalition &enemy, const ofVec2f &arrayIndex);

	string toString(string evaluateKind);

	static void update_BF(vector<ofVec2f> vecArrayIndex);

private:
	vector<Tank> m_coalition;
	ofColor m_color;
	double  m_simpleEvaluate;
	double  m_fitness;
	double  m_weight;

};