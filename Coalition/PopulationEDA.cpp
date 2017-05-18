#include "PopulationEDA.h"

PopulationEDA::PopulationEDA()
{
	m_update          = true;
	m_appearTarget    = false;
	m_stop            = false;
	m_experimentTimes = 0;
	m_updateCounter   = 0;
}

void PopulationEDA::initialize()
{
	m_selectRatio = 0.8;

	string timeNow = getTimeNow();

	m_fileNameEnemyInput = string("../sample/4_case_20.txt");                              // TODO: string should be in a file
	m_logNamePM = string("../log/producer-consumer/logPM-simple-evaluate-.txt");  // all below should be in a file
	m_logNameRunningResult = string("../log/producer-consumer/running-result-.txt");
	m_logNameAnalyseResult = string("../log/producer-consumer/analyse-result-.txt");
	m_logNamePM.insert(m_logNamePM.length() - 4, timeNow);
	m_logNameRunningResult.insert(m_logNameRunningResult.length() - 4, timeNow);
	m_logNameAnalyseResult.insert(m_logNameAnalyseResult.length() - 4, timeNow);

	// 初始化 enemy
	m_enemy.initialize(Coalition::INDIVIDUAL_SIZE);
	m_enemy.setup_file(Tank::ABILITY_DISTANCE, true, m_fileNameEnemyInput);
	m_dimension = Coalition::INDIVIDUAL_SIZE;

	// 种群大小，初始化“变异系数”，问题的维度（个体里面坦克的数量）
	m_bRatio = 0.0002;
	m_n = Coalition::INDIVIDUAL_SIZE / 15;
	if (m_n < 2)
		m_n = 2;
	int avalablePlaceInPMatrix;  // 概率矩阵中avalable的位置，等于战场的大小 - 敌人的数量
	avalablePlaceInPMatrix = (Global::BF_LR.x - Global::BF_UL.x)*(Global::BF_UL.y - Global::BF_LR.y) - Coalition::INDIVIDUAL_SIZE;
	m_populationSize = 5 * sqrt(avalablePlaceInPMatrix * m_dimension);
	//m_populationSize = populationSize;  // 不再根据经验，而是依据一个比例
	m_e = (m_populationSize*Coalition::INDIVIDUAL_SIZE) / (avalablePlaceInPMatrix)* m_bRatio;

	m_selectNum = m_populationSize * m_selectRatio;
	uid_selectPop = uniform_int_distribution<int>(0, m_selectNum - 1);
	
	// 初始化日志文件名
	m_logPM.open(m_logNamePM);
	m_logAnalyse.open(m_logNameRunningResult);

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

void PopulationEDA::resetMe()
{
	for (int i = 0; i < m_population.size(); ++i)
	{
		m_population[i].setup_CR(Tank::ABILITY_DISTANCE, false, m_enemy);  // 重新初始化种群里面所有个体
	}
	
	updateEvaluations();
	updateBestCoalitions();
	selectIndividuals();
	estimateDistribution();
	writeLogMatrix(0);
}

/*
	选择若干最好的个体，以此为依据，估计概率分布
*/
void PopulationEDA::selectIndividuals()
{
	m_selectedPop.clear();                                                    // 先清空
	m_selectedPop.resize(m_selectNum);
	sort(m_population.begin(), m_population.end(), Coalition::decrease); // 考虑到在基本有序或者逆序的时候（可能会有很多个体fitness相等），快排的时间复杂度接近 n^2

	for (int i = 0; i < m_selectNum; ++i)
	{
		m_selectedPop[i] = m_population[i];
	}
}

/*
	进行概率分布的估计
*/
void PopulationEDA::estimateDistribution()
{
	for (auto & vec_double : m_probabilityMatrix)
	{
		for (double & p : vec_double)
		{
			p = m_e;
		}
	}

	for (const Tank & t : m_enemy.getCoalition())
	{
		int x = t.getArrayIndex().x;
		int y = t.getArrayIndex().y;
		m_probabilityMatrix[y][x] = 0;
	}

	for (const Coalition & c : m_selectedPop)   // 从选择的部分种群个体里面估计出概率分布
	{
		for (const Tank & t : c.getCoalition())
		{
			int x = t.getArrayIndex().x;
			int y = t.getArrayIndex().y;
			m_probabilityMatrix[y][x] += 1;
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
	进行采样，产生下一代种群中的一个个体
	sample方法参考了with template方法
*/
void PopulationEDA::sampleOneSolution(int index)
{
	Coalition newIndividual;                // individual即是一个联盟，一个solution
	newIndividual.initialize(m_dimension, Tank::ABILITY_DISTANCE, false);
	int pos = uid_selectPop(Global::dre);   // 随机选择one个体做template，在这个template的基础上进化
	vector<int> randomIndex = generateRandomIndex();
	vector<int> allCutIndex(m_dimension);
	set<int> cuttedIndex;
	
	for (int i = 0; i < m_dimension; ++i)
		allCutIndex[i] = i;
	for (int i = 0; i < m_n; ++i)
	{
		uniform_int_distribution<int> uid(0, m_dimension - 1 - i);
		int index = uid(Global::dre);
		cuttedIndex.insert(allCutIndex[index]);
		allCutIndex[index] = allCutIndex[m_dimension - 1 - i];
	}
	int lengthWT = m_dimension - (*(--cuttedIndex.end()) - *cuttedIndex.begin());

	int p = 0;
	while(p < m_dimension - lengthWT)
	{
		newIndividual.pushBackTank(m_selectedPop[pos].getCoalition(randomIndex[p++]));
	}
	while(p++ < m_dimension)
	{
		ofVec2f arrayIndex;
		do
		{
			arrayIndex = newIndividual.getPlaceFromPMatrix(m_probabilityMatrix, m_sumOfRow, m_PMtotal);  // 问题：可供选择的点越来越少，可能一些很好的点，就“消失”了
		} while (newIndividual.contain(newIndividual, arrayIndex) == true || newIndividual.contain(m_enemy, arrayIndex) == true);  // 修复一个bug
		Tank newTank;
		newTank.setup(arrayIndex, Tank::ABILITY_DISTANCE, false);
		newIndividual.pushBackTank(move(newTank));
	}
	newIndividual.setSimpleEvaluate(Coalition::simpleEvalute(m_enemy, newIndividual));
	int evaluateNew = newIndividual.getSimpleEvaluate();
	int evaluateOld = m_selectedPop[pos].getSimpleEvaluate();
	if (evaluateNew > evaluateOld)
	{
		m_selectedPop[pos] = newIndividual;
		m_population[index] = newIndividual;
	}
	else if (evaluateNew == evaluateOld)
	{
		if (urd_0_1(Global::dre) < 0.5)
		{
			m_selectedPop[pos] = newIndividual;
			m_population[index] = newIndividual;
		}
		else
		{
			m_population[index] = m_selectedPop[pos];
		}
	}
	else  // evaluateNew < evaluateOld
	{
		m_population[index] = m_selectedPop[pos];
	}
}

void PopulationEDA::updateEvaluations()
{
	for (Coalition & c : m_population)
	{
		c.setSimpleEvaluate(Coalition::simpleEvalute(m_enemy, c));
	}
}

vector<int> PopulationEDA::generateRandomIndex()
{
	vector<int> temp(m_dimension);
	vector<int> randomIndex(m_dimension);
	
	for (int i = 0; i < temp.size(); ++i)
		temp[i] = i;

	for (int i = 0; i < randomIndex.size(); ++i)
	{
		uniform_int_distribution<int> uid(0, m_dimension - 1 - i);
		int pos = uid(Global::dre);
		randomIndex[i] = temp[pos];
		temp[pos] = temp[m_dimension - 1 - i];
	}

	return randomIndex;
}

/*
	种群进化一代
*/
void PopulationEDA::update()
{
	m_updateCounter++;

	if (m_appearTarget == true || m_updateCounter == m_maxUpdate)         // 在某一次试验中，到达最大进化次数
	{
		if (m_appearTarget == false)            // MAX_UPDATE 次之内没有找到 target
		{
			cout << "target not found @" << m_updateCounter << '\n';
			m_logAnalyse << "target not found @" << m_updateCounter << '\n';
		}

		cout << m_experimentTimes << "次试验\n-------\n";
		resetExperVariable();

		if (m_experimentTimes != m_maxExperimentTimes)
			resetMe();

		if (m_experimentTimes == m_maxExperimentTimes)  // 准备做 MAX_EXPERIMENT 次实验
		{
			m_logAnalyse.close();                  // 先关闭，再由另外一个类打开“临界文件”
			AnalyzeLog analyzeLog(m_logNameRunningResult, m_logNameAnalyseResult);
			analyzeLog.analyze();
			cout << "\nEnd of " << m_maxExperimentTimes << " times of experiments!" << endl;
			cout << "Let's start over again in 10 seconds" << endl << endl;
			this_thread::sleep_for(chrono::milliseconds(10000));
			resetExperVariable();
			m_experimentTimes = 0;
			Global::dre.seed(0);
			resetMe();
			m_updateCounter++;
		}
	}

	selectIndividuals();

	estimateDistribution();

	for (int i = 0; i < m_populationSize; ++i)
	{
		sampleOneSolution(i);
	}

	updateEvaluations();
	updateBestCoalitions();
	writeLogMatrix(m_updateCounter);
}
