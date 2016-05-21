#include "Global.h"

const int HEIGHT = 16;
const int WIDTH  = 16;
const double EPSILON = 1e-8;
const int MOVE_X[8] = { 0, 1, 1, 1, 0, -1, -1, -1 };  // 8 连通
const int MOVE_Y[8] = { 1, 1, 0, -1, -1, -1, 0, 1 };

ofVec2f BF_UL;    // battlefield 的左上角, upper left
ofVec2f BF_LR;    // battlefield 的右下角, lower right

int POPULATION_SIZE = 24;
int INDIVIDUAL_SIZE = 10;
int ABILITY_DISTANCE = 3;

double PL = 0.9;
vector<vector<double>> PROBABILITY_MATRIX;
double SMALL_NUMBER = 0.1;
ofstream LOG_PM;       // 概率矩阵的日志
ofstream LOG_ANALYSE;  // 算法分析日志