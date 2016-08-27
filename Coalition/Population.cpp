#include "Population.h"

string Population::LOG_EXPER_EVALUATE = string("../log/case-2/experiment_");         // 程序运行日志，记录每一次实验的评估值
string Population::LOG_ANALYSE_OUTPUT = string("../log/case-2/result_");             // 分析程序运行的运行记录

Population::Population()
{
	PL = 0.9;    // Probability Learning
	LS = 0.9;    // Local Search
	
	ENEMY_INPUT = string("../sample/2_case_20.txt");                                 // enemy阵型的初始化编队
	LOG_PM_NAME = string("../log/case-2/log_simpleEvaluate.txt");                    // 概率矩阵日志
	MAX_UPDATE = 500;
	MAX_EXPERIMENT = 15;

	SMALL_NUMBER = 0.1;
	m_update = true;
	m_appearTarget = false;
	m_experimentTimes = 0;
	m_updateCounter = 0;
	m_evaluateCounter = 0;
	m_stop = false;
	m_isStagnate = false;
	m_latestPopAvg = -Coalition::INDIVIDUAL_SIZE;

	urd_0_1 = uniform_real_distribution<double>(0.0, 1.0);
}

void Population::initialize(double pl, double ls, int populationSize)
{
	PL = pl;
	LS = ls;
	m_populationSize = populationSize;
	SAMPLE_INTERVAL = populationSize;   // 初始条件下，SAMPLE_INTERVAL 设为种群规模大小
	m_stagnateCriteria = 2*SAMPLE_INTERVAL/m_populationSize;  // 连续若干次评价-->对应的进化代数，种群的fitness无变化，则终止程序

	LOG_PM.open(LOG_PM_NAME);

	m_enemy.initialize(Coalition::INDIVIDUAL_SIZE);                  // 修正BUG：之前 m_enemy 调用重载的默认构造函数，导致vector大小=0
	m_enemy.setup_file(Tank::ABILITY_DISTANCE, true, ENEMY_INPUT);   // 从文件从读入数据，进行初始化
	//m_enemy.setup_8(Tank::ABILITY_DISTANCE, true, Coalition()); 
	//m_enemy.setup_CR(Tank::ABILITY_DISTANCE, true, Coalition());

	// 初始化 m_population
	m_population.resize(m_populationSize);                  
	for (int i = 0; i < m_population.size(); ++i)
	{
		Coalition::logNumber++;  // todo: 可以删掉？
		m_population[i].initialize(Coalition::INDIVIDUAL_SIZE);       // 修正BUG：之前 m_population[i] 调用重载的默认构造函数，导致vector大小=0
	}
	m_bestCoalitionIndex.emplace_back(0);                             // 就是初始化加入一个元素
	m_bestEvaluation = -Coalition::INDIVIDUAL_SIZE;                   // 2016/8/4日，引入此成员变量，为了修复BUG，在multi-thread版本的updateBestCoalitions中出现的BUG
    // 初始化 概率矩阵
	PROBABILITY_MATRIX.resize(Global::HEIGHT);                     
	vector<double> tmpVector(Global::WIDTH, 0.0);
	for (auto & vec_double : PROBABILITY_MATRIX)
	{
		vec_double = tmpVector;
	}
}

/*
	this member function is called in "multi_thread" version
*/
void Population::update()
{
	if (m_update)
	{
		m_updateCounter++;

		if (m_updateCounter == MAX_UPDATE)   // 每一次实验进化 MAX_UPDATE 代
		{
			if (m_appearTarget == false)            // MAX_UPDATE 次之内没有找到 target
			{
				cout << "target not found @" << m_updateCounter << '\n';
				LOG_ANALYSE << "target not found @" << m_updateCounter << '\n';
			}
			m_experimentTimes++;                    // 做完了一次实验
			m_updateCounter = 0;                    // 为下一次实验做准备
			m_appearTarget = false;
			m_bestEvaluation = -Coalition::INDIVIDUAL_SIZE;
			cout << m_experimentTimes << "次实验\n------\n";
			writeLogAnalyse(m_updateCounter);
			resetMe();
			Global::dre.seed(m_experimentTimes);    // 给随机引擎设置种子，从 0 ~ MAX_EXPERIMENT-1
		}

		if (m_experimentTimes == MAX_EXPERIMENT)    // 准备做 MAX_EXPERIMENT 次实验
		{
			cout << "End of " << MAX_EXPERIMENT << " times of experiments!" << endl;
			LOG_ANALYSE.close();                    // 先关闭，再由另外一个类打开“临界文件”
			//AnalyzeLog analyzeLog(LOG_EXPER_EVALUATE, LOG_ANALYSE_OUTPUT);
			//analyzeLog.analyze();
			m_experimentTimes = 0;
			m_update = 0;
			cout << "\n\nLet's start over again~" << endl;
		}

		updatePopluation();          //  新的全局概率矩阵 --> 更新种群位置
		updateWeight();              //  新的种群位置     --> 更新种群的权值
		updatePMatrix();             //  新的种群权值     --> 更新全局的概率矩阵
		updateBestCoalitions();      //  更新最好的Coalitions
		writeLogMatrix(m_updateCounter);
	}
}

