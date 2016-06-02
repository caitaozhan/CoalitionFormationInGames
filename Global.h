#pragma once
#include "ofMain.h"

const double E = 2.718281828;

extern const int HEIGHT;
extern const int WIDTH;
extern const double EPSILON;
extern const int MOVE_X[8];
extern const int MOVE_Y[8];

extern ofVec2f BF_UL;   
extern ofVec2f BF_LR;

extern int POPULATION_SIZE;
extern int INDIVIDUAL_SIZE;
extern int ABILITY_DISTANCE;

extern double PL;
extern double LS;
extern vector<vector<double>> PROBABILITY_MATRIX;
extern double SMALL_NUMBER;

extern ofstream LOG_PM;
extern ofstream LOG_ANALYSE;