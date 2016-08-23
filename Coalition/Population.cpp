#include "Population.h"

Population::Population()
{
	PL = 0.9;    // Probability Learning
	LS = 0.9;    // Local Search
	
	ENEMY_INPUT = string("../sample/4_case_20.txt");                                 // enemy���͵ĳ�ʼ�����
	LOG_PM_NAME = string("../log/32^2,pop=32,ind=32_param/log_simpleEvaluate.txt");  // ���ʾ�����־
	LOG_ANALYSE_INPUT = string("../log/32^2,pop=32,ind=32_param/log_analyze.txt");   // ����������־����¼ÿһ��ʵ�������ֵ
	LOG_ANALYSE_OUTPUT = string("../log/32^2,pop=32,ind=32_param/8-2_0.9_0.9.txt");  // �����������е����м�¼
	MAX_UPDATE = 500;
	MAX_EXPERIMENT = 15;

	SMALL_NUMBER = 0.1;
	m_update = true;
	m_appearTarget = false;
	m_experimentTimes = 0;
	m_updateCounter = 0;
	m_stop = false;
	
	urd_0_1 = uniform_real_distribution<double>(0.0, 1.0);
}

void Population::initialize(double pl, double ls, int populationSize)
{
	PL = pl;
	LS = ls;
	m_populationSize = populationSize;

	LOG_PM.open(LOG_PM_NAME);
	LOG_ANALYSE.open(LOG_ANALYSE_INPUT);

	m_enemy.initialize(Coalition::INDIVIDUAL_SIZE);                  // ����BUG��֮ǰ m_enemy �������ص�Ĭ�Ϲ��캯��������vector��С=0
	m_enemy.setup_file(Tank::ABILITY_DISTANCE, true, ENEMY_INPUT);   // ���ļ��Ӷ������ݣ����г�ʼ��
	//m_enemy.setup_8(Tank::ABILITY_DISTANCE, true, Coalition()); 
	//m_enemy.setup_CR(Tank::ABILITY_DISTANCE, true, Coalition());

	// ��ʼ�� m_population
	m_population.resize(m_populationSize);                  
	for (int i = 0; i < m_population.size(); ++i)
	{
		Coalition::logNumber++;
		m_population[i].initialize(Coalition::INDIVIDUAL_SIZE);       // ����BUG��֮ǰ m_population[i] �������ص�Ĭ�Ϲ��캯��������vector��С=0
		//m_population[i].setup_CR(Tank::ABILITY_DISTANCE, false, m_enemy);
	}
	m_bestCoalitionIndex.emplace_back(0);                             // ���ǳ�ʼ������һ��Ԫ��
	m_bestEvaluation = -Coalition::INDIVIDUAL_SIZE;                   // 2016/8/4�գ�����˳�Ա������Ϊ���޸�BUG����multi-thread�汾��updateBestCoalitions�г��ֵ�BUG
    // ��ʼ�� ���ʾ���
	PROBABILITY_MATRIX.resize(Global::HEIGHT);                     
	vector<double> tmpVector(Global::WIDTH, 0.0);
	for (auto & vec_double : PROBABILITY_MATRIX)
	{
		vec_double = tmpVector;
	}
	//updateWeight();                 // ��ʼ������Ⱥ --> ������Ȩֵ
	//updatePMatrix();                // ��ʼ������Ⱥ��Ȩֵ --> ����һ����ʼ���ĸ��ʾ���
	//updateBestCoalitions();         // �ӳ�ʼ������Ⱥ�л����õ���Ⱥ
}

void Population::update()
{
	if (m_update)
	{
		m_updateCounter++;

		if (m_updateCounter == MAX_UPDATE)   // ÿһ��ʵ����� MAX_UPDATE ��
		{
			if (m_appearTarget == false)            // MAX_UPDATE ��֮��û���ҵ� target
			{
				cout << "target not found @" << m_updateCounter << '\n';
				LOG_ANALYSE << "target not found @" << m_updateCounter << '\n';
			}
			m_experimentTimes++;                    // ������һ��ʵ��
			m_updateCounter = 0;                    // Ϊ��һ��ʵ����׼��
			m_appearTarget = false;
			m_bestEvaluation = -Coalition::INDIVIDUAL_SIZE;
			cout << m_experimentTimes << "��ʵ��\n------\n";
			writeLogAnalyse(m_updateCounter);
			resetMe();
			Global::dre.seed(m_experimentTimes);    // ����������������ӣ��� 0 ~ MAX_EXPERIMENT-1
		}

		if (m_experimentTimes == MAX_EXPERIMENT)  // ׼���� MAX_EXPERIMENT ��ʵ��
		{
			cout << "End of " << MAX_EXPERIMENT << " times of experiments!" << endl;
			LOG_ANALYSE.close();                  // �ȹرգ���������һ����򿪡��ٽ��ļ���
			AnalyzeLog analyzeLog(LOG_ANALYSE_INPUT, LOG_ANALYSE_OUTPUT);
			analyzeLog.analyze();
			m_experimentTimes = 0;
			m_update = 0;
			cout << "\n\nLet's start over again~" << endl;
		}

		updatePopluation();          //  �µ�ȫ�ָ��ʾ��� --> ������Ⱥλ��
		updateWeight();              //  �µ���Ⱥλ��     --> ������Ⱥ��Ȩֵ
		updatePMatrix();             //  �µ���ȺȨֵ     --> ����ȫ�ֵĸ��ʾ���
		updateBestCoalitions();      //  ������õ�Coalitions
		writeLogMatrix(m_updateCounter);
	}
}