/*
	in Console mode, run one single experiment
*/
void Population::run(int ID)
{
	Global::dre.seed(ID);

	resetMe();                       // 重置我方编队

	while (m_appearTarget == false && m_isStagnate == false)  // 试验继续运行的条件：没有找到目标，并且没有停滞状态
	{
		updatePopluation();          //  新的全局概率矩阵 --> 更新种群位置
		updateWeight();              //  新的种群位置     --> 更新种群的权值
		updatePMatrix();             //  新的种群权值     --> 更新全局的概率矩阵
		updateBestCoalitions();      //  更新最好的Coalitions
		writeLogMatrix(m_updateCounter);
		
		if (++m_updateCounter == 10)
		{
			SAMPLE_INTERVAL = 100;
			m_stagnateCriteria = 100;
		}
		//if (m_updateCounter == MAX_UPDATE)  // 退出本次实验-2：当达到实现规定的MAX_UPDATE
			//break;
	}

	if (m_appearTarget == true)
	{                           //当前评估次数          此时整个种群的最优适应值
		LOG_ANALYSE << setw(8) << (m_evaluateCounter / 100 + 1) * 100 << m_population[m_bestCoalitionIndex[0]].getSimpleEvaluate();
		LOG_ANALYSE << endl;  

		////当前评估次数          此时整个种群的最优适应值
		//LOG_ANALYSE << setw(6) << m_evaluateCounter;
		//string str = to_string(m_population[m_bestCoalitionIndex[0]].getSimpleEvaluate()) + "\n";
		//LOG_ANALYSE << str;

		unique_lock<mutex> lock(Global::mtx);
		cout << "Experiment " << setw(2) << ID << " found Global best(" << Coalition::target << ") after "
			<< setw(4) << m_updateCounter << " generations, " << setw(5) << (m_updateCounter - 1)*m_populationSize << " evaluations" << endl;
	}
	else
	{
		unique_lock<mutex> lock(Global::mtx);
		cout << "Experiment " << setw(2) << ID << " not found Global best. ";
		if (m_isStagnate == true)
			cout << m_updateCounter;
		cout << endl;
	}
	resetExperVariables();
}

void Population::resetExperVariables()
{
	m_evaluateCounter = 0;                           // 重新计数评估次数
	m_updateCounter = 0;                             // 重新计数代数
	m_appearTarget = false;                          // 恢复没有找到目标
	m_isStagnate = false;                            // 恢复为“不停滞”状态
	m_bestEvaluation = -Coalition::INDIVIDUAL_SIZE;  // 为下一次实验做准备工作
	SAMPLE_INTERVAL = m_populationSize;              // 重置采样间隔
	LOG_ANALYSE.close();                             // 关闭当前的日子文件
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
					constructC.pushBackTank(newTank);
				}
			}
			else
			{
				ofVec2f arrayIndex;
				do
				{
					arrayIndex = c.getPlaceFromPMatrix(PROBABILITY_MATRIX);  // 问题：可供选择的点越来越少，可能一些很好的点，就“消失”了
				} while (c.contain(backupC, arrayIndex) == true || c.contain(m_enemy, arrayIndex) == true);  // 修复一个bug
																											 // 当新选的点，如果是该联盟中已存在的点的话，继续选；如果可选择的点很少的话，循环次数较多
				Tank newTank;
				newTank.setup(arrayIndex, Tank::ABILITY_DISTANCE, false);
				constructC.pushBackTank(newTank);
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
			else if (evaluateBackupC == evaluateC && localSearch == true)  // 相等，但是如果是 local search
			{
				if (urd_0_1(Global::dre) < 0.5)                            // 给 50% 概率更新
				{
					c = backupC;
				}
			}
		}
		//c.writeLog();
	}
}

