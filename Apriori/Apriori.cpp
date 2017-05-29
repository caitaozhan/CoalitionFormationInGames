#include "Apriori.h"

Apriori::Apriori()
{
	m_minSupportRatio    = 0.5;
	m_minConfidence      = 0.75;
	m_transactionCounter = 0;
}

Apriori::Apriori(double minSupportRatio, double minConfidence)
{
	m_minSupportRatio    = minSupportRatio;
	m_minConfidence      = minConfidence;
	m_transactionCounter = 0;
}

void Apriori::setParam(double minSupportRatio, double minConfidence, double maxRepresentativeItemSet, double maxAssociateRule)
{
	m_minSupportRatio = minSupportRatio;
	m_minConfidence = minConfidence;
	m_maxRepresentativeItemSet = maxRepresentativeItemSet;
	m_maxAssociateRule = maxAssociateRule;                      // ��ֵ���� population.size()
	m_supportRatioStep = 1 / maxAssociateRule;
	m_transactionCounter = 0;
}

/*
	@param fileName:�������ԭʼ���ݵ��ļ���
*/
void Apriori::inputTransactions(const string &fileName)
{
	ifstream inFile(fileName);
	if (!inFile)
	{
		cout << "File to open data file!" << endl;
	}
	else
	{
		vector<ItemSet> transaction;            // һ��transaction���������� item��һ�� item �൱����һ�� one-item set 
		string line;
		size_t begin, end;
		while (getline(inFile, line))        // һ�� transaction �������ӵģ�"(3,3) (1,1) (4,4) (6,6) (7,7) "
		{
			m_transactionCounter++;
			begin = 0;
			line.erase(0, line.find_first_not_of(" \r\t\n"));        // ȥ���ַ����ײ��Ŀո�
			line.erase(line.find_last_not_of(" \r\t\n") + 1);        // ȥ���ַ���β���Ŀո�

			while ((end = line.find(' ', begin)) != string::npos)    // һ��transaction���棬ÿһ��item���Կո������ֵ�
			{
				string itemStr = line.substr(begin, end - begin);
				Item item(itemStr);
				ItemSet itemSet;
				itemSet.insert(item);
				transaction.emplace_back(itemSet);
				begin = end + 1;
			}
			string itemStr = line.substr(begin);
			Item item(itemStr);
			ItemSet itemSet;
			itemSet.insert(item);
			transaction.emplace_back(itemSet);                       // һ�������е����һ��
			sort(transaction.begin(), transaction.end());            // �����������Ҫ��Ĭ�� operator< ��С��������
			m_transactions.emplace_back(transaction);
			transaction.clear();
		}
	}
	inFile.close();
}

/*
    ����Ҫ����������һ�£����ҳ�ʼ�� m_transactionCounter
*/
void Apriori::inputTransactions(vector<vector<ItemSet>>&& transactions)
{
	for (vector<ItemSet> & transaction : transactions)
	{
		sort(transaction.begin(), transaction.end());
	}
	m_transactions = transactions;
	m_transactionCounter = transactions.size();
}


void Apriori::findFrequentOneItemSets()
{
	m_oneItemSetCount.clear();
	for (size_t i = 0; i != m_transactions.size(); ++i)
	{
		for (size_t j = 0; j != m_transactions[i].size(); ++j)
		{
			m_oneItemSetCount[m_transactions[i].at(j)]++;       // ͳ��ԭʼtransaction�����и���item�ĳ����������� Strict weak orderings ���˴��
		}
	}
	removeUnfrequentCandidates(m_oneItemSetCount);
}

void Apriori::findAllFrequentItemSets()
{
	m_frequentKItemSetCount.clear();
	m_frequentKItemSetCount.emplace_back();  // ����һ���ռ���Ϊ�� k �� vector �� index�±�һһ��Ӧ
	
	int k = 1;
	findFrequentOneItemSets();
	while (m_frequentKItemSetCount[k].size() != 0)  
	{
		k++;
		vector<ItemSet> candidateKItemSets;
		generateCandidates(candidateKItemSets, k);
		findFrequentItemsFromCandidate(candidateKItemSets);
	}
}

