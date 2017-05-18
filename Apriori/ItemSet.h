#pragma once
#include <iostream>
#include <set>
#include <string>
#include "Item.h"

using namespace std;

class ItemSet
{
public:
	ItemSet();
	ItemSet(const set<Item> & itemSet);

	void insert(const Item & item);
	void erase(size_t index);           // set 里面的元素是排序的，erase 删除掉第 index 个元素
	void erase(const Item & item);
	size_t size() const;
	ItemSet subSet(size_t start, size_t end) const;
	Item lastItem() const;

	bool operator==(const ItemSet & itemSet) const;  // 如果不重载 operator== 的话，std::sort 会出bug
	bool operator!=(const ItemSet & itemSet) const;
	bool operator< (const ItemSet & itemSet) const;
	bool operator> (const ItemSet & itemSet) const;  // 重载 operator< 需要满足 strict weak ordering

	const set<Item>& getItemSet() const;
	
	friend ostream& operator<<(ostream& output, const ItemSet& item);

private:
	set<Item> m_itemSet;

};