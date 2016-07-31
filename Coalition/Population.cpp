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

	// ��ʼ�� m_population
	m_population.resize(m_populationSize);                  
	for (int i = 0; i < m_population.size(); ++i)
	{
		Coalition::logNumber++;
		m_population[i].initialize(Coalition::INDIVIDUAL_SIZE);       // ����BUG��֮ǰ m_population[i] �������ص�Ĭ�Ϲ��캯��������vector��С=0
		m_population[i].setup_CR(Tank::ABILITY_DISTANCE, false, m_enemy);
	}

    // ��ʼ�� ���ʾ���
	Global::PROBABILITY_MATRIX.resize(Global::HEIGHT);                     
	vector<double> tmpVector(Global::WIDTH, 0.0);
	for (auto & vec_double : Global::PROBABILITY_MATRIX)
	{
		vec_double = tmpVector;
	}
	updateWeight();   // ��ʼ������Ⱥ --> ������Ȩֵ
	updatePMatrix();  // ��ʼ������Ⱥ��Ȩֵ --> ����һ����ʼ���ĸ��ʾ���
	m_update = false;
	updateBestCoalitions();                  // �ӳ�ʼ������Ⱥ�л����õ���Ⱥ
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
			cout << m_experimentTimes << "��ʵ��\n------\n";
			writeLogAnalyse(m_updateCounter);
			resetMe();
			m_updateCounter = 0;                    // Ϊ��һ��ʵ����׼��
			m_appearTarget = false;
		}

		if (m_experimentTimes == MAX_EXPERIMENT)  // ׼���� MAX_EXPERIMENT ��ʵ��
		{
			cout << "end of experiment!" << endl;
			LOG_ANALYSE.close();                  // �ȹرգ���������һ����򿪡��ٽ��ļ���
			AnalyzeLog analyzeLog;                // ���� AnalyzeLog ��һ��������project
			analyzeLog.analyze();                 // �½���һ��filter(ɸѡ��),�ϲ�������Ŀ��

			m_update = 0;
		}

		//if (++m_updateCounter % 10 == 0)  //  ÿ������10������ƽ�� Evalation
		//writeLogAnalyse(m_updateCounter);
		updatePopluation();          //  �µ�ȫ�ָ��ʾ��� --> ������Ⱥλ��
		updateWeight();              //  �µ���Ⱥλ��     --> ������Ⱥ��Ȩֵ
		updatePMatrix();             //  �µ���ȺȨֵ     --> ����ȫ�ֵĸ��ʾ���
		updateBestCoalitions();      //  ������õ�Coalition
		writeLogMatrix(m_updateCounter);
	}

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
			if (ofRandom(0, 1) < PL)      // todo: ���ﻹ��һ��&&
			{
				if (ofRandom(0, 1) < LS)
				{// �����仯��ֱ�Ӹ��ƹ�ȥ
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
					arrayIndex = Coalition::getPlaceFromPMatrix();  // ���⣺�ɹ�ѡ��ĵ�Խ��Խ�٣�����һЩ�ܺõĵ㣬�͡���ʧ����
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
				if (ofRandom(0, 2) < 1.0)                                 // �� 50% ���ʸ���
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
	for (auto &vec_double : Global::PROBABILITY_MATRIX)  // ���⣺vector��û��һ�д�������
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
			LOG_PM << left << setw(7) << Global::PROBABILITY_MATRIX[x][y];  // ��ʷ�������⣬(y,x) --> (x,y)
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
		m_population[i].setup_CR(Tank::ABILITY_DISTANCE, false, m_enemy);  // �������������� tank ��λ��
		m_population[i].setIsStangate(true);                         // �޸�һ��BUG
		m_population[i].setStagnate0(0);
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
		c.resetAtStagnate0(m_enemy, m_updateCounter);
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

