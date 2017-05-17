#include "PopulationMPL.h"

PopulationMPL::PopulationMPL()
{
	PL = 0.5;    // Probability Learning
	LS = 0.9;    // Local Search
	m_e = 0.01;
	m_populationSize = 100;
}

/*
    TODO：从文件输入，得到参数
*/
void PopulationMPL::initialize()
{
	PL = 0.5;
	LS = 1;

	string timeNow = getTimeNow();

	m_fileNameEnemyInput   = string("../sample/1_case_10.txt");                              // TODO: string should be in a file
	m_logNamePM            = string("../log/producer_consumer/logPM_simple_evaluate_.txt");  // all below should be in a file
	m_logNameRunningResult = string("../log/producer_consumer/running_result_.txt");
	m_logNameAnalyseResult = string("../log/producer_consumer/analyse_result_.txt");
	m_logNamePM.insert(m_logNamePM.length() - 4, timeNow);
	m_logNameRunningResult.insert(m_logNameRunningResult.length() - 4, timeNow);
	m_logNameAnalyseResult.insert(m_logNameAnalyseResult.length() - 4, timeNow);
	
	LOG_PM.open(m_logNamePM);
	LOG_ANALYSE.open(m_logNameRunningResult);
	m_enemy.initialize(Coalition::INDIVIDUAL_SIZE);                   // 修正BUG：之前 m_enemy 调用重载的默认构造函数，导致vector大小=0
	m_enemy.setup_file(Tank::ABILITY_DISTANCE, true, m_fileNameEnemyInput);    // 从文件从读入数据，进行初始化
																			   //m_enemy.setup_8(Tank::ABILITY_DISTANCE, true, Coalition()); 
	m_dimension = Coalition::INDIVIDUAL_SIZE;
	m_bRatio = 0.0002;
	int avalablePlaceInPMatrix;
	avalablePlaceInPMatrix = (Global::BF_LR.x - Global::BF_UL.x)*(Global::BF_UL.y - Global::BF_LR.y) - m_enemy.getSize();
	m_populationSize = 2 * sqrt(avalablePlaceInPMatrix * m_dimension);
	m_e = m_bRatio*(m_populationSize*Coalition::INDIVIDUAL_SIZE) / (avalablePlaceInPMatrix);

	// 初始化 m_population
	m_population.resize(m_populationSize);                  
	for (int i = 0; i < m_population.size(); ++i)
	{
		m_population[i].initialize(Coalition::INDIVIDUAL_SIZE);       // 修正BUG：之前 m_population[i] 调用重载的默认构造函数，导致vector大小=0
	}

	m_bestCoalitionIndex.emplace_back(0);                             // 就是初始化加入一个元素
	m_bestEvaluation = -Coalition::INDIVIDUAL_SIZE;                   // 2016/8/4日，引入此成员变量，为了修复BUG，在multi-thread版本的updateBestCoalitions中出现的BUG
    // 初始化 概率矩阵
	m_probabilityMatrix.resize(Global::HEIGHT);                     
	vector<double> tmpVector(Global::WIDTH, 0.0);
	for (auto & vec_double : m_probabilityMatrix)
	{
		vec_double = tmpVector;
	}
	Global::dre.seed(0);
	resetMe();
}

