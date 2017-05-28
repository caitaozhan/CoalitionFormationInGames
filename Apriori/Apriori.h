#pragma once
#include <fstream>
#include <vector>
#include <map>
#include <algorithm>
#include <iomanip>
#include "ItemSet.h"
using namespace std;

/*
    ��������ľ����㷨��Apriori�㷨
	I learned apriori algorithm mostly from a book named Data Mining: Practical Machine Learning Tools and Techniques
	and from YouTube vedios such as https://www.youtube.com/watch?v=TcUlzuQ27iQ

    ע�⣡���Apriori�㷨�ǡ�ItemSet�汾��������˵ transaction ����� item �� itemSet �����Զ������
	@author: Caitao Zhan
*/
class Apriori
{
public:
	Apriori();
	Apriori(double minSupportRatio, double minConfidence);

public:
	void setParam(double minSupportRatio, double minConfidence, int maxRepresentativeItemSet, int maxAssociateRule); // ���ò���
	void inputTransactions(const string &fileName);                              // �������ݵ�m_transactions��
	void inputTransactions(vector<vector<ItemSet>> && transactions);             // �������ݵ�m_transactions��
	void findAllFrequentItemSets();                                              // �ҵ����е�Ƶ����     
	void findStrongestAssociateRules();                                          // �ҵ����еĹ�������
	void updateParamters(int maxRepresentativeItemSet, int maxAssociationRule);  // ���²���
	void printRules(const string &fileName);                                     // ��ӡ���еĹ�������
	const map<pair<ItemSet, ItemSet>, double>& getAssociateRules()const;         // getter����
	int  getOneItemSetCount(const ItemSet & itemSet);                            // ���m_oneItemSetCount���������
	void updateMin();                                                            // ����m_minSupportRatio��m_minConfidence

private:
	void findFrequentOneItemSets();                                                 // ��ԭʼ�� transaction ���棬�ҵ�Ƶ���� one-item set
	void generateCandidates(vector<ItemSet>& candidateItemSets, int k);             // ������ѡ�� item set
	void findFrequentItemsFromCandidate(const vector<ItemSet>& candidateKItemSets); // A frequent item is a item with sufficient coverage(support)
	void removeUnfrequentCandidates(map<ItemSet, int>& candidateKItemSetCount);     // �Ӻ�ѡ�� item set ����,ɾ�� support �����,�Ӷ��õ�Ƶ�� item set
	bool onlyDifferInLastItem(const ItemSet& items1, const ItemSet& items2);        // �����ź���� item set ֻ�����һ�� item ��ͬ
	pair<ItemSet, int> findRepresentativeSuperSetCount(const pair<ItemSet, int> & frequentKItemSet); // �� representative item set ����,�ҵ����� frequentKItemSet �ĳ���
	void findSubSet(ItemSet set, unsigned int level, unsigned int prune, vector<ItemSet>& subSet);   // �Ӵ�СΪ level �� set ���棬�ҵ����д�СΪ prune ���Ӽ�
	ItemSet complementSet(const ItemSet & fullSet, const ItemSet & left);           // �ҵ� left �Ļ�������

private:
	double m_minSupportRatio;                    // ��С֧�ֶ�
	double m_minConfidence;                      // ��С���Ŷ�
	int    m_transactionCounter;                 // ���������
	int    m_maxRepresentativeItemSet;           // ����������������� m_minSupportRatio �� m_minConfidence
	int    m_maxAssociateRule;                   // ������ɹ������������ m_minConfidence
	double m_supportRatioStep;                   // ֧�ֶ����ӵĲ���

	vector<vector<ItemSet>>   m_transactions;                 // ԭʼ���ݼ�����vector��˳��洢��˳���ԭʼ���ݵ�˳��һ��
	map<ItemSet, int>         m_oneItemSetCount;              // transaction ԭʼ���ݼ������ one-item set ͳ�Ƽ���
	vector<map<ItemSet, int>> m_frequentKItemSetCount;        // ��¼ K:0~N ��Ƶ�� K-item set���Լ���Ӧ��ͳ�Ƽ���
	map<ItemSet, int>         m_representativeItemSetCount;   // Instead of forming rules from frequent item sets, form rules from representative item sets	
															  // ���ڵ�item��string�����ź���ģ�δ��������Ҫ�滻��MyPoint��ֵ��ע����ǣ�string��set����string��MyPoint��set������
	map<pair<ItemSet, ItemSet>, double> m_associationRule;    // �����������ɵĹ��� (A, CDF)-->0.75, (A, CDF) is the pair, 0.75 is the double
};