#include "PopulationBase.h"

uniform_real_distribution<double> PopulationBase::urd_0_1 = uniform_real_distribution<double>(0.0, 1.0);

PopulationBase::PopulationBase()
{
	m_maxUpdate          = 100;
	m_maxExperimentTimes = 1;
	m_PMtotal            = 0;
	m_experimentTimes    = 0;
	m_updateCounter      = 0;

	m_update             = true;
	m_resetMe            = false;
	m_resetEnemy         = false;
	m_appearTarget       = false;
	m_stop               = false;

	m_apriori.setParam(0.3, 0.8);
}

void PopulationBase::writeLogMatrix(int updateCounter)
{
	m_logPM << '\n' << "update counter: " << updateCounter << '\n';
	for (int y = Global::HEIGHT - 1; y >= 0; --y)
	{
		for (int x = 0; x < Global::WIDTH - 1; ++x)
		{
			if (isZero(m_probabilityMatrix[y][x]))
			{
				m_logPM << setprecision(0);
			}
			else
			{
				m_logPM << setprecision(3);
			}
			m_logPM << left << setw(7) << m_probabilityMatrix[y][x];  // (x,y) --> [y][x]
		}
		m_logPM << '\n';
	}
	m_logPM << '\n' << "*******************" << endl;
}

/*
    @param:  ���µĴ���
    @return: ��ʱ����population��ƽ����ֵ
*/
int PopulationBase::writeLogPopAverage(int updateCounter)
{
	double sum = 0.0;
	double avg = 0.0;
	for (const Coalition &c : m_population)
	{
		sum += c.getSimpleEvaluate();
	}
	avg = sum / m_population.size();
	m_logAnalyse << updateCounter << ": " << avg << "\n\n";
	return avg;
}

/*
    �� population ת���� Apriori ��Ӧ�����ݼ�
	one coalition ��Ӧ one transaction
*/
void PopulationBase::population2transaction(const vector<Coalition>& population, vector<vector<ItemSet>>& transaction)
{
	vector<ItemSet> oneTransaction;
	oneTransaction.reserve(Coalition::INDIVIDUAL_SIZE);
	transaction.reserve(population.size());

	for (const Coalition & c: population)
	{
		vector<Tank> tanks = c.getCoalition();
		for (const Tank & t : tanks)
		{
			ofVec2f arrayIndex = t.getArrayIndex();
			Item item(static_cast<int>(arrayIndex.x + Global::EPSILON), static_cast<int>(arrayIndex.y + Global::EPSILON));
			//ItemSet oneItemSet;
			//oneItemSet.insert(item);
			//oneTransaction.emplace_back(move(oneItemSet)); // TODO: reserve Ȼ�� push_back �� emplace_back ��һЩ������ resize Ȼ�� ��ֵ move ��һЩ
			oneTransaction.emplace_back(item);
		}
		transaction.emplace_back(move(oneTransaction));
	}
}

void PopulationBase::printTransaction(vector<vector<ItemSet>>& transactions, int transactionID)
{
	m_logTransaction << endl << "Transaction #" << transactionID << endl;
	for (vector<ItemSet> & oneTransaction : transactions)
	{
		for (ItemSet & oneItemSet : oneTransaction)
		{
			m_logTransaction << oneItemSet;
		}
		m_logTransaction << endl;
	}
	m_logTransaction << endl;
}

void PopulationBase::takeActionToKnowledge(const map<pair<ItemSet, ItemSet>, double> & associateRules)
{
	if (associateRules.size() == 0)
		return;

	Coalition newCoalition;
	for (Coalition & c : m_population)
	{
		pair<ItemSet, ItemSet> matchedRule = matchRules(c, associateRules);  // ���û��ƥ��Ļ���Ӧ���� (-1, -1) --> (-1, -1)
		if (matchedRule.first.size() == 0)
		{
			continue;
		}
		ItemSet moveDestination, moveSource;
		moveDestination = matchedRule.second - c.toItemSet();
		moveSource = findSource(moveDestination.size(), c, matchedRule, associateRules);
		
		newCoalition = c;
		newCoalition.actionMove(moveSource, moveDestination);
		int evaluateOld = c.getSimpleEvaluate();
		int evaluateNew = Coalition::simpleEvalute(m_enemy, newCoalition);
		if (evaluateNew > evaluateOld)
		{
			c = newCoalition;
		}
		else if (evaluateNew == evaluateOld)
		{
			if (urd_0_1(Global::dre) < 0.5)        // �� 50% ���ʸ���
			{
				c = newCoalition;
			}
		}
	}
}