/*
    Idealy, rules are as shortest as possible one the left-hand-side, and as long as possible on the right-hand-side.
	For example, rule A-->CDF is stronger than AC-->DF. If the former rule is right, then the latter rule must be right,
	because P(AC) <= P(A) and P(ACDF)/P(AC) >= P(ACDF)/P(A) >= minimum accuracy
	Also, rule A-->CDF is stronger than rule A-->CD
*/
void Apriori::findStrongestAssociateRules()
{
	m_representativeItemSetCount.clear();
	m_associationRule.clear();
	// Find the representative K-item sets and their counts
	for (size_t i = m_frequentKItemSetCount.size() - 2; i > 1; --i)  // [0] �� [size()-1] �ǿ�Ԫ�أ�[1] �� one-item set �����γ�rule
	{
		map<ItemSet, int>::const_iterator iter = m_frequentKItemSetCount[i].begin();
		while (iter != m_frequentKItemSetCount[i].end())
		{
			pair<ItemSet, int> superSet = findRepresentativeSuperSetCount(*iter);
			if (superSet.second == 0)                                // û���ҵ� super set
			{
				m_representativeItemSetCount.insert(*iter);
			}
			else
			{
				if (iter->second > superSet.second)
				{
					m_representativeItemSetCount.insert(*iter);      // �ҵ��� super set������ support �� super set �ĸ�
				}
			}
			iter++;
		}
	}
	
	// Generate rules from the representative item sets��һ��������ߵ�item����Խ�٣����������Խ��ǿ�󡱣�����ֻ��Ҫ������һЩǿ��Ĺ��������
	map<ItemSet, int>::const_iterator iterRepre = m_representativeItemSetCount.begin();
	while (iterRepre != m_representativeItemSetCount.end())
	{
		ItemSet leftSet = iterRepre->first;  // �ܹ����������ֱߵ�item����ʼ�����е�items
		size_t  leftNum = 1;                 // �������ֱߵ� item ��������ʼ�������һ��

		while (leftNum < leftSet.size())
		{
			map<pair<ItemSet, ItemSet>, double> newRules;   // �������� rules
			vector<ItemSet> allLeft;                        // rule ���ֱ����п��ܵ� itemSet
			vector<ItemSet> leftInNewRules;                 // �ﵽ confidence ��ͱ�׼���� rules �����ֱ�
			findSubSet(leftSet, leftSet.size(), leftNum, allLeft);
			for (size_t i = 0; i < allLeft.size(); ++i)
			{
				ItemSet left = allLeft[i];                                  // left-hand-side is the antecedent
				map<ItemSet, int>::const_iterator iter = m_frequentKItemSetCount[left.size()].find(left);
				int support = iter->second;
				double confidence = static_cast<double>(iterRepre->second) / static_cast<double>(support);  // ��rule�����Ŷȣ��ֳ�׼ȷ��accuracy
				if (confidence > (m_minConfidence - 1.0e-7))
				{
					leftInNewRules.emplace_back(left);
					ItemSet right = complementSet(iterRepre->first, left);  // right-hand-side is the consequent
					newRules.emplace(make_pair(left, right), confidence);
				}
			}
			if (newRules.size() > 0)
			{
				map<pair<ItemSet, ItemSet>, double>::const_iterator iter = newRules.begin();
				while (iter != newRules.end())
				{
					m_associationRule.insert(*iter);
					iter++;
				}
				for (size_t i = 0; i < leftInNewRules.size(); ++i)
				{
					ItemSet left = leftInNewRules[i];
					set<Item> setItem = left.getItemSet();
					set<Item>::iterator iterSet = setItem.begin();
					while (iterSet != setItem.end())
					{
						leftSet.erase(*iterSet);          // ���Ѿ������� rule �����ֱ��Ѿ����ֹ��� item����������֮����� rules �����ֱ�����
						iterSet++;
					}
				}
			}
			leftNum++;
		}
		iterRepre++;
	}
}

/*
    ���²��� m_minSupportRatio �� m_minConfidence��ʹ��
	m_frequentKItemSetCount �� m_associationRule �Ĵ�С �ֱ�С��maxRepresentativeItemSet �� maxAssociationRule
	�൱������߲�������ġ��ż���
	����Ҫ�ǳ���ʱ��Ŀ��ǣ��ر���m_minSupportRatio�����Ƚϵ͵Ļ���������Ƶ����̫��Ļ���ʱ���ָ������
*/			
void Apriori::updateParamters(int maxRepresentativeItemSet, int maxAssociationRule)
{

}

