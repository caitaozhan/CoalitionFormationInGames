#include "ItemSet.h"

ItemSet::ItemSet()
{
}

ItemSet::ItemSet(const Item & item)
{
	m_itemSet.emplace(item);
}

ItemSet::ItemSet(const set<Item>& itemSet)
{
	m_itemSet = itemSet;
}

ItemSet::ItemSet(const ItemSet & itemSet)
{
	m_itemSet = itemSet.getItemSet();
}

void ItemSet::insert(const Item & item)
{
	m_itemSet.insert(item);
}

void ItemSet::insert(const ItemSet & itemSet)
{
	set<Item> setItem = itemSet.getItemSet();
	set<Item>::iterator iter = setItem.begin();
	while (iter != setItem.end())
	{
		m_itemSet.insert(*iter);
		iter++;
	}
}

void ItemSet::erase(size_t index)
{
	if (index >= m_itemSet.size())
	{
		return;
	}
	else
	{
		set<Item>::iterator iter = m_itemSet.begin();
		for (size_t i = 0; i != index; ++i)
		{
			iter++;
		}
		iter = m_itemSet.erase(iter);
	}
}

/*
    从 m_itemSet 里面删除 @param item
	如果 m_itemSet 里面没有 item 的话，就不删除，什么都不返回
*/
void ItemSet::erase(const Item & item)
{
	m_itemSet.erase(item);
}

size_t ItemSet::size() const
{
	return m_itemSet.size();
}

/*
    获取 m_itemSet 的子集，参数 start 和 end 是区间范围，左闭右开 [start, end)
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
    判断 @param subset 是不是 *this 的子集
*/
bool ItemSet::hasSubset(const ItemSet & subset) const
{
	if (size() < subset.size())
		return false;

	set<Item>::const_iterator iterThis = m_itemSet.begin();
	set<Item>::const_iterator iterSubset = subset.getItemSet().begin();

	while (iterThis != m_itemSet.end() && iterSubset != subset.getItemSet().end())   // set 里面的元素是排序好的，可以借鉴 merge sort 里面的 merge 思想
	{
		if (*iterThis > *iterSubset)
		{
			return false;
		}
		else if (*iterThis == *iterSubset)
		{
			iterThis++, iterSubset++;
		}
		else  // *iterThis < *iterSubset
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
	if (m_itemSet.size() != itemSet.size())   // 如果大小不相等，两个 set 一定不相等
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

ItemSet & ItemSet::operator=(const ItemSet & itemSet)
{
	if (this != &itemSet)
	{
		m_itemSet.clear();
		m_itemSet = itemSet.getItemSet();
	}
	return *this;
}

/*
    Calculate the difference set A - B, where A is *this and B is @param itemSet
    A - B = { x | every x that belongs to A, and not belongs to B }
    For example, A = { a c h i j }, B = { a b c e f g i } == > A - B = { h j }
	Note: elements in a set are ordered by operator<
*/
ItemSet & ItemSet::operator-=(const ItemSet & itemSet)
{
	set<Item> itemSetA = this->getItemSet();
	set<Item> itemSetB = itemSet.getItemSet();
	set<Item>::const_iterator iterA = itemSetA.begin();
	set<Item>::const_iterator iterB = itemSetB.begin();

	while (iterA != itemSetA.end() && iterB != itemSetB.end())
	{
		if (*iterA == *iterB)
		{
			this->erase(*iterA);
			iterA = itemSetA.erase(iterA);
			iterB++;
		}
		else if (*iterA > *iterB)
		{
			iterB++;
		}
		else  // *iterA < *iterB
		{
			iterA++;
		}
	}

	return *this;
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

/*
    Calculate the difference set A - B.
    A - B = { x | every x that belongs to A, and not belongs to B }
    For example, A = { a c h i j }, B = { a b c e f g i } ==> A - B = { h j }   (Note: elements in a set are ordered by operator<)
    Sine operator- is a member function of class ItemSet,
    A is *this, and B is the @param itemSet.
*/
ItemSet operator-(const ItemSet & itemSetA, const ItemSet & itemSetB)
{
	ItemSet differenceSet(itemSetA);
	differenceSet -= itemSetB;
	return differenceSet;
}