/*
	itemSet = coalition.toItemSet()
    ƥ��ı�׼�ǣ�left �� itemSet ���Ӽ������� right ���� itemSet ���Ӽ�
*/
pair<ItemSet, ItemSet> PopulationBase::matchRules(const Coalition & coalition, const map<pair<ItemSet, ItemSet>, double>& associateRules)
{
	map<pair<ItemSet, ItemSet>, double> candidateRules;

	ItemSet coalitionItemSet;
	coalitionItemSet = coalition.toItemSet();
	map<pair<ItemSet, ItemSet>, double>::const_iterator iter = associateRules.begin();
	while (iter != associateRules.end())
	{
		ItemSet left = iter->first.first;
		if (coalitionItemSet.hasSubset(left) == true)         // ��������ֱ��� coalition ����
		{
			ItemSet right = iter->first.second;
			if (coalitionItemSet.hasSubset(right) == false)   // ��������ֱ߲��� coalition ����
			{
				candidateRules.emplace(*iter);
			}
		}
		iter++;
	}

	iter = candidateRules.begin();
	double maxConfidence = 0;
	pair<ItemSet, ItemSet> bestMatchedRule;
	while (iter != candidateRules.end())
	{
		if (iter->second > maxConfidence)
		{
			maxConfidence = iter->second;
			bestMatchedRule = iter->first;
		}
		else if (isZero(iter->second - maxConfidence) == true)
		{
			bestMatchedRule = iter->first;
		}
		iter++;
	}

	return bestMatchedRule;
}

/*
    local search ��ʱ����Ҫ��һЩ�����������������ǰ�һЩ��������Ϊsource��ת�Ƶ�destination
	@param moveSize       ��ת�Ƶ�������ĸ���
	@param coalition      �Ǹ��壨���ͣ��⣩
	@param matchedRule    ��ƥ��Ĺ���
	@param associateRules �ǵ�ǰ population ���ɵ����еĹ���
	�ҳ����� source��Ӧ����һЩ��Ƶ���� Item
*/
ItemSet PopulationBase::findSource(size_t moveSize, const Coalition & coalition, const pair<ItemSet, ItemSet>& matchedRule,
	const map<pair<ItemSet, ItemSet>, double>& associateRules)
{
	ItemSet source;
	ItemSet candidate(coalition.toItemSet());
	candidate -= matchedRule.first;
	candidate -= matchedRule.second;           // matchedRule ���������Ѿ����ֹ����������ų�
	set<Item> setCandidate = candidate.getItemSet();
	// TODO: ��candidate����ѡ����ִ����ϵ͵ġ����ִ����� m_oneItemSetCount ����ѡ��
	map<ItemSet, int> candidateCount;
	for (const Item & item : setCandidate)
	{
		ItemSet oneItemSet(item);
		int count = m_apriori.getOneItemSetCount(oneItemSet);
		candidateCount.emplace(move(oneItemSet), count);
	}

	forward_list<pair<ItemSet, int>> candidateList;   // Ҫѡ��� Item ���ִ����ٵ�
	candidateList.emplace_front(ItemSet(), INT_MAX);
	forward_list<pair<ItemSet, int>>::const_iterator iterListA;
	forward_list<pair<ItemSet, int>>::const_iterator iterListB;

	map<ItemSet, int>::const_iterator iterCandidateCount = candidateCount.begin();
	while (iterCandidateCount != candidateCount.end())
	{
		size_t counter = 0;
		iterListA = candidateList.before_begin();
		iterListB = candidateList.begin();          // �� itemCount �����ǰ��� key ����ģ����ڵ��������ҵ� value ��С��
		while (iterListB != candidateList.end())    // �ҵ����� list ��λ�ã�list ��ǰ moveSize ������ item �ǳ��ִ�����С��
		{
			if (counter >= moveSize)                // ֻ��Ҫ��ע��С�� moveSize ��
				break;

			if (iterCandidateCount->second > iterListB->second)
			{
				iterListA++;
				iterListB++;
			}
			else if (iterCandidateCount->second == iterListB->second)
			{
				iterListB = candidateList.emplace_after(iterListB, iterCandidateCount->first, iterCandidateCount->second);
				break;                              // ����˲���֮�󣬾Ϳ����˳�ѭ����
			}
			else  // iterCandidateCount->second  < iterListB->second
			{
				iterListA = candidateList.emplace_after(iterListA, iterCandidateCount->first, iterCandidateCount->second);
				break;                              // ����˲���֮�󣬾Ϳ����˳�ѭ����
			}
			counter++;
		}
		iterCandidateCount++;
	}

	iterListA = candidateList.begin();
	size_t counter = 0;
	while (iterListA != candidateList.end())
	{
		if (counter >= moveSize)
		{
			break;
		}
		source.insert(iterListA->first);
		counter++;
		iterListA++;
	}

	return source;
}