void Apriori::printRules(const string &fileName)
{
	ofstream outFile(fileName);
	cout << "There are " << m_associationRule.size() << " rules" << endl;
	map<pair<ItemSet, ItemSet>, double>::const_iterator iter = m_associationRule.begin();
	while (iter != m_associationRule.end())
	{
		outFile << "Condidence = " <<left<< setw(5) << setprecision(3) << iter->second << " " << iter->first.first << " --> " << iter->first.second << endl;
		cout << "Condidence = " << left<<setw(5) << setprecision(3) << iter->second << " " << iter->first.first << " --> " << iter->first.second << endl;
		iter++;
	}
	outFile.close();
}

const map<pair<ItemSet, ItemSet>, double>& Apriori::getAssociateRules() const
{
	return m_associationRule;
}

/*
    ��Ա m_oneItemSetCount �����˴�ԭʼ transaction data set ����� item ��ͳ�Ƹ���
*/
int Apriori::getOneItemSetCount(const ItemSet & itemSet)
{
	map<ItemSet, int>::iterator iter;
	iter = m_oneItemSetCount.find(itemSet);
	if (iter != m_oneItemSetCount.end())
	{
		return iter->second;
	}
	return -1;
}

/*
    ����m_minSupportRatio��m_minConfidence
	��Ҫ������Ŀǰֻ����ߣ���С֧�ֶȺ���С���Ŷȣ�����������ɹ�����ż�����ֹ���ɵ�Ƶ����͹���̫�ࣨ�ǳ�����ʱ�䣩
*/
void Apriori::updateMin()
{
	if (m_representativeItemSetCount.size() > m_maxRepresentativeItemSet)
	{
		if (m_minSupportRatio < 0.990000001)
		{
			m_minSupportRatio += 0.01;       // �������Ƶ������ż�
		}
	}

	if (m_associationRule.size() > m_maxAssociateRule)
	{
		if (m_minConfidence < 0.990000001)
		{
			m_minConfidence += 0.005;                       // ������ɹ�����ż�
		}
	}
}

void Apriori::generateCandidates(vector<ItemSet>& candidateKItemSets, int k)
{
	// Find all pairs of (k-1)-item sets in m_frequentKItemSetCount[k-1] that only differ in their last item
	vector<pair<ItemSet, ItemSet>> itemPairs;
	map<ItemSet, int>::const_iterator iter1 = m_frequentKItemSetCount[k - 1].begin();
	map<ItemSet, int>::const_iterator iter2;
	while (iter1 != m_frequentKItemSetCount[k - 1].end())
	{
		iter2 = iter1;
		iter2++;
		while (iter2 != m_frequentKItemSetCount[k - 1].end())  // ���� m_frequentKItemSetCount[k-1]��һ��map����������key���ź����
		{                                                      // ������ iter ����������pair<string, string>���������stringҲ���ź���ģ���ߵ�string < �ұߵ�string
			ItemSet itemSet1 = iter1->first;                   // item set �� string������� item ��һ��char������Ҳ����string�洢�ġ�����
			ItemSet itemSet2 = iter2->first;
			if (onlyDifferInLastItem(itemSet1, itemSet2))
			{
				itemPairs.emplace_back(itemSet1, itemSet2);
			}
			iter2++;
		}
		iter1++;
	}
	
	// Create candidate k-item set for each pair by combining the two (k-1)-item sets that are paired
	for (size_t i = 0; i < itemPairs.size(); ++i)
	{
		ItemSet itemSet1(itemPairs[i].first);
		ItemSet itemSet2(itemPairs[i].second);
		itemSet1.insert(itemSet2.lastItem());

		candidateKItemSets.emplace_back(itemSet1);
	}

	// Remove all k-item sets containing any (k-1)-item sets that are not in the m_frequentKItemSetCount[k - 1]
	vector<ItemSet>::const_iterator iterVec = candidateKItemSets.begin();
	map<ItemSet, int>::const_iterator iterMap;
	while (iterVec != candidateKItemSets.end())
	{
		bool flag = true;
		set<Item> itemSet = iterVec->getItemSet();
		set<Item>::iterator iterSet = itemSet.begin();
		while (iterSet != itemSet.end())
		{
			set<Item> itemSubSet(itemSet);
			itemSubSet.erase(*iterSet);
			ItemSet itemSubSetObject(itemSubSet);

			iterMap = m_frequentKItemSetCount[k - 1].find(itemSubSetObject);
			if (iterMap == m_frequentKItemSetCount[k - 1].end())
			{
				iterVec = candidateKItemSets.erase(iterVec);  // ɾ����k-item ����ֻҪ����һ�� (k-1)-item ���� m_frequentKItemSetCount[k - 1] ���棬��Ҫɾȥ��� k-item
				flag = false;
				break;
			}
			iterSet++;
		}

		if (flag == true)
			iterVec++;
	}
}

