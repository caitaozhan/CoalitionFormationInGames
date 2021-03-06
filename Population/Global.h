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
	static const double EPSILON;  // 很小的数
	static const double E;        // 自然对数
	static const int MOVE_X[8];
	static const int MOVE_Y[8];

	static ofVec2f BF_UL;             // battlefield 的左上角, upper left
	static ofVec2f BF_LR;			  // battlefield 的右下角, lower right

	static mt19937 dre;  // 默认随机引擎，需要配合“分布”去使用

	static mutex mtx; 
};
