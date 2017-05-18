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

	// ��ʼ�� enemy
	m_enemy.initialize(Coalition::INDIVIDUAL_SIZE);
	m_enemy.setup_file(Tank::ABILITY_DISTANCE, true, m_fileNameEnemyInput);
	m_dimension = Coalition::INDIVIDUAL_SIZE;

	// ��Ⱥ��С����ʼ��������ϵ�����������ά�ȣ���������̹�˵�������
	m_bRatio = 0.0002;
	m_n = Coalition::INDIVIDUAL_SIZE / 15;
	if (m_n < 2)
		m_n = 2;
	int avalablePlaceInPMatrix;  // ���ʾ�����avalable��λ�ã�����ս���Ĵ�С - ���˵�����
	avalablePlaceInPMatrix = (Global::BF_LR.x - Global::BF_UL.x)*(Global::BF_UL.y - Global::BF_LR.y) - Coalition::INDIVIDUAL_SIZE;
	m_populationSize = 5 * sqrt(avalablePlaceInPMatrix * m_dimension);
	//m_populationSize = populationSize;  // ���ٸ��ݾ��飬��������һ������
	m_e = (m_populationSize*Coalition::INDIVIDUAL_SIZE) / (avalablePlaceInPMatrix)* m_bRatio;

	m_selectNum = m_populationSize * m_selectRatio;
	uid_selectPop = uniform_int_distribution<int>(0, m_selectNum - 1);
	
	// ��ʼ����־�ļ���
	m_logPM.open(m_logNamePM);
	m_logAnalyse.open(m_logNameRunningResult);

	// ��ʼ�� m_population
	m_population.resize(m_populationSize);
	for (int i = 0; i < m_population.size(); ++i)
	{
		m_population[i].initialize(Coalition::INDIVIDUAL_SIZE);       // ����BUG��֮ǰ m_population[i] �������ص�Ĭ�Ϲ��캯��������vector��С=0
	}
	m_bestCoalitionIndex.emplace_back(0);                             // ���ǳ�ʼ������һ��Ԫ��
	m_bestEvaluation = -Coalition::INDIVIDUAL_SIZE;                   // 2016/8/4�գ�����˳�Ա������Ϊ���޸�BUG����multi-thread�汾��updateBestCoalitions�г��ֵ�BUG
	// ��ʼ�� ���ʾ���
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
		m_population[i].setup_CR(Tank::ABILITY_DISTANCE, false, m_enemy);  // ���³�ʼ����Ⱥ�������и���
	}
	
	updateEvaluations();
	updateBestCoalitions();
	selectIndividuals();
	estimateDistribution();
	writeLogMatrix(0);
}

/*
	ѡ��������õĸ��壬�Դ�Ϊ���ݣ����Ƹ��ʷֲ�
*/
void PopulationEDA::selectIndividuals()
{
	m_selectedPop.clear();                                                    // �����
	m_selectedPop.resize(m_selectNum);
	sort(m_population.begin(), m_population.end(), Coalition::decrease); // ���ǵ��ڻ���������������ʱ�򣨿��ܻ��кܶ����fitness��ȣ������ŵ�ʱ�临�ӶȽӽ� n^2

	for (int i = 0; i < m_selectNum; ++i)
	{
		m_selectedPop[i] = m_population[i];
	}
}

/*
	���и��ʷֲ��Ĺ���
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

	for (const Coalition & c : m_selectedPop)   // ��ѡ��Ĳ�����Ⱥ����������Ƴ����ʷֲ�
	{
		for (const Tank & t : c.getCoalition())
		{
			int x = t.getArrayIndex().x;
			int y = t.getArrayIndex().y;
			m_probabilityMatrix[y][x] += 1;
		}
	}

	// ���ӿռ䣬������ʾ����metadata
	int x1 = Global::BF_UL.x, x2 = Global::BF_LR.x;
	int y1 = Global::BF_LR.y, y2 = Global::BF_UL.y;
	m_PMtotal = 0.0;                                         // �ܺ�
	m_sumOfRow = vector<double>(y2 - y1 + 1, 0.0);       // �ۻ�������֮��
	for (int y = y1; y <= y2; ++y)
	{
		if (y - 1 >= y1)                                         // �ȼ���ǰ���еĺ�        
		{
			m_sumOfRow[y - y1] = m_sumOfRow[y - y1 - 1];    // ����BUG���±����
		}
		for (int x = x1; x <= x2; ++x)
		{
			m_PMtotal += m_probabilityMatrix[y][x];
			m_sumOfRow[y - y1] += m_probabilityMatrix[y][x];  // ����BUG���±����
		}
	}
}

/*
	���в�����������һ����Ⱥ�е�һ������
	sample�����ο���with template����
*/
void PopulationEDA::sampleOneSolution(int index)
{
	Coalition newIndividual;                // individual����һ�����ˣ�һ��solution
	newIndividual.initialize(m_dimension, Tank::ABILITY_DISTANCE, false);
	int pos = uid_selectPop(Global::dre);   // ���ѡ��one������template�������template�Ļ����Ͻ���
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
			arrayIndex = newIndividual.getPlaceFromPMatrix(m_probabilityMatrix, m_sumOfRow, m_PMtotal);  // ���⣺�ɹ�ѡ��ĵ�Խ��Խ�٣�����һЩ�ܺõĵ㣬�͡���ʧ����
		} while (newIndividual.contain(newIndividual, arrayIndex) == true || newIndividual.contain(m_enemy, arrayIndex) == true);  // �޸�һ��bug
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
	��Ⱥ����һ��
*/
void PopulationEDA::update()
{
	m_updateCounter++;

	if (m_appearTarget == true || m_updateCounter == m_maxUpdate)         // ��ĳһ�������У���������������
	{
		if (m_appearTarget == false)            // MAX_UPDATE ��֮��û���ҵ� target
		{
			cout << "target not found @" << m_updateCounter << '\n';
			m_logAnalyse << "target not found @" << m_updateCounter << '\n';
		}

		cout << m_experimentTimes << "������\n-------\n";
		resetExperVariable();

		if (m_experimentTimes != m_maxExperimentTimes)
			resetMe();

		if (m_experimentTimes == m_maxExperimentTimes)  // ׼���� MAX_EXPERIMENT ��ʵ��
		{
			m_logAnalyse.close();                  // �ȹرգ���������һ����򿪡��ٽ��ļ���
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