/*
    ��candidate item �����ҵ�Ƶ���� item�������ݱ��������ݳ�Աm_frequentKItemSetCount����
    @param itemCount item -> ͳ�ƴ���
*/
void Apriori::findFrequentItemsFromCandidate(const vector<ItemSet>& candidateKItemSets)
{
	map<ItemSet, int> candidateKItemSetCount;

	// ���²����� kItems ���м���ͳ�ƣ����� candidateItemCount���ı�������������������ò�����
	for (size_t i = 0; i < candidateKItemSets.size(); ++i)
	{
		ItemSet itemSet = candidateKItemSets[i];
		set<Item> setItem = itemSet.getItemSet();
		int itemCounter = 0;
		for (size_t j = 0; j < m_transactions.size(); ++j)  // kItems[i]�����item���ź���ģ�m_transactions[j]�����itemҲ���ź����
		{
			vector<ItemSet> oneTransaction = m_transactions[j];
			size_t transactionIndex = 0;
			set<Item>::const_iterator iterSet = setItem.begin();

			while (transactionIndex < oneTransaction.size() && iterSet != setItem.end())  // ������ merge sort �� merge ˼��
			{
				ItemSet transactionItem = oneTransaction[transactionIndex];
				ItemSet oneItem;
				oneItem.insert(*iterSet);
				if (oneItem < transactionItem)  // ��֦�����candidate item-set��ǰ��item �� ��ǰ��transaction�����item�� ����ĸ���������
				{
					break;
				}
				else if (oneItem == transactionItem)
				{
					transactionIndex++;
					iterSet++;
				}
				else
				{
					transactionIndex++;
				}
			}
			if (iterSet == setItem.end())    // itemSet��������ˣ�˵��itemSet�����Ԫ�ض��ҵ���
			{
				itemCounter++;
			}
		}
		candidateKItemSetCount.emplace(itemSet, itemCounter);
	}
	removeUnfrequentCandidates(candidateKItemSetCount);
}

void Apriori::removeUnfrequentCandidates(map<ItemSet, int>& candidateKItemSetCount)
{
	map<ItemSet, int> frequentItemCount;
	map<ItemSet, int>::const_iterator iter = candidateKItemSetCount.begin();
	while (iter != candidateKItemSetCount.end())
	{
		if (static_cast<double>(iter->second) / static_cast<double>(m_transactionCounter) > (m_minSupportRatio - 1.0e-8))
		{
			frequentItemCount.emplace(iter->first, iter->second);
		}
		iter++;
	}
	m_frequentKItemSetCount.emplace_back(frequentItemCount);  // Ƶ�� k-item ����ͳ�ƴ��� ������ m_frequentKItemSetCount[k] ���� 
}


/*
	Figure out whether two items only differ in their last item
	�����һ�� item ����һ�� char��items ����һ�� string
*/
bool Apriori::onlyDifferInLastItem(const ItemSet& itemSet1, const ItemSet& itemSet2)
{
	size_t length1 = itemSet1.size();
	size_t length2 = itemSet2.size();  // length1, length2 ������ֵ����ȵ�

	if (length1 == 1)
		return true;

	if (itemSet1.subSet(0, length1 - 1) == itemSet2.subSet(0, length2 - 1))
	{
		if (itemSet1.lastItem() != itemSet2.lastItem())
		{
			return true;        // ֻ�����һ�ͬ
		}
	}
	return false;
}

