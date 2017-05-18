#pragma once
#include <iostream>
#include <string>
using namespace std;


/*
    OpenFrameworks����� class ofVec2f û��operator<����
	����޷��ȴ�С������޷���������޷��ŵ�std::map����
	�������Լ�дһ��Item
*/
class Item
{
public:
	Item();
	Item(string itemStr);
	Item(int x, int y);

	string toString();

	bool operator<(const Item & item) const;
	bool operator>(const Item & item) const;
	bool operator==(const Item & item) const;
	bool operator!=(const Item & item) const;

	friend ostream& operator<<(ostream & output, const Item & item);

private:
	int m_x;
	int m_y;
};