void PopulationBase::setResetMe(const bool & resetMe)
{
	m_resetMe = resetMe;
}

void PopulationBase::setResetEnemy(const bool & resetEnemy)
{
	m_resetEnemy = resetEnemy;
}

void PopulationBase::setUpdate(const bool & update)
{
	m_update = update;
}

bool PopulationBase::getResetMe() const
{
	return m_resetMe;
}

bool PopulationBase::getResetEnemy() const
{
	return m_resetEnemy;
}

bool PopulationBase::getUpdate() const
{
	return m_update;
}

bool PopulationBase::getStop() const
{
	return m_stop;
}

int PopulationBase::getSize() const
{
	return m_population.size();
}

/*
    ����population�ĳ�Ա����m_bestCoalitionIndex(��������ø����vector�±�)������������˸���
    ֱ�Ӹ��Ĵ�����������
    @param bC, ��������������˸����vector
*/
void PopulationBase::updateBestCoalitions(vector<Coalition>& bC)
{
	bC.clear();
	bC.reserve(m_bestCoalitionIndex.size());
	for (int & index : m_bestCoalitionIndex)
	{
		bC.push_back(m_population[index]);
	}
}

/*
    get coalitions with the highest evaluations, note that there could be tied best ones
*/
void PopulationBase::updateBestCoalitions()
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
	if (m_population[m_bestCoalitionIndex[0]].getSimpleEvaluate() < 0)
		newBestEvaluation = m_population[m_bestCoalitionIndex[0]].getSimpleEvaluate() - Global::EPSILON;
	else
		newBestEvaluation = m_population[m_bestCoalitionIndex[0]].getSimpleEvaluate() + Global::EPSILON;

	if (newBestEvaluation > m_bestEvaluation)  // �������ֵ�����
	{
		m_bestEvaluation = newBestEvaluation;
		cout << "Best @" << m_updateCounter << "  " << m_bestEvaluation << '\n';
		if (m_appearTarget == false && isZero(m_bestEvaluation - Coalition::target))
		{
			m_logAnalyse << "Best @" << m_updateCounter * m_populationSize << "  " << Coalition::target << '\n';
			m_appearTarget = true;
		}
	}
}

const Coalition & PopulationBase::getEnemy() const
{
	return m_enemy;
}

bool PopulationBase::isZero(double d)
{
	if (d<Global::EPSILON && d>-Global::EPSILON)
		return true;

	return false;
}

/*
    �õ����ڵ�ʱ�䣬C++�����ǰʱ�����ǷѾ�������
	return ʱ���ַ�������ʽ��Mmm dd hh:mm:ss������"May  3 18:10:34"
*/
string PopulationBase::getTimeNow()
{
	using namespace chrono;
	system_clock::time_point tp_now = system_clock::now();
	time_t tt_now = system_clock::to_time_t(tp_now);
	char str[26];
	ctime_s(str, sizeof str, &tt_now);
	string timeNow(str);
	timeNow = timeNow.substr(11, 8);
	for (size_t i = 0; i < timeNow.size(); ++i)
	{
		if (timeNow[i] == ':')
		{
			timeNow[i] = '_';
		}
	}
	return timeNow;
}

/*
    ���³�ʼ���з�̹������
*/
void PopulationBase::resetEnemy(const string & way)
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

/*
    ����ʵ����Ʊ���
*/
void PopulationBase::resetExperVariable()
{
	m_bestEvaluation = -Coalition::INDIVIDUAL_SIZE;
	m_appearTarget = false;
	m_updateCounter = 0;
	m_experimentTimes++;
	Global::dre.seed(m_experimentTimes);
}
