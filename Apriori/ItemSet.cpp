#include "ItemSet.h"

ItemSet::ItemSet()
{
}

ItemSet::ItemSet(const set<Item>& itemSet)
{
	m_itemSet = itemSet;
}

void ItemSet::insert(const Item & item)
{
	m_itemSet.insert(item);
}

void ItemSet::erase(size_t index)
{
	if (index >= m_itemSet.size())
	{
		return;
	}
	else
	{
		size_t counter = 0;
		set<Item>::iterator iter = m_itemSet.begin();
		while (iter != m_itemSet.end())
		{
			if (counter == index)
			{
				iter = m_itemSet.erase(iter);
				break;
			}
			counter++, iter++;
		}
	}
}

/*
    �� m_itemSet ����ɾ�� @param item
	��� m_itemSet ����û�� item �Ļ����Ͳ�ɾ����ʲô��������
*/
void ItemSet::erase(const Item & item)
{
	set<Item>::iterator iter = m_itemSet.begin();
	while (iter != m_itemSet.end())
	{
		if (*iter == item)
		{
			iter = m_itemSet.erase(iter);
			break;
		}
		iter++;
	}
}

size_t ItemSet::size() const
{
	return m_itemSet.size();
}

/*
    ��ȡ m_itemSet ���Ӽ������� start �� end �����䷶Χ������ҿ� [start, end)
*/
ItemSet ItemSet::subSet(size_t start, size_t end) const
{
	ItemSet subSet;
	size_t counter = 0;
	set<Item>::const_iterator iter = m_itemSet.begin();
	while (iter != m_itemSet.end())
	{
		if (counter >= start && counter < end)
		{
			subSet.insert(*iter);
		}
		counter++, iter++;

		if (counter >= end)
			break;
	}

	return subSet;
}

Item ItemSet::lastItem() const
{
	return *m_itemSet.rbegin();
}

/*
    �ж� @param subset �ǲ��� *this ���Ӽ�
*/
bool ItemSet::hasSubset(const ItemSet & subset) const
{
	if (size() < subset.size())
		return false;

	set<Item>::const_iterator iterThis = m_itemSet.begin();
	set<Item>::const_iterator iterSubset = subset.getItemSet().end();

	while (iterThis != m_itemSet.end() && iterSubset != subset.getItemSet().end())   // set �����Ԫ��������õģ����Խ�� merge sort ����� merge ˼��
	{
		if (*iterSubset > *iterThis)
		{
			return false;
		}
		else if (*iterSubset == *iterThis)
		{
			iterThis++, iterSubset++;
		}
		else
		{
			iterThis++;
		}
	}
	if (iterSubset == subset.getItemSet().end())
	{
		return true;
	}
	else
	{
		return false;
	}
}

bool ItemSet::operator==(const ItemSet & itemSet) const
{
	if (m_itemSet.size() != itemSet.size())   // �����С����ȣ����� set һ�������
		return false;

	set<Item>::const_iterator iterThis = m_itemSet.begin();
	set<Item>::const_iterator iterParam = itemSet.m_itemSet.begin();

	while (iterThis != m_itemSet.end() && iterParam != itemSet.m_itemSet.end())
	{
		if (*iterThis != *iterParam)
			return false;

		iterThis++, iterParam++;
	}
	return true;
}

bool ItemSet::operator!=(const ItemSet & itemSet) const
{
	if (*this == itemSet)
		return false;

	return true;
}

bool ItemSet::operator<(const ItemSet & itemSet) const
{
	set<Item>::const_iterator iterThis = m_itemSet.begin();
	set<Item>::const_iterator iterParam = itemSet.m_itemSet.begin();

	while (iterThis != m_itemSet.end() && iterParam != itemSet.m_itemSet.end())
	{
		if (*iterThis < *iterParam)
		{
			return true;
		}
		else if(*iterThis == *iterParam)
		{
			iterThis++, iterParam++;
			continue;
		}
		else
		{
			return false;
		}
	}
	if (iterThis == m_itemSet.end() && iterParam != itemSet.m_itemSet.end())
	{
		return true;
	}
	else
	{
		return false;
	}
}

bool ItemSet::operator>(const ItemSet & itemSet) const
{
	set<Item>::const_iterator iterThis = m_itemSet.begin();
	set<Item>::const_iterator iterParam = itemSet.m_itemSet.begin();

	while (iterThis != m_itemSet.end() && iterParam != itemSet.m_itemSet.end())
	{
		if (*iterThis > *iterParam)
		{
			return true;
		}
		else if (*iterThis == *iterParam)
		{
			iterThis++, iterParam++;
			continue;
		}
		else
		{
			return false;
		}
	}
	if (iterThis == m_itemSet.end() && iterParam != itemSet.m_itemSet.end())
	{
		return false;
	}
	else
	{
		return true;
	}
}

const set<Item>& ItemSet::getItemSet() const
{
	return m_itemSet;
}

ostream & operator<<(ostream & output, const ItemSet & itemSet)
{
	set<Item>::const_iterator iter = itemSet.m_itemSet.begin();
	output << *iter << " ";
	iter++;
	while (iter != itemSet.m_itemSet.end())
	{
		output << " " << *iter;
		iter++;
	}
	return output;
}
