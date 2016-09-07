#include "PopulationEDA.h"

PopulationEDA::PopulationEDA()
{
	ENEMY_INPUT = string("../sample/8_case_50.txt");                           // enemy���͵ĳ�ʼ�����
	LOG_PM_NAME = string("../log/50^2,pop=50,ind=50/log_simpleEvaluate.txt");  // ���ʾ�����־
	LOG_ANALYSE_INPUT = string("../log/50^2,pop=50,ind=50/log_analyze.txt");   // ����������־����¼ÿһ��ʵ�������ֵ
	LOG_ANALYSE_OUTPUT = string("../log/5^2,pop=50,ind=50/9-1_0.9_0.9.txt");  // �����������е����м�¼
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

	// ��ʼ�� m_population
	m_population.resize(m_populationSize);
	for (int i = 0; i < m_population.size(); ++i)
	{
		m_population[i].initialize(Coalition::INDIVIDUAL_SIZE);       // ����BUG��֮ǰ m_population[i] �������ص�Ĭ�Ϲ��캯��������vector��С=0
	}
	m_bestCoalitionIndex.emplace_back(0);                             // ���ǳ�ʼ������һ��Ԫ��
	m_bestEvaluation = -Coalition::INDIVIDUAL_SIZE;                   // 2016/8/4�գ�����˳�Ա������Ϊ���޸�BUG����multi-thread�汾��updateBestCoalitions�г��ֵ�BUG
	// ��ʼ�� ���ʾ���
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
