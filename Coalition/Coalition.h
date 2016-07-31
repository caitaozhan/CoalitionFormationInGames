#pragma once
#include <ofMain.h>
#include "Tank.h"
#include "Global.h"


class Coalition
{
public:
	Coalition();                                       // 重载默认构造函数
	Coalition(const Coalition &c);                     // 重载拷贝构造函数
	Coalition& operator=(const Coalition& c);          // 重载赋值函数
	// 问题：是否要重载，比如析构函数
	void initialize(int individualSize);               // 初始化，参数是联盟里面的元素个数
	void setup_8(double abilityDistance, bool isEnemy, const Coalition &enemy);     // 初始化一个联盟，随机的8联通区域
	void setup_CR(double abilityDistance, bool isEnemy, const Coalition &enemy);    // 初始化一个联盟，Complete Random
	void setup_file(double abilityDistance, bool isEnemy, const string &filename);  // 从文件中输入一个联盟
	static int simpleEvalute(const Coalition &enemy, const Coalition &me);  // 简单的评价一个联盟的好坏：依据双方的识别个数
	double calculateFitness(double evaluate, double maxEvaluate, double minEvaluate);
	double calculateWeight(double fitness);
	
	void  setSimpleEvaluate(const double evaluate);
	void  setFitness(double fit);
	void  setWeight(double weight);
	void  setColor(bool isEnemy);
	void  setAbilityDistance(double abilityDistance);
	void  setIsEnemy(bool isEnemy);
	void  setCoalition(int i, const Tank &t);
	void  setStagnate0(int s);
	void  setIsStangate(bool iS);
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
	const int    getStagnate0()const;
	const bool   getIsStagnate()const;

	void resetAtStagnate0(const Coalition &m_enemy, int updateCounter);
	bool isZero(double d);

	void draw();
	bool contain(const vector<ofVec2f> &vecArrayIndex, const ofVec2f &arrayIndex);
	bool contain(const Coalition &c, const ofVec2f &arrayIndex);

	string toString(string evaluateKind);
	static void update_BF(const vector<ofVec2f> &vecArrayIndex);

	ofVec2f localSearch_small(const Coalition &enemy, int i);  // backupC 是调用者
	ofVec2f localSearch_big(const Coalition &enemy);           // backupC 是调用者
	ofVec2f localSearch_big_PM(const Coalition &enemy);        // backupC 是调用者,加入概率矩阵

	static ofVec2f getPlaceFromPMatrix();
	
	void writeLog();

	static int logNumber;  // 每一个 Coalition 对象都有一个自己的日志文件，通过这个静态的序号区分日志的名字
	static double target;  // 针对某一 enemy，若能进化出 evulation = target，则认为达到目标了

	static int INDIVIDUAL_SIZE;  // 每一个Coalition的大小

private:
	vector<Tank> m_coalition;
	ofColor  m_color;
	double   m_simpleEvaluate;
	double   m_fitness;
	double   m_weight;
	double   m_abilityDistance;  // 放在这里合理么？Tank 类中也有此属性
	bool     m_isEnemy;
	int      m_stagnate0;        // 代数：对于 SimpleEvaluation 而言，在 Evaluation = 0 时候的停滞的代数
	bool     m_isStagnate;       // 假设开始的时候，都是可能在 Evaluation = 0 停滞的
	ofstream m_logPlace;         // 每一个 Coalition 对象都有自己的日志文件

};
