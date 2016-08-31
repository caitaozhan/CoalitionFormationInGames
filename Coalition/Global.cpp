#include "Global.h"

const int Global::HEIGHT = 50;
const int Global::WIDTH = 50;
const int Global::PIXEL_PER_INDEX = 5;
const double Global::EPSILON = 1e-8;
const double Global::E = 2.718281828;

const int Global::MOVE_X[8] = { 0, 1, 1, 1, 0, -1, -1, -1 };  // 8 Á¬Í¨
const int Global::MOVE_Y[8] = { 1, 1, 0, -1, -1, -1, 0, 1 };

ofVec2f Global::BF_UL = ofVec2f(0, Global::HEIGHT - 1);
ofVec2f Global::BF_LR = ofVec2f(Global::WIDTH - 1, 0);

vector<vector<double>> Global::PROBABILITY_MATRIX = vector<vector<double>>();
vector<double> Global::SUM_OF_ROW = vector<double>();
double Global::TOTAL = 0.0;

mt19937 Global::dre(0);