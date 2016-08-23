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
	static const int MOVE_X[8];
	static const int MOVE_Y[8];

	static ofVec2f BF_UL;             // battlefield �����Ͻ�, upper left
	static ofVec2f BF_LR;			  // battlefield �����½�, lower right

	static vector<vector<double>> PROBABILITY_MATRIX;

	static mt19937 dre;  // Ĭ��������棬��Ҫ��ϡ��ֲ���ȥʹ��

	static mutex mtx;  // TODO: �������û����
};

