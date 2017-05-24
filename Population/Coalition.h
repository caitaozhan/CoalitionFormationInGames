#pragma once
#include <vector>
#include "../Apriori/ItemSet.h"
#include "Tank.h"


class Coalition
{
public:
	Coalition();                                       // ����Ĭ�Ϲ��캯��
	Coalition(const Coalition &c);                     // ���ؿ������캯��
	Coalition& operator=(const Coalition& c);          // ���ظ�ֵ����
	// ���⣺�Ƿ�Ҫ���أ�������������
	void initialize(int individualSize);               // ��ʼ�������������������Ԫ�ظ���
	void initialize(int individualSize, int abilityDistance, bool isEnemy);         // ��ʼ�������������İ汾
	void setup_8(double abilityDistance, bool isEnemy, const Coalition &enemy);     // ��ʼ��һ�����ˣ������8��ͨ����
	void setup_CR(double abilityDistance, bool isEnemy, const Coalition &enemy);    // ��ʼ��һ�����ˣ�Complete Random
	void setup_file(double abilityDistance, bool isEnemy, const string &filename);  // ���ļ�������һ������
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
	const vector<Tank>& getCoalition()const;
	const Tank&  getCoalition(int i)const;
	const double getSimpleEvaluate()const;
	const double getFitness()const;
	const double getWeight()const;
	const ofColor& getColor()const;
	const int    getSize()const;
	const double getAbilityDistance()const;
	const bool   getIsEnemy()const;

	bool isZero(double d);

	void draw();
	bool contain(const vector<ofVec2f> &vecArrayIndex, const ofVec2f &arrayIndex);
	bool contain(const Coalition &c, const ofVec2f &arrayIndex);

	string toString(string evaluateKind);
	static void update_BF(const vector<ofVec2f> &vecArrayIndex);

	ofVec2f localSearch_small(const Coalition &enemy, int i);  // backupC �ǵ�����
	ofVec2f localSearch_big(const Coalition &enemy);           // backupC �ǵ�����
	ofVec2f localSearch_big_PM(const Coalition &enemy, const vector<vector<double>> &probabilityMatrix);        // backupC �ǵ�����,������ʾ���

	ofVec2f getPlaceFromPMatrix(const vector<vector<double>> &probabilityMatrix, const vector<double> &SUM_OF_ROW, const double &TOTAL);
	static bool decrease(const Coalition & c1, const Coalition & c2);

	static int logNumber;  // ÿһ�� Coalition ������һ���Լ�����־�ļ���ͨ�������̬�����������־������
	static double target;  // ���ĳһ enemy�����ܽ����� evulation = target������Ϊ�ﵽĿ����

	static int INDIVIDUAL_SIZE;  // ÿһ��Coalition�Ĵ�С

	static uniform_real_distribution<double> urd_0_1;
	static uniform_int_distribution<int> uid_x;
	static uniform_int_distribution<int> uid_y;

	ItemSet toItemSet() const;
	void actionMove(const ItemSet & sourceSet, const ItemSet & destinationSet);

private:
	vector<Tank> m_coalition;
	ofColor  m_color;
	double   m_simpleEvaluate;
	double   m_fitness;
	double   m_weight;
	double   m_abilityDistance;  // �����������ô��Tank ����Ҳ�д�����
	bool     m_isEnemy;
};
