#include "PopulationMPL.h"

PopulationMPL::PopulationMPL()
{
	PL = 0.5;    // Probability Learning
	LS = 0.9;    // Local Search
	m_e = 0.01;
	m_populationSize = 100;
}

/*
    TODO�����ļ����룬�õ�����
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
	m_enemy.initialize(Coalition::INDIVIDUAL_SIZE);                   // ����BUG��֮ǰ m_enemy �������ص�Ĭ�Ϲ��캯��������vector��С=0
	m_enemy.setup_file(Tank::ABILITY_DISTANCE, true, m_fileNameEnemyInput);    // ���ļ��Ӷ������ݣ����г�ʼ��
																			   //m_enemy.setup_8(Tank::ABILITY_DISTANCE, true, Coalition()); 
	m_dimension = Coalition::INDIVIDUAL_SIZE;
	m_bRatio = 0.0002;
	int avalablePlaceInPMatrix;
	avalablePlaceInPMatrix = (Global::BF_LR.x - Global::BF_UL.x)*(Global::BF_UL.y - Global::BF_LR.y) - m_enemy.getSize();
	m_populationSize = 2 * sqrt(avalablePlaceInPMatrix * m_dimension);
	m_e = m_bRatio*(m_populationSize*Coalition::INDIVIDUAL_SIZE) / (avalablePlaceInPMatrix);

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

/*
	��Ⱥ����һ��
*/
void PopulationMPL::update()
{
	m_updateCounter++;

	if (/*m_appearTarget == true || */m_updateCounter == m_maxUpdate)   // һ��ʵ������������� MAX_UPDATE ��
	{
		if (m_appearTarget == false)            // MAX_UPDATE ��֮��û���ҵ� target
		{
			cout << "target not found @" << m_updateCounter << '\n';
			LOG_ANALYSE << "target not found @" << m_updateCounter << '\n';
		}
		
		cout << m_experimentTimes << "������\n-------\n";
		resetExperVariable();
		
		if (m_experimentTimes != m_maxExperimentTimes)
			resetMe();
		
		if (m_experimentTimes == m_maxExperimentTimes)  // ׼���� m_maxExperimentTimes ��ʵ��
		{
			LOG_ANALYSE.close();                  // �ȹرգ���������һ����򿪡��ٽ��ļ���
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

	updatePopluation();          //  �µ�ȫ�ָ��ʾ��� --> ������Ⱥλ��
	updateWeight();              //  �µ���Ⱥλ��     --> ������Ⱥ��Ȩֵ
	updatePMatrix();             //  �µ���ȺȨֵ     --> ����ȫ�ֵĸ��ʾ���
	updateBestCoalitions();      //  ������õ�Coalitions
	writeLogMatrix(m_updateCounter);
}

void PopulationMPL::updatePopluation()
{
	for (Coalition &c : m_population)     // c          �������е� Si
	{
		Coalition backupC(c);             // backupC    �������е� Xi
		Coalition constructC;             // constructC �������е� Xt�� ���⣺��д����ĽǶȣ���� Xt ���Բ�Ҫ
		for (int i = 0; i < backupC.getSize(); ++i)
		{
			bool localSearch = false;
			if (urd_0_1(Global::dre) < PL)      // todo: ���ﻹ��һ��&&
			{
				if (urd_0_1(Global::dre) < LS)
				{// �����仯��ֱ�Ӹ��ƹ�ȥ
					constructC.pushBackTank(backupC.getCoalition(i));
				}
				else
				{// local search
					localSearch = true;
					ofVec2f arrayIndex;
					arrayIndex = backupC.localSearch_big(m_enemy);
					Tank newTank;
					newTank.setup(arrayIndex, Tank::ABILITY_DISTANCE, false);
					constructC.pushBackTank(move(newTank));  // TODO:������ emplace_back
				}
			}
			else
			{
				ofVec2f arrayIndex;
				do
				{
					arrayIndex = c.getPlaceFromPMatrix(m_probabilityMatrix, m_sumOfRow, m_PMtotal);  // ���⣺�ɹ�ѡ��ĵ�Խ��Խ�٣�����һЩ�ܺõĵ㣬�͡���ʧ����
				} while (c.contain(backupC, arrayIndex) == true || c.contain(m_enemy, arrayIndex) == true);  // �޸�һ��bug
																											 // ����ѡ�ĵ㣬����Ǹ��������Ѵ��ڵĵ�Ļ�������ѡ�������ѡ��ĵ���ٵĻ���ѭ�������϶�
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
			if (evaluateBackupC > evaluateC)  // ��� backup �� m_population �е� c ���ã��͸��� c
			{
				c = backupC;
			}
			else if (evaluateBackupC == evaluateC /*&& localSearch == true*/)  // ��ȣ���������� local search // ��localSearch�������ȥ��֮���㷨�����ܴ��������
			{
				if (urd_0_1(Global::dre) < 0.5)                            // �� 50% ���ʸ���
				{
					c = backupC;
				}
			}
		}
	}
}

/*
	����Propability Matrix (���Ǹ�ȫ�ֱ���)
	�޷���֤ÿһ�еĸ��ʺ� = 1 --> û�г���һ�����ܺ͡�֮��ı�Ҫ
*/
void PopulationMPL::updatePMatrix()
{
	for (auto &vec_double : m_probabilityMatrix) // ���⣺vector��û��һ�д�������
	{
		for (double &p : vec_double)
		{
			p = m_e;		                     // �������㣬��ʼ��һ����С����
		}
	}

	for (const Tank &t : m_enemy.getCoalition()) // ���˵ĵط���������
	{
		int x = t.getArrayIndex().x;             // Warning: ������о�����ʧô�ķ��ա��������˺ܶ൥�����ԣ�û�з������� 1.9999999998 --> 1 ������
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
	���³�ʼ���ҷ�̹������
*/
void PopulationMPL::resetMe()
{
	for (int i = 0; i < m_population.size(); ++i)
	{
		m_population[i].setup_CR(Tank::ABILITY_DISTANCE, false, m_enemy);  // ���³�ʼ����Ⱥ�������и���
	}
	updateWeight();   // �µ�λ�� --> �µ� weight
	updatePMatrix();
	updateBestCoalitions();
	writeLogMatrix(0);
}

/*
	����Ȩֵ�Ĺ��̣�
	1. ���� ����ֵ
	2. ���� ��Ӧֵ
	3. ���� Ȩֵ
*/
void PopulationMPL::updateWeight()
{
	for (Coalition &c : m_population)                  // ����ÿһ�����˵�����ֵ
	{
		c.setSimpleEvaluate(Coalition::simpleEvalute(m_enemy, c));
	}

	int maxE = -Coalition::INDIVIDUAL_SIZE;
	int minE = Coalition::INDIVIDUAL_SIZE;
	for (Coalition &c : m_population)                  // �ҵ���Ⱥ���������˵������С��ֵ
	{
		if (c.getSimpleEvaluate() > maxE)
			maxE = c.getSimpleEvaluate();

		if (c.getSimpleEvaluate() < minE)
			minE = c.getSimpleEvaluate();
	}
	for (Coalition &c : m_population)                  // ��ֵ --> ��Ӧֵ --> Ȩֵ
	{
		c.setFitness(c.calculateFitness(c.getSimpleEvaluate(), maxE, minE));
		c.setWeight(c.calculateWeight(c.getFitness()));
	}
}

