#include "PopulationEDA.h"

PopulationEDA::PopulationEDA()
{
	ENEMY_INPUT = string("../sample/8_case_50.txt");                           // enemy阵型的初始化编队
	LOG_PM_NAME = string("../log/50^2,pop=50,ind=50/log_simpleEvaluate.txt");  // 概率矩阵日志
	LOG_ANALYSE_INPUT = string("../log/50^2,pop=50,ind=50/log_analyze.txt");   // 程序运行日志，记录每一次实验的评估值
	LOG_ANALYSE_OUTPUT = string("../log/5^2,pop=50,ind=50/9-1_0.9_0.9.txt");  // 分析程序运行的运行记录
	MAX_UPDATE = 2000;
	MAX_EXPERIMENT = 15;

	SMALL_NUMBER = 0.01;
	m_update = true;
	m_appearTarget = false;
	m_experimentTimes = 0;
	m_updateCounter = 0;
	m_stop = false;
}

void PopulationEDA::initialize(double selectRatio, int populationSize)
{
	SELECT_RATIO = selectRatio;
	m_populationSize = populationSize;
	
	LOG_PM.open(LOG_PM_NAME);
	LOG_ANALYSE.open(LOG_ANALYSE_INPUT);

	m_enemy.initialize(Coalition::INDIVIDUAL_SIZE);
	m_enemy.setup_file(Tank::ABILITY_DISTANCE, true, ENEMY_INPUT);

	// 初始化 m_population
	m_population.resize(m_populationSize);
	for (int i = 0; i < m_population.size(); ++i)
	{
		m_population[i].initialize(Coalition::INDIVIDUAL_SIZE);       // 修正BUG：之前 m_population[i] 调用重载的默认构造函数，导致vector大小=0
	}
	m_bestCoalitionIndex.emplace_back(0);                             // 就是初始化加入一个元素
	m_bestEvaluation = -Coalition::INDIVIDUAL_SIZE;                   // 2016/8/4日，引入此成员变量，为了修复BUG，在multi-thread版本的updateBestCoalitions中出现的BUG
	// 初始化 概率矩阵
	Global::PROBABILITY_MATRIX.resize(Global::HEIGHT);
	vector<double> tmpVector(Global::WIDTH, 0.0);
	for (auto & vec_double : Global::PROBABILITY_MATRIX)
	{
		vec_double = tmpVector;
	}
	Global::dre.seed(0);
	//resetMe();
}

void PopulationEDA::update()
{
	if (m_update)
	{

	}
}
