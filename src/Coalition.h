#pragma once
#include <ofMain.h>
#include "Tank.h"
#include "Global.h"

class Coalition
{
public:
	Coalition();                                       // ����Ĭ�Ϲ��캯��
	Coalition(const Coalition &c);                     // ���ؿ������캯��
	Coalition& operator=(const Coalition& c);          // ���ظ�ֵ����
	void setup_8(double abilityDistance, bool isEnemy, const Coalition &enemy);  // ��ʼ��һ�����ˣ������8��ͨ����
	void setup_CR(double abilityDistance, bool isEnemy, const Coalition &enemy); // ��ʼ��һ�����ˣ�Complete Random
	
	static int simpleEvalute(const Coalition &enemy, const Coalition &me);  // �򵥵�����һ�����˵ĺû�������˫����ʶ�����
	double calculateFitness(double evaluate, double maxEvaluate, double minEvaluate);
	double calculateWeight(double fitness);
	
	void  setSimpleEvaluate(const double evaluate);
	void  setFitness(double fit);
	void  setWeight(double weight);
	void  setColor(bool isEnemy);
	void  setAbilityDistance(double abilityDistance);
	void  setIsEnemy(bool isEnemy);
	void  setCoalition(int i, const Tank &t);
	void  pushBackTank(const Tank &t);
	const vector<Tank>& getCoalition() const;
	const Tank& getCoalition(int i)const;
	const double getSimpleEvaluate() const;
	const double getFitness()const;
	const double getWeight()const;
	const ofColor& getColor()const;
	const int getSize()const;
	const double getAbilityDistance()const;
	const bool   getIsEnemy()const;

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
	double  m_abilityDistance;
	bool    m_isEnemy;

};