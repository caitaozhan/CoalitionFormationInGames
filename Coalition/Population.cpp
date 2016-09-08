#include "Population.h"

uniform_real_distribution<double> Population::urd_0_1 = uniform_real_distribution<double>(0.0, 1.0);

Population::Population()
{
	PL = 0.9;    // Probability Learning
	LS = 0.9;    // Local Search
	
	ENEMY_INPUT = string("../sample/8_case_50.txt");                           // enemy阵型的初始化编队
	LOG_PM_NAME = string("../log/50^2,pop=50,ind=50/log_simpleEvaluate.txt");  // 概率矩阵日志
	LOG_ANALYSE_INPUT = string("../log/50^2,pop=50,ind=50/log_analyze.txt");   // 程序运行日志，记录每一次实验的评估值
	LOG_ANALYSE_OUTPUT = string("../log/50^2,pop=50,ind=50/9-1_0.9_0.9.txt");  // 分析程序运行的运行记录
	MAX_UPDATE = 1000;
	MAX_EXPERIMENT = 15;

	SMALL_NUMBER = 0.01;
	m_update = true;
	m_appearTarget = false;
	m_experimentTimes = 0;
	m_updateCounter = 0;
	m_stop = false;
}

void Population::initialize(double pl, double ls, int populationSize)
{
	PL = pl;
	LS = ls;
	m_populationSize = populationSize;

	LOG_PM.open(LOG_PM_NAME);
	LOG_ANALYSE.open(LOG_ANALYSE_INPUT);

	m_enemy.initialize(Coalition::INDIVIDUAL_SIZE);                  // 修正BUG：之前 m_enemy 调用重载的默认构造函数，导致vector大小=0
	m_enemy.setup_file(Tank::ABILITY_DISTANCE, true, ENEMY_INPUT);   // 从文件从读入数据，进行初始化
	//m_enemy.setup_8(Tank::ABILITY_DISTANCE, true, Coalition()); 
	//m_enemy.setup_CR(Tank::ABILITY_DISTANCE, true, Coalition());

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
void Population::update()
{
	m_updateCounter++;

	if (/*m_appearTarget == true || */m_updateCounter == MAX_UPDATE)   // 一次实验结束：进化了 MAX_UPDATE 代
	{
		if (m_appearTarget == false)            // MAX_UPDATE 次之内没有找到 target
		{
			cout << "target not found @" << m_updateCounter << '\n';
			LOG_ANALYSE << "target not found @" << m_updateCounter << '\n';
		}
		
		cout << m_experimentTimes << "次试验\n-------\n";
		resetExperVariable();
		
		if (m_experimentTimes != MAX_EXPERIMENT)
			resetMe();
		
		if (m_experimentTimes == MAX_EXPERIMENT)  // 准备做 MAX_EXPERIMENT 次实验
		{
			LOG_ANALYSE.close();                  // 先关闭，再由另外一个类打开“临界文件”
			AnalyzeLog analyzeLog(LOG_ANALYSE_INPUT, LOG_ANALYSE_OUTPUT);
			analyzeLog.analyze();
			cout << "\nEnd of " << MAX_EXPERIMENT << " times of experiments!" << endl;
			cout << "Let's start over again in 5 seconds" << endl << endl;
			this_thread::sleep_for(chrono::milliseconds(5000));
			resetExperVariable();
			m_experimentTimes = 0;
			Global::dre.seed(0);
			resetMe();
			m_updateCounter++;
		}
	}

	//if(m_experimentTimes == 16)
	//{
	updatePopluation();          //  新的全局概率矩阵 --> 更新种群位置
	updateWeight();              //  新的种群位置     --> 更新种群的权值
	updatePMatrix();             //  新的种群权值     --> 更新全局的概率矩阵
	updateBestCoalitions();      //  更新最好的Coalitions
	writeLogMatrix(m_updateCounter);
	//}
}

void Population::updatePopluation()
{
	for (Coalition &c : m_population)     // c          是论文中的 Si
	{
		Coalition backupC(c);             // backupC    是论文中的 Xi
		Coalition constructC;             // constructC 是论文中的 Xt。 问题：从写代码的角度，这个 Xt 可以不要
		for (int i = 0; i < backupC.getSize(); ++i)
		{
			bool localSearch = false;
			if (urd_0_1(Global::dre) < PL)      // todo: 这里还有一个&&
			{
				if (urd_0_1(Global::dre) < LS)
				{// 不做变化，直接复制过去
					constructC.pushBackTank(backupC.getCoalition(i));
				}
				else
				{// local search
					localSearch = true;
					ofVec2f arrayIndex;
					arrayIndex = backupC.localSearch_big(m_enemy);
					Tank newTank;
					newTank.setup(arrayIndex, Tank::ABILITY_DISTANCE, false);
					constructC.pushBackTank(move(newTank));  // TODO:可以用 emplace_back
				}
			}
			else
			{
				ofVec2f arrayIndex;
				do
				{
					arrayIndex = Coalition::getPlaceFromPMatrix();  // 问题：可供选择的点越来越少，可能一些很好的点，就“消失”了
				} while (c.contain(backupC, arrayIndex) == true || c.contain(m_enemy, arrayIndex) == true);  // 修复一个bug
																											 // 当新选的点，如果是该联盟中已存在的点的话，继续选；如果可选择的点很少的话，循环次数较多
				Tank newTank;
				newTank.setup(arrayIndex, Tank::ABILITY_DISTANCE, false);
				constructC.pushBackTank(move(newTank));
			}
			if (backupC.getCoalition()[i] != constructC.getCoalition()[i])
			{
				backupC.setCoalition(i, constructC.getCoalition(i));
			}

			int evaluateBackupC = Coalition::simpleEvalute(m_enemy, backupC);
			int evaluateC = Coalition::simpleEvalute(m_enemy, c);
			if (evaluateBackupC > evaluateC)  // 如果 backup 比 m_population 中的 c 更好，就更新 c
			{
				c = backupC;
			}
			else if (evaluateBackupC == evaluateC && localSearch == true)  // 相等，但是如果是 local search // TODO: 把localSearch这个限制去掉？
			{
				if (urd_0_1(Global::dre) < 0.5)                            // 给 50% 概率更新
				{
					c = backupC;
				}
			}
		}
	}
}

/*
	更新Propability Matrix (这是个全局变量)
	无法保证每一行的概率和 = 1 --> 没有除以一个“总和”之类的必要
*/
void Population::updatePMatrix()
{
	for (auto &vec_double : Global::PROBABILITY_MATRIX)  // 问题：vector有没有一行代码解决？
	{
		for (double &p : vec_double)
		{
			p = SMALL_NUMBER;		             // 不再清零，初始化一个很小的数
		}
	}

	for (const Tank &t : m_enemy.getCoalition()) // 敌人的地方，还是零
	{
		int x = t.getArrayIndex().x;
		int y = t.getArrayIndex().y;
		Global::PROBABILITY_MATRIX[y][x] = 0;
	}

	for (const Coalition &c : m_population)
	{
		for (const Tank &tank : c.getCoalition())
		{
			int x = tank.getArrayIndex().x;
			int y = tank.getArrayIndex().y;
			Global::PROBABILITY_MATRIX[y][x] += c.getWeight();
		}
	}
	
	// 增加空间，保存概率矩阵的metadata
	int x1 = Global::BF_UL.x, x2 = Global::BF_LR.x;
	int y1 = Global::BF_LR.y, y2 = Global::BF_UL.y;
	Global::TOTAL = 0.0;                                         // 总和
	Global::SUM_OF_ROW = vector<double>(y2 - y1 + 1, 0.0);       // 累积到该行之和
	for (int y = y1; y <= y2; ++y)
	{
		if (y - 1 >= y1)                                         // 先加上前面行的和        
		{
			Global::SUM_OF_ROW[y - y1] = Global::SUM_OF_ROW[y - y1 - 1];    // 修正BUG：下标错误
		}
		for (int x = x1; x <= x2; ++x)
		{
			Global::TOTAL += Global::PROBABILITY_MATRIX[y][x];
			Global::SUM_OF_ROW[y - y1] += Global::PROBABILITY_MATRIX[y][x];  // 修正BUG：下标错误
		}
	}
}

void Population::resetEnemy(string & way)
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

void Population::writeLogMatrix(int updateCounter)
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
int Population::writeLogAnalyse(int updateCounter)
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

void Population::setResetMe(const bool & resetMe)
{
	m_resetMe = resetMe;
}

void Population::setResetEnemy(const bool & resetEnemy)
{
	m_resetEnemy = resetEnemy;
}

void Population::setUpdate(const bool & update)
{
	m_update = update;
}

bool Population::getResetMe()
{
	return m_resetMe;
}

bool Population::getResetEnemy()
{
	return m_resetEnemy;
}

bool Population::getUpdate()
{
	return m_update;
}

bool Population::getStop()
{
	return m_stop;
}

int Population::getSize()
{
	return m_population.size();
}

/*
	get coalitions with the highest evaluations, note that there could be tied best ones
*/
void Population::updateBestCoalitions()
{
	m_bestCoalitionIndex.clear();
	int n = getSize();
	double bestEvaluation = -n;
	for (int i = 0; i < n; ++i)
	{
		if (m_population[i].getSimpleEvaluate() > bestEvaluation + Global::EPSILON)
		{
			m_bestCoalitionIndex.clear();
			bestEvaluation = m_population[i].getSimpleEvaluate();
			m_bestCoalitionIndex.emplace_back(i);
		}
		else if (isZero(m_population[i].getSimpleEvaluate() - bestEvaluation))
		{
			m_bestCoalitionIndex.emplace_back(i);
		}
	}
	int newBestEvaluation = m_population[m_bestCoalitionIndex[0]].getSimpleEvaluate() + Global::EPSILON;
	if (newBestEvaluation > m_bestEvaluation)  // 最佳评估值有提高
	{
		m_bestEvaluation = newBestEvaluation;
		cout << "Best @" << m_updateCounter << "  " << m_bestEvaluation << '\n';
		if (m_appearTarget == false && isZero(m_bestEvaluation - Coalition::target))
		{
			LOG_ANALYSE << "Best @" << m_updateCounter * m_populationSize << "  " << Coalition::target << '\n';
			m_appearTarget = true;
		}
	}
}

/*
	依据population的成员变量m_bestCoalitionIndex(仅保存最好个体的vector下标)，产生最好联盟个体
	直接更改传进来的引用
	@param bC, 保存最新最好联盟个体的vector
*/
void Population::updateBestCoalitions(vector<Coalition> &bC)
{
	bC.clear();
	bC.reserve(m_bestCoalitionIndex.size());
	for (int & index : m_bestCoalitionIndex)
	{
		bC.push_back(m_population[index]);
	}
}

Coalition & Population::getEnemy()
{
	return m_enemy;
}

bool Population::isZero(double d)
{
	if (d<Global::EPSILON && d>-Global::EPSILON)
		return true;

	return false;
}

/*
	重新初始化我方坦克阵型
*/
void Population::resetMe()
{
	for (int i = 0; i < m_population.size(); ++i)
	{
		m_population[i].setup_CR(Tank::ABILITY_DISTANCE, false, m_enemy);  // 重新初始化种群里面所有个体
		//m_population[i].setIsStangate(true);                             // 修复一个BUG
		//m_population[i].setStagnate0(0);
	}
	updateWeight();   // 新的位置 --> 新的 weight
	updatePMatrix();
	updateBestCoalitions();
	writeLogMatrix(0);
}

/*
	重新初始化实验的参数
*/
void Population::resetExperVariable()
{
	m_bestEvaluation = -Coalition::INDIVIDUAL_SIZE;
	m_appearTarget = false;
	m_updateCounter = 0;
	m_experimentTimes++;
	Global::dre.seed(m_experimentTimes);
}


/*
	更新权值的过程：
	1. 更新 评估值
	2. 更新 适应值
	3. 更新 权值
*/
void Population::updateWeight()
{
	for (Coalition &c : m_population)                  // 更新每一个联盟的评估值
	{
		c.setSimpleEvaluate(Coalition::simpleEvalute(m_enemy, c));
		//c.resetAtStagnate0(m_enemy, m_updateCounter);
	}

	int maxE = -Coalition::INDIVIDUAL_SIZE;
	int minE = Coalition::INDIVIDUAL_SIZE;
	for (Coalition &c : m_population)                  // 找到种群里所有联盟的最大最小估值
	{
		if (c.getSimpleEvaluate() > maxE)
			maxE = c.getSimpleEvaluate();

		if (c.getSimpleEvaluate() < minE)
			minE = c.getSimpleEvaluate();
	}
	for (Coalition &c : m_population)                  // 估值 --> 适应值 --> 权值
	{
		c.setFitness(c.calculateFitness(c.getSimpleEvaluate(), maxE, minE));
		c.setWeight(c.calculateWeight(c.getFitness()));
	}
}

