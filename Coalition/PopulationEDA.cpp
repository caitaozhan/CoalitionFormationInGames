#include "PopulationEDA.h"

uniform_real_distribution<double> PopulationEDA::urd_0_1 = uniform_real_distribution<double>(0.0, 1.0);

PopulationEDA::PopulationEDA()
{
	ENEMY_INPUT = string("../sample/1_case_10.txt");                           // enemy���͵ĳ�ʼ�����
	LOG_PM_NAME = string("../log/50^2,pop=50,ind=50/log_simpleEvaluate.txt");  // ���ʾ�����־
	LOG_ANALYSE_INPUT = string("../log/50^2,pop=50,ind=50/log_analyze.txt");   // ����������־����¼ÿһ��ʵ�������ֵ
	LOG_ANALYSE_OUTPUT = string("../log/5^2,pop=50,ind=50/9-1_0.9_0.9.txt");   // �����������е����м�¼
	MAX_UPDATE = 200;
	MAX_EXPERIMENT = 15;

	//SMALL_NUMBER = 0.01;
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
	m_selectNum = m_populationSize * SELECT_RATIO;
	uid_selectPop = uniform_int_distribution<int>(0, m_selectNum - 1);
	
	LOG_PM.open(LOG_PM_NAME);
	LOG_ANALYSE.open(LOG_ANALYSE_INPUT);

	m_enemy.initialize(Coalition::INDIVIDUAL_SIZE);
	m_enemy.setup_file(Tank::ABILITY_DISTANCE, true, ENEMY_INPUT);

	// ��ʼ��������ϵ���������ʾ����У�ÿ��λ�ö�
	m_bRatio = 0.0002;
	m_n = Coalition::INDIVIDUAL_SIZE / 15;
	if (m_n < 2)
		m_n = 2;
	int avalablePlaceInPMatrix;  // ���ʾ�����avalable��λ�ã�����ս���Ĵ�С - ���˵�����
	avalablePlaceInPMatrix = (Global::BF_LR.x - Global::BF_UL.x)*(Global::BF_UL.y - Global::BF_LR.y) - Coalition::INDIVIDUAL_SIZE;
	m_e = (m_populationSize*Coalition::INDIVIDUAL_SIZE) / (avalablePlaceInPMatrix) * m_bRatio;
	m_dimension = Coalition::INDIVIDUAL_SIZE;

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

void PopulationEDA::writeLogMatrix(int updateCounter)
{
	LOG_PM << '\n' << "update counter: " << updateCounter << '\n';
	for (int y = Global::HEIGHT - 1; y >= 0; --y)
	{
		for (int x = 0; x < Global::WIDTH - 1; ++x)
		{
			if (isZero(m_probabilityMatrix[y][x]))
			{
				LOG_PM << setprecision(0);
			}
			else
			{
				LOG_PM << setprecision(3);
			}
			LOG_PM << left << setw(7) << m_probabilityMatrix[y][x];  // (x,y) --> [y][x]
		}
		LOG_PM << '\n';
	}
	LOG_PM << '\n' << "*******************" << endl;
}

/*
@param:  ���µĴ���
@return: ��ʱ����population��ƽ����ֵ
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
	����population�ĳ�Ա����m_bestCoalitionIndex(��������ø����vector�±�)������������˸���
	ֱ�Ӹ��Ĵ�����������
	@param bC, ��������������˸����vector
*/
void PopulationEDA::updateBestCoalitions(vector<Coalition>& bC)
{
	bC.clear();
	bC.reserve(m_bestCoalitionIndex.size());
	for (int & index : m_bestCoalitionIndex)
	{
		bC.push_back(m_population[index]);
	}
}

/*
	����population�ĳ�Ա����m_bestCoalitionIndex(��������ø����vector�±�)������������˸���
	ֱ�Ӹ��Ĵ�����������
	@param bC, ��������������˸����vector
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
		m_population[i].setup_CR(Tank::ABILITY_DISTANCE, false, m_enemy);  // ���³�ʼ����Ⱥ�������и���
	}
	updateEvaluations();
	updateBestCoalitions();
	writeLogMatrix(0);
}

void PopulationEDA::resetExperVariable()
{
	// TODO: �����б䶯
	m_bestEvaluation = -Coalition::INDIVIDUAL_SIZE;
	m_appearTarget = false;
	m_updateCounter = 0;
	m_experimentTimes++;
	Global::dre.seed(m_experimentTimes);
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

void PopulationEDA::updateBestCoalitions()
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
	int newBestEvaluation;
	if (m_population[m_bestCoalitionIndex[0]].getSimpleEvaluate() < 0)  // TODO consolee: a bug, �߾�����ʧ�����ڸ�����
		newBestEvaluation = m_population[m_bestCoalitionIndex[0]].getSimpleEvaluate() - Global::EPSILON;
	else
		newBestEvaluation = m_population[m_bestCoalitionIndex[0]].getSimpleEvaluate() + Global::EPSILON;

	if (newBestEvaluation > m_bestEvaluation)  // �������ֵ�����
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

	if (/*m_appearTarget == true || */m_updateCounter == MAX_UPDATE)         // ��ĳһ�������У���������������
	{
		if (m_appearTarget == false)            // MAX_UPDATE ��֮��û���ҵ� target
		{
			cout << "target not found @" << m_updateCounter << '\n';
			LOG_ANALYSE << "target not found @" << m_updateCounter << '\n';
		}

		cout << m_experimentTimes << "������\n-------\n";
		resetExperVariable();

		if (m_experimentTimes != MAX_EXPERIMENT)
			resetMe();

		if (m_experimentTimes == MAX_EXPERIMENT)  // ׼���� MAX_EXPERIMENT ��ʵ��
		{
			LOG_ANALYSE.close();                  // �ȹرգ���������һ����򿪡��ٽ��ļ���
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