/*
	更新Propability Matrix (这是个全局变量)
	无法保证每一行的概率和 = 1 --> 没有除以一个“总和”之类的必要
*/
void Population::updatePMatrix()
{
	for (auto &vec_double : PROBABILITY_MATRIX)  // 问题：vector有没有一行代码解决？
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
		PROBABILITY_MATRIX[x][y] = 0;
	}

	for (const Coalition &c : m_population)
	{
		for (const Tank &tank : c.getCoalition())
		{
			int x = tank.getArrayIndex().x;
			int y = tank.getArrayIndex().y;
			PROBABILITY_MATRIX[x][y] += c.getWeight();
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
			if (isZero(PROBABILITY_MATRIX[x][y]))
			{
				LOG_PM << setprecision(0);
			}
			else
			{
				LOG_PM << setprecision(3);
			}
			LOG_PM << left << setw(7) << PROBABILITY_MATRIX[x][y];  // 历史遗留问题，(y,x) --> (x,y)
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
	double avg = getPopAverageEvaluate();
	
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

/*
	依据实验的ID号，设置本次实验的log文件名
*/
void Population::setLogExperEvaluate(int ID)
{
	string logName = LOG_EXPER_EVALUATE;
	logName += to_string(ID) + ".txt";

	LOG_ANALYSE.open(logName);             // 设置日志文件名字，然后打开日志
}


const string Population::getLogExperEvaluate() const
{
	return LOG_EXPER_EVALUATE;
}

const string Population::getLogAnalyzeOutput() const
{
	return LOG_ANALYSE_OUTPUT;
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
		//cout << "Best @" << m_updateCounter << "  " << m_bestEvaluation << '\n';        // Console版本注释掉
		if (m_appearTarget == false && isZero(m_bestEvaluation - Coalition::target))
		{
			//LOG_ANALYSE << "Best @" << m_updateCounter << "  " << Coalition::target << '\n';  // Console版本注释掉
			m_appearTarget = true;
		}
	}
}

void Population::getBestCoalitions(vector<Coalition>& bC)
{
	vector<Coalition> temp;
	temp.reserve(m_bestCoalitionIndex.size());
	for (int & index : m_bestCoalitionIndex)
	{
		temp.push_back(m_population[index]);
	}

	bC = move(temp);
	return;
}

void Population::getEnemy(Coalition & e)
{
	e = m_enemy;
}

bool Population::isZero(double d)
{
	if (d<Global::EPSILON && d>-Global::EPSILON)
		return true;

	return false;
}

double Population::getPopAverageEvaluate()
{
	double sum = 0.0;
	double avg = 0.0;
	for (const Coalition &c : m_population)
	{
		sum += c.getSimpleEvaluate();
	}
	avg = sum / m_population.size();
	return avg;
}

void Population::resetMe()
{
	for (int i = 0; i < m_population.size(); ++i)
	{
		m_population[i].setup_CR(Tank::ABILITY_DISTANCE, false, m_enemy);  // 更新联盟里所有 tank 的位置
	}
	updateWeight();   // 新的位置 --> 新的 weight
	updatePMatrix();
	updateBestCoalitions();
}


/*
更新权值的过程：
1. 更新 评估值  (顺便输出日志)
2. 更新 适应值
3. 更新 权值
*/
void Population::updateWeight()
{
	for (Coalition &c : m_population)                  // 更新每一个联盟的评估值
	{
		c.setSimpleEvaluate(Coalition::simpleEvalute(m_enemy, c));                  // 评价一次，原始目标值
		
		if (++m_evaluateCounter % SAMPLE_INTERVAL == 0)
		{
			double latestBestEvaluate = m_population[m_bestCoalitionIndex[0]].getSimpleEvaluate();
			LOG_ANALYSE << setw(8) << left << m_evaluateCounter << latestBestEvaluate << endl;  // 评价的次数，此时整个种群的最优适应值
		}
	}

	if (m_updateCounter % m_stagnateCriteria == 0 && m_updateCounter != 0)
	{
		double newPopAvg = getPopAverageEvaluate();
		if (isZero(newPopAvg - m_latestPopAvg))
		{
			m_isStagnate = true;
		}
		else
		{
			m_latestPopAvg = newPopAvg;
		}
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