/*
	in Console mode, run one single experiment
*/
void Population::run(int ID)
{
	Global::dre.seed(ID);

	resetMe();                                       // �����ҷ����

	while (m_appearTarget == false)  // �˳�����ʵ��-1�����ҵ�Ŀ���ʱ��
	{
		updatePopluation();          //  �µ�ȫ�ָ��ʾ��� --> ������Ⱥλ��
		updateWeight();              //  �µ���Ⱥλ��     --> ������Ⱥ��Ȩֵ
		updatePMatrix();             //  �µ���ȺȨֵ     --> ����ȫ�ֵĸ��ʾ���
		updateBestCoalitions();      //  ������õ�Coalitions
		writeLogMatrix(m_updateCounter);
		
		m_updateCounter++;
		if (m_updateCounter == MAX_UPDATE)  // �˳�����ʵ��-2�����ﵽʵ�ֹ涨��MAX_UPDATE
			break;
	}

	if (m_appearTarget == true)
	{
		unique_lock<mutex> lock(Global::mtx);
		cout << "Experiment " << ID << " has found Global best(" << Coalition::target << ") after "
			<< m_updateCounter << " generations and " << m_updateCounter*m_populationSize << " evaluations." << endl;
	}
	else
	{
		unique_lock<mutex> lock(Global::mtx);
		cout << "Experiment " << ID << " has not found Global best " << endl;
	}
	m_updateCounter = 0;                             // ���¼���
	m_appearTarget = false;                          // �ָ�û���ҵ�Ŀ��
	m_bestEvaluation = -Coalition::INDIVIDUAL_SIZE;  // Ϊ��һ��ʵ����׼������
}


void Population::updatePopluation()
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
					arrayIndex = backupC.localSearch_big_PM(m_enemy, PROBABILITY_MATRIX);
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
					arrayIndex = c.getPlaceFromPMatrix(PROBABILITY_MATRIX);  // ���⣺�ɹ�ѡ��ĵ�Խ��Խ�٣�����һЩ�ܺõĵ㣬�͡���ʧ����
				} while (c.contain(backupC, arrayIndex) == true || c.contain(m_enemy, arrayIndex) == true);  // �޸�һ��bug
																											 // ����ѡ�ĵ㣬����Ǹ��������Ѵ��ڵĵ�Ļ�������ѡ�������ѡ��ĵ���ٵĻ���ѭ�������϶�
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
			if (evaluateBackupC > evaluateC)  // ��� backup �� m_population �е� c ���ã��͸��� c
			{
				c = backupC;
			}
			else if (evaluateBackupC == evaluateC && localSearch == true)  // ��ȣ���������� local search
			{
				if (urd_0_1(Global::dre) < 0.5)                            // �� 50% ���ʸ���
				{
					c = backupC;
				}
			}
		}
		//c.writeLog();
	}
}

/*
	����Propability Matrix (���Ǹ�ȫ�ֱ���)
	�޷���֤ÿһ�еĸ��ʺ� = 1 --> û�г���һ�����ܺ͡�֮��ı�Ҫ
*/
void Population::updatePMatrix()
{
	for (auto &vec_double : PROBABILITY_MATRIX)  // ���⣺vector��û��һ�д�������
	{
		for (double &p : vec_double)
		{
			p = SMALL_NUMBER;		             // �������㣬��ʼ��һ����С����
		}
	}

	for (const Tank &t : m_enemy.getCoalition()) // ���˵ĵط���������
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
			LOG_PM << left << setw(7) << PROBABILITY_MATRIX[x][y];  // ��ʷ�������⣬(y,x) --> (x,y)
		}
		LOG_PM << '\n';
	}
	LOG_PM << '\n' << "*******************" << endl;
}

/*
@param:  ���µĴ���
@return: ��ʱ����population��ƽ����ֵ
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
	if (newBestEvaluation > m_bestEvaluation)  // �������ֵ�����
	{
		m_bestEvaluation = newBestEvaluation;
		//cout << "Best @" << m_updateCounter << "  " << m_bestEvaluation << '\n';        // Console�汾ע�͵�
		if (m_appearTarget == false && isZero(m_bestEvaluation - Coalition::target))
		{
			//LOG_ANALYSE << "Best @" << m_updateCounter << "  " << Coalition::target << '\n';  // Console�汾ע�͵�
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

void Population::resetMe()
{
	for (int i = 0; i < m_population.size(); ++i)
	{
		m_population[i].setup_CR(Tank::ABILITY_DISTANCE, false, m_enemy);  // �������������� tank ��λ��
	}
	updateWeight();   // �µ�λ�� --> �µ� weight
	updatePMatrix();
	updateBestCoalitions();
}


/*
����Ȩֵ�Ĺ��̣�
1. ���� ����ֵ
2. ���� ��Ӧֵ
3. ���� Ȩֵ
*/
void Population::updateWeight()
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

