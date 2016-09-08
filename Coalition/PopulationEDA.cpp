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
	resetMe();
}

/*
	种群进化一代
*/
void PopulationEDA::update()
{
	if (m_update)
	{
		m_updateCounter++;

		if (m_updateCounter == MAX_UPDATE)         // 在某一次试验中，到达最大进化次数
		{

		}

		if (m_experimentTimes == MAX_EXPERIMENT)   
		{

		}
	}
}

void PopulationEDA::writeLogMatrix(int updateCounter)
{
	LOG_PM << '\n' << "update counter: " << updateCounter << '\n';
	for (int y = Global::HEIGHT - 1; y >= 0; --y)
	{
		for (int x = 0; x < Global::WIDTH - 1; ++x)
		{
			if (isZero(Global::PROBABILITY_MATRIX[y][x]))
			{
				LOG_PM << setprecision(0);
			}
			else
			{
				LOG_PM << setprecision(3);
			}
			LOG_PM << left << setw(7) << Global::PROBABILITY_MATRIX[y][x];  // (x,y) --> [y][x]
		}
		LOG_PM << '\n';
	}
	LOG_PM << '\n' << "*******************" << endl;
}

/*
@param:  更新的次数
@return: 此时整个population的平均估值
*/
int PopulationEDA::writeLoganalyse(int updateCounter)
{
	double sum = 0.0;
	double avg = 0.0;
	for (const Coalition &c : m_population)
	{
		sum += c.getSimpleEvaluate();
	}
	avg = sum / m_population.size();
	LOG_ANALYSE << updateCounter << ": " << avg << "\n\n";
	return avg;
}

void PopulationEDA::setResetMe(const bool & resetMe)
{
	m_resetMe = resetMe;
}

void PopulationEDA::setResetEnemy(const bool & resetEnemy)
{
	m_resetEnemy = resetEnemy;
}

void PopulationEDA::setUpdate(const bool & update)
{
	m_update = update;
}

bool PopulationEDA::getResetMe()
{
	return m_resetMe;
}

bool PopulationEDA::getResetEnemy()
{
	return m_resetEnemy;
}

bool PopulationEDA::getUpdate()
{
	return m_update;
}

bool PopulationEDA::getStop()
{
	return m_stop;
}

int PopulationEDA::getSize()
{
	return m_population.size();
}
/*
依据population的成员变量m_bestCoalitionIndex(仅保存最好个体的vector下标)，产生最好联盟个体
直接更改传进来的引用
@param bC, 保存最新最好联盟个体的vector
*/
void PopulationEDA::getBestCoalitions(vector<Coalition>& bC)
{
	bC.clear();
	bC.reserve(m_bestCoalitionIndex.size());
	for (int & index : m_bestCoalitionIndex)
	{
		bC.push_back(m_population[index]);
	}
}
/*
依据population的成员变量m_bestCoalitionIndex(仅保存最好个体的vector下标)，产生最好联盟个体
直接更改传进来的引用
@param bC, 保存最新最好联盟个体的vector
*/
Coalition & PopulationEDA::getEnemy()
{
	return m_enemy;
}

bool PopulationEDA::isZero(double d)
{
	if (d<Global::EPSILON && d>-Global::EPSILON)
		return true;

	return false;
}

void PopulationEDA::resetEnemy(string & way)
{
	if (way == string("8"))
	{
		m_enemy.setup_8(Tank::ABILITY_DISTANCE, true, Coalition());
	}
	else if (way == string("CR"))
	{
		m_enemy.setup_CR(Tank::ABILITY_DISTANCE, true, Coalition());
	}
}

void PopulationEDA::resetMe()
{
	for (int i = 0; i < m_population.size(); ++i)
	{
		m_population[i].setup_CR(Tank::ABILITY_DISTANCE, false, m_enemy);  // 重新初始化种群里面所有个体
	}
	//updateWeight();   // 新的位置 --> 新的 weight
	//updatePMatrix();
	//updateBestCoalitions();
	//writeLogMatrix(0);

	// TODO: is there any stuff in here?
}

void PopulationEDA::resetExperVariable()
{
	// TODO: 可能有变动
	m_bestEvaluation = -Coalition::INDIVIDUAL_SIZE;
	m_appearTarget = false;
	m_updateCounter = 0;
	cout << m_experimentTimes << "次试验\n-------\n";
	m_experimentTimes++;
	Global::dre.seed(m_experimentTimes);
}

/*
选择若干最好的个体，以此为依据，估计概率分布
*/
void PopulationEDA::select()
{

}

/*
经行概率分布的估计
*/
void PopulationEDA::estimateDistribution()
{
	
}

/*
经行采样，产生下一代种群
*/
void PopulationEDA::sample()
{

}

