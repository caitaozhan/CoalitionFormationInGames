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
	
	void set(int x, int y);
	void setX(int x);
	void setY(int y);
	const int& getX()const;
	const int& getY()const;

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