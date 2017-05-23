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
	ItemSet(const set<Item> & itemSet); // 构造函数
	ItemSet(const ItemSet & itemSet);   // 拷贝构造函数

	void insert(const Item & item);
	void erase(size_t index);           // set 里面的元素是排序的，erase 删除掉第 index 个元素
	void erase(const Item & item);
	size_t  size() const;
	ItemSet subSet(size_t start, size_t end) const;
	Item lastItem() const;
	bool hasSubset(const ItemSet & subset) const;

	bool operator==(const ItemSet & itemSet) const;   // 如果不重载 operator== 的话，std::sort 会出bug
	bool operator!=(const ItemSet & itemSet) const;
	bool operator< (const ItemSet & itemSet) const;
	bool operator> (const ItemSet & itemSet) const;   // 重载 operator< 需要满足 strict weak ordering
	
	ItemSet& operator=(const ItemSet & itemSet);      // 赋值函数   assignment operators
	ItemSet& operator-=(const ItemSet & itemSet);     // 集合的差集 compound-assignment operators  

	const set<Item>& getItemSet() const;
	
	friend ostream& operator<<(ostream& output, const ItemSet& item);

private:
	set<Item> m_itemSet;

};

ItemSet operator-(const ItemSet & itemSetA, const ItemSet & itemSetB); // 集合的差集
