#pragma once
#include "ofMain.h"
#include <random>
using namespace std;

class Global
{
public:

	static const int HEIGHT;
	static const int WIDTH;
	static const int PIXEL_PER_INDEX;
	static const double EPSILON;
	static const double E;
	static const int MOVE_X[8];
	static const int MOVE_Y[8];

	thread_local static ofVec2f BF_UL;             // battlefield �����Ͻ�, upper left
	thread_local static ofVec2f BF_LR;			  // battlefield �����½�, lower right

	thread_local static default_random_engine dre;  // Ĭ��������棬��Ҫ��ϡ��ֲ���ȥʹ�á�thread_local��ʾ�����̶߳���һ����������

	static mutex mtx;
};