/*
	种群进化一代
*/
void PopulationMPL::update()
{
	m_updateCounter++;

	if (/*m_appearTarget == true || */m_updateCounter == m_maxUpdate)   // 一次实验结束：进化了 MAX_UPDATE 代
	{
		if (m_appearTarget == false)            // MAX_UPDATE 次之内没有找到 target
		{
			cout << "target not found @" << m_updateCounter << '\n';
			LOG_ANALYSE << "target not found @" << m_updateCounter << '\n';
		}
		
		cout << m_experimentTimes << "次试验\n-------\n";
		resetExperVariable();
		
		if (m_experimentTimes != m_maxExperimentTimes)
			resetMe();
		
		if (m_experimentTimes == m_maxExperimentTimes)  // 准备做 m_maxExperimentTimes 次实验
		{
			LOG_ANALYSE.close();                  // 先关闭，再由另外一个类打开“临界文件”
			AnalyzeLog analyzeLog(m_logNameRunningResult, m_logNameAnalyseResult);
			analyzeLog.analyze();
			cout << "\nEnd of " << m_maxExperimentTimes << " times of experiments!" << endl;
			cout << "Let's start over again in 5 seconds" << endl << endl;
			this_thread::sleep_for(chrono::milliseconds(5000));
			resetExperVariable();
			m_experimentTimes = 0;
			Global::dre.seed(0);
			resetMe();
			m_updateCounter++;
		}
	}

	updatePopluation();          //  新的全局概率矩阵 --> 更新种群位置
	updateWeight();              //  新的种群位置     --> 更新种群的权值
	updatePMatrix();             //  新的种群权值     --> 更新全局的概率矩阵
	updateBestCoalitions();      //  更新最好的Coalitions
	writeLogMatrix(m_updateCounter);
}

void PopulationMPL::updatePopluation()
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
					arrayIndex = c.getPlaceFromPMatrix(m_probabilityMatrix, m_sumOfRow, m_PMtotal);  // 问题：可供选择的点越来越少，可能一些很好的点，就“消失”了
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
			else if (evaluateBackupC == evaluateC /*&& localSearch == true*/)  // 相等，但是如果是 local search // 把localSearch这个限制去掉之后，算法的性能大幅度提升
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
void PopulationMPL::updatePMatrix()
{
	for (auto &vec_double : m_probabilityMatrix) // 问题：vector有没有一行代码解决？
	{
		for (double &p : vec_double)
		{
			p = m_e;		                     // 不再清零，初始化一个很小的数
		}
	}

	for (const Tank &t : m_enemy.getCoalition()) // 敌人的地方，还是零
	{
		int x = t.getArrayIndex().x;             // Warning: 这里会有精度损失么的风险。但是做了很多单步调试，没有发现诸如 1.9999999998 --> 1 的例子
		int y = t.getArrayIndex().y;
		m_probabilityMatrix[y][x] = 0;
	}

	for (const Coalition &c : m_population)
	{
		for (const Tank &tank : c.getCoalition())
		{
			int x = tank.getArrayIndex().x;
			int y = tank.getArrayIndex().y;
			m_probabilityMatrix[y][x] += c.getWeight();
		}
	}
	
	// 增加空间，保存概率矩阵的metadata
	int x1 = Global::BF_UL.x, x2 = Global::BF_LR.x;
	int y1 = Global::BF_LR.y, y2 = Global::BF_UL.y;
	m_PMtotal = 0.0;                                         // 总和
	m_sumOfRow = vector<double>(y2 - y1 + 1, 0.0);       // 累积到该行之和
	for (int y = y1; y <= y2; ++y)
	{
		if (y - 1 >= y1)                                         // 先加上前面行的和        
		{
			m_sumOfRow[y - y1] = m_sumOfRow[y - y1 - 1];    // 修正BUG：下标错误
		}
		for (int x = x1; x <= x2; ++x)
		{
			m_PMtotal += m_probabilityMatrix[y][x];
			m_sumOfRow[y - y1] += m_probabilityMatrix[y][x];  // 修正BUG：下标错误
		}
	}
}

/*
	重新初始化我方坦克阵型
*/
void PopulationMPL::resetMe()
{
	for (int i = 0; i < m_population.size(); ++i)
	{
		m_population[i].setup_CR(Tank::ABILITY_DISTANCE, false, m_enemy);  // 重新初始化种群里面所有个体
	}
	updateWeight();   // 新的位置 --> 新的 weight
	updatePMatrix();
	updateBestCoalitions();
	writeLogMatrix(0);
}

/*
	更新权值的过程：
	1. 更新 评估值
	2. 更新 适应值
	3. 更新 权值
*/
void PopulationMPL::updateWeight()
{
	for (Coalition &c : m_population)                  // 更新每一个联盟的评估值
	{
		c.setSimpleEvaluate(Coalition::simpleEvalute(m_enemy, c));
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

