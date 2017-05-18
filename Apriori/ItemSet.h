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
	void erase(size_t index);           // set �����Ԫ��������ģ�erase ɾ������ index ��Ԫ��
	void erase(const Item & item);
	size_t size() const;
	ItemSet subSet(size_t start, size_t end) const;
	Item lastItem() const;

	bool operator==(const ItemSet & itemSet) const;  // ��������� operator== �Ļ���std::sort ���bug
	bool operator!=(const ItemSet & itemSet) const;
	bool operator< (const ItemSet & itemSet) const;
	bool operator> (const ItemSet & itemSet) const;  // ���� operator< ��Ҫ���� strict weak ordering

	const set<Item>& getItemSet() const;
	
	friend ostream& operator<<(ostream& output, const ItemSet& item);

private:
	set<Item> m_itemSet;

};