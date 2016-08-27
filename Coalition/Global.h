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

	thread_local static ofVec2f BF_UL;             // battlefield 的左上角, upper left
	thread_local static ofVec2f BF_LR;			  // battlefield 的右下角, lower right

	thread_local static default_random_engine dre;  // 默认随机引擎，需要配合“分布”去使用。thread_local表示各个线程都有一个“副本”

	static mutex mtx;
};

