#include "Population.h"


Population::Population()
{
	PL = 0.9;    // Probability Learning
	LS = 0.9;    // Local Search
	SMALL_NUMBER = 0.1;

	m_update = false;
	m_appearTarget = false;
	m_experimentTimes = 0;

	MAX_UPDATE = 500;
	MAX_EXPERIMENT = 15;
}

Population::Population(size_t size)
{
	m_population.reserve(size);
	m_bestCoalitionIndex.reserve(size);
	m_updateCounter = 0;

	PL = 0.9;    // Probability Learning
	LS = 0.8;    // Local Search
	SMALL_NUMBER = 0.1;

	m_update = false;
	m_appearTarget = false;
	m_experimentTimes = 0;
	
	MAX_UPDATE = 500;
	MAX_EXPERIMENT = 15;
}

void Population::initialize(double pl, double ls, int populationSize, int individualSize)
{
	PL = pl;
	LS = ls;
	m_populationSize = populationSize;
	Coalition::INDIVIDUAL_SIZE = individualSize;

	LOG_PM.open("../log/32^2,pop=32,ind=32_param/log_simpleEvaluate.txt");
	LOG_ANALYSE.open("../log/32^2,pop=32,ind=32_param/log_analyze.txt");

	// 初始化 m_population
	m_population.resize(m_populationSize);                  
	for (int i = 0; i < m_population.size(); ++i)
	{
		Coalition::logNumber++;
		m_population[i].initialize(Coalition::INDIVIDUAL_SIZE);       // 修正BUG：之前 m_population[i] 调用重载的默认构造函数，导致vector大小=0
		m_population[i].setup_CR(Tank::ABILITY_DISTANCE, false, m_enemy);
	}

    // 初始化 概率矩阵
	Global::PROBABILITY_MATRIX.resize(Global::HEIGHT);                     
	vector<double> tmpVector(Global::WIDTH, 0.0);
	for (auto & vec_double : Global::PROBABILITY_MATRIX)
	{
		vec_double = tmpVector;
	}
	updateWeight();   // 初始化的种群 --> 计算其权值
	updatePMatrix();  // 初始化的种群的权值 --> 生成一个初始化的概率矩阵
	m_update = false;
	updateBestCoalitions();                  // 从初始化的种群中获得最好的种群
}

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
			cout << m_experimentTimes << "次实验\n------\n";
			writeLogAnalyse(m_updateCounter);
			resetMe();
			m_updateCounter = 0;                    // 为下一次实验做准备
			m_appearTarget = false;
		}

		if (m_experimentTimes == MAX_EXPERIMENT)  // 准备做 MAX_EXPERIMENT 次实验
		{
			cout << "end of experiment!" << endl;
			LOG_ANALYSE.close();                  // 先关闭，再由另外一个类打开“临界文件”
			AnalyzeLog analyzeLog;                // 曾经 AnalyzeLog 是一个独立的project
			analyzeLog.analyze();                 // 新建了一个filter(筛选器),合并到此项目了

			m_update = 0;
		}

		//if (++m_updateCounter % 10 == 0)  //  每隔更新10代分析平均 Evalation
		//writeLogAnalyse(m_updateCounter);
		updatePopluation();          //  新的全局概率矩阵 --> 更新种群位置
		updateWeight();              //  新的种群位置     --> 更新种群的权值
		updatePMatrix();             //  新的种群权值     --> 更新全局的概率矩阵
		updateBestCoalitions();      //  更新最好的Coalition
		writeLogMatrix(m_updateCounter);
	}

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
			if (ofRandom(0, 1) < PL)      // todo: 这里还有一个&&
			{
				if (ofRandom(0, 1) < LS)
				{// 不做变化，直接复制过去
					constructC.pushBackTank(backupC.getCoalition(i));
				}
				else
				{// local search
					localSearch = true;
					ofVec2f arrayIndex;
					arrayIndex = backupC.localSearch_big_PM(m_enemy);
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
					arrayIndex = Coalition::getPlaceFromPMatrix();  // 问题：可供选择的点越来越少，可能一些很好的点，就“消失”了
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
				if (ofRandom(0, 2) < 1.0)                                 // 给 50% 概率更新
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
		Global::PROBABILITY_MATRIX[x][y] = 0;
	}

	for (const Coalition &c : m_population)
	{
		for (const Tank &tank : c.getCoalition())
		{
			int x = tank.getArrayIndex().x;
			int y = tank.getArrayIndex().y;
			Global::PROBABILITY_MATRIX[x][y] += c.getWeight();
		}
	}
}

void Population::writeLogMatrix(int updateCounter)
{
	LOG_PM << '\n' << "update counter: " << updateCounter << '\n';
	for (int y = Global::HEIGHT - 1; y >= 0; --y)
	{
		for (int x = 0; x < Global::WIDTH - 1; ++x)
		{
			if (isZero(Global::PROBABILITY_MATRIX[x][y]))
			{
				LOG_PM << setprecision(0);
			}
			else
			{
				LOG_PM << setprecision(3);
			}
			LOG_PM << left << setw(7) << Global::PROBABILITY_MATRIX[x][y];  // 历史遗留问题，(y,x) --> (x,y)
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
		if (m_population[i].getSimpleEvaluate() > bestEvaluation)
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

bool Population::isZero(double d)
{
	if (d<Global::EPSILON && d>-Global::EPSILON)
		return true;

	return false;
}

void Population::resetMe()
{
	for (int i = 0; i < m_population.size(); ++i)
	{
		m_population[i].setup_CR(Tank::ABILITY_DISTANCE, false, m_enemy);  // 更新联盟里所有 tank 的位置
		m_population[i].setIsStangate(true);                         // 修复一个BUG
		m_population[i].setStagnate0(0);
	}
	updateWeight();   // 新的位置 --> 新的 weight
	updatePMatrix();
	updateBestCoalitions();
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
		c.resetAtStagnate0(m_enemy, m_updateCounter);
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