/*
	Find and return the representative super set of @param representativeKItemSet from m_representativeKItemSetCount
	if not find, return pair("null", 0);
*/
pair<ItemSet, int> Apriori::findRepresentativeSuperSetCount(const pair<ItemSet, int>& frequentKItemSet)
{
	map<ItemSet, int> someRepresentativeSuperSets;
	set<Item> subSet = frequentKItemSet.first.getItemSet();
	map<ItemSet, int>::const_iterator iter = m_representativeItemSetCount.begin();
	
	while (iter != m_representativeItemSetCount.end())
	{
		set<Item> superSet = iter->first.getItemSet();

		set<Item>::const_iterator iterSubSet = subSet.begin();
		set<Item>::const_iterator iterSuperSet = superSet.begin();

		while (iterSubSet != subSet.end() && iterSuperSet != superSet.end())
		{
			if (*iterSubSet < *iterSuperSet)
			{
				break;
			}
			else if (*iterSubSet == *iterSuperSet)
			{
				iterSubSet++;
				iterSuperSet++;
			}
			else
			{
				iterSuperSet++;
			}
		}
		if (iterSubSet == subSet.end())
		{
			someRepresentativeSuperSets.emplace(iter->first, iter->second);   // �����ж������
		}
		iter++;
	}

	// �Ӷ����������ѡȡsupport����һ��
	pair<ItemSet, int> superSetCount = pair<ItemSet, int>(ItemSet(), 0);
	int maxSupport = 0;
	iter = someRepresentativeSuperSets.begin();
	while (iter != someRepresentativeSuperSets.end())
	{
		if (iter->second > maxSupport)
		{
			superSetCount = *iter;
			maxSupport = iter->second;
		}
		iter++;
	}
	return superSetCount;
}

/*
	�Ӵ�СΪlevel��set���棬�ҵ����д�СΪprune��subSet

    level ������������ȣ���ʼ�����ʼ�����ԭʼ set �� size ��С����ʱ��һ�����ĸ��ڵ㡣�� level ����0��ʱ�򣬾���Ҷ�ӽڵ�
    ��������ȴﵽ prune���ҵ��˴�СΪ prune �� subSet����ʱ�򣬾ͼ�֦
    subSet �����ҵ����Ӽ��Ľ��
	���磺
	vector<ItemSet> subset;
	ItemSet set = {ACDF}
	findSubSet(set, 4, 2, subset);
	�õ��Ľ����subset={DF, CF, AF, CD, AD, AC}
*/
void Apriori::findSubSet(ItemSet set, unsigned int level, unsigned int prune, vector<ItemSet>& subSet)
{
	if (set.size() - level > prune)      // ʣ��Ľڵ�϶�û��ϣ������֦
		return;

	if (set.size() == prune)             // �ҵ�Ŀ�꣬ʣ��ļ�֦
	{
		subSet.emplace_back(set);
	}
	else
	{
		findSubSet(set, level - 1, prune, subSet);
		if (level > 0)
		{
			set.erase(level - 1);
			findSubSet(set, level - 1, prune, subSet);
		}
	}
}

/*
    ��һ�����ϵĻ�������,Ҫ�󼯺������Ԫ�����ź���ģ���С����
	���磺
	fullSet = {ACDF}, left = {AC}
	�򷵻ص� right = {DF}
*/
ItemSet Apriori::complementSet(const ItemSet & fullSet, const ItemSet & left)
{
	set<Item>::const_iterator iterFull = fullSet.getItemSet().begin();
	set<Item>::const_iterator iterLeft = left.getItemSet().begin();
	set<Item> right;
	while (iterFull != fullSet.getItemSet().end() && iterLeft != left.getItemSet().end())
	{
		if (*iterFull == *iterLeft)
		{
			iterFull++, iterLeft++;
		}
		else
		{
			right.emplace(*iterFull);
			iterFull++;
		}
	}
	while (iterFull != fullSet.getItemSet().end())
	{
		right.emplace(*iterFull);
		iterFull++;
	}
	return right;
}
