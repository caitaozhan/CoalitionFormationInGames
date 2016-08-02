#pragma once
#include "ofMain.h"
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

	static ofVec2f BF_UL;             // battlefield 的左上角, upper left
	static ofVec2f BF_LR;			  // battlefield 的右下角, lower right

	static vector<vector<double>> PROBABILITY_MATRIX;

	static mutex mtx;
};

