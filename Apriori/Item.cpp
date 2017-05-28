#include "Item.h"

Item::Item()
{
	m_x = -1;
	m_y = -1;
}

Item::Item(string itemStr)
{
	itemStr.erase(0, 1);                     // É¾³ý(
	itemStr.erase(itemStr.size() - 1, 1);    // É¾³ý)
	size_t i;
	for (i = 0; i < itemStr.size(); ++i)
	{
		if (itemStr[i] == ',')
		{
			m_x = atoi(itemStr.substr(0, i).c_str());
			i++;
			break;
		}
	}
	m_y = atoi(itemStr.substr(i, itemStr.size() - 1).c_str());
}

Item::Item(int x, int y)
{
	m_x = x;
	m_y = y;
}

void Item::set(int x, int y)
{
	m_x = x;
	m_y = y;
}

void Item::setX(int x)
{
	m_x = x;
}

void Item::setY(int y)
{
	m_y = y;
}

const int & Item::getX() const
{
	return m_x;
}

const int & Item::getY() const
{
	return m_y;
}

string Item::toString()
{
	string str("()");
	str.insert(1, to_string(m_x));
	str.insert(2, ",");
	str.insert(3, to_string(m_y));
	return str;
}

bool Item::operator<(const Item & item) const
{
	if (m_x < item.m_x)
	{
		return true;
	}
	else if (m_x == item.m_x)
	{
		return m_y < item.m_y;
	}
	return false;
}

bool Item::operator>(const Item & item) const
{
	if (m_x > item.m_x)
	{
		return true;
	}
	else if (m_x == item.m_x)
	{
		return m_y > item.m_y;
	}
	return false;
}

bool Item::operator==(const Item & item) const
{
	if (m_x == item.m_x && m_y == item.m_y)
		return true;
	else
		return false;
}

bool Item::operator!=(const Item & item) const
{
	if (m_x != item.m_x || m_y != item.m_y)
		return true;
	else
		return false;
}

ostream & operator<<(ostream & output, const Item & item)
{
	output << "(";
	output << item.m_x << "," << item.m_y;
	output << ")";
	return output;
}
