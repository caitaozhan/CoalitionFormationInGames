#include "Coalition.h"

Coalition::Coalition()
{
	m_coalition.resize(INDIVIDUAL_SIZE);
}

void Coalition::setup_8(double abilityDistance, bool isEnemy, const Coalition &enemy)
{
	vector<ofVec2f> vecArrayIndex;      // 保存生成的二维坐标
	ofVec2f startPoint;                                              
	if (isEnemy)						// enemy 随机选择一个初始二维坐标
	{
		startPoint = ofVec2f((int)ofRandom(0, 16), (int)ofRandom(0, 16));
	}
	else
	{
		startPoint = ofVec2f((int)ofRandom(BF_UL.x, BF_LR.x), (int)ofRandom(BF_LR.y, BF_UL.y));// 随机选择一个初始二维坐标 
		while (contain(enemy, startPoint) == true)
		{
			startPoint.set((int)ofRandom(BF_UL.x, BF_LR.x), (int)ofRandom(BF_LR.y, BF_UL.y));
		}
	}

	// vecArrayIndex.push_back(startPoint);                              // BUG: 随机找初始点的时候，也要检查是否和敌人区域重合；思维漏洞，检查日志检查了好久才找到
	
	vecArrayIndex.push_back(startPoint);

	while (vecArrayIndex.size() < INDIVIDUAL_SIZE)
	{
		int i = (int)ofRandom(0, vecArrayIndex.size());              // 在已有的二维坐标中随机选一个
		ofVec2f startPoint = vecArrayIndex[i];
		vector<ofVec2f> newArrayIndex;

		for (int j = 0; j < 8; ++j)                                  // 找到所有没有占用的二维坐标 
		{
			ofVec2f tempArrayIndex;
			tempArrayIndex.x = startPoint.x + MOVE_X[j];
			tempArrayIndex.y = startPoint.y + MOVE_Y[j];
			if (Tank::checkInBoundary(tempArrayIndex))
			{
				if (isEnemy && contain(vecArrayIndex, tempArrayIndex) == false)
				{                                                     // 是敌人的情况下，生成的位置没有限制
					newArrayIndex.push_back(tempArrayIndex);
				}
				if (isEnemy == false && contain(vecArrayIndex, tempArrayIndex) == false && contain(enemy, tempArrayIndex) == false)
				{                                                     // 我军的情况下
					newArrayIndex.push_back(tempArrayIndex);          // 生成的位置不能和敌军的重合
				}
			}
		}
		
		if (newArrayIndex.size() != 0)
		{
			int choose = (int)ofRandom(0, newArrayIndex.size());
			vecArrayIndex.push_back(newArrayIndex[choose]);
		}
	}
	
	for (int i = 0; i < INDIVIDUAL_SIZE; ++i)
	{
		m_coalition[i].setup(vecArrayIndex[i], abilityDistance, isEnemy);
	}
	if (isEnemy)
	{
		m_color.set(ofColor::red);
		int lowX = WIDTH, highX = 0, lowY = HEIGHT, highY = 0;
		for (int i = 0; i < vecArrayIndex.size(); ++i)           // 寻找战场范围
		{
			if (vecArrayIndex[i].x < lowX)
				lowX = vecArrayIndex[i].x;                       // 找到最 low 的 X
			else if (vecArrayIndex[i].x > highX)
				highX = vecArrayIndex[i].x;                      // 找到最 high 的 X

			if (vecArrayIndex[i].y < lowY)
				lowY = vecArrayIndex[i].y;						 // 找到最 low 的 Y
			else if (vecArrayIndex[i].y > highY)
				highY = vecArrayIndex[i].y;						 // 找到最 high 的 Y
		}
		BF_UL.x = (lowX - ABILITY_DISTANCE >= 0) ? lowX - ABILITY_DISTANCE : 0;
		BF_UL.y = (highY + ABILITY_DISTANCE <= HEIGHT - 1) ? highY + ABILITY_DISTANCE : HEIGHT - 1;
		BF_LR.x = (highX + ABILITY_DISTANCE <= WIDTH - 1) ? highX + ABILITY_DISTANCE : WIDTH - 1;
		BF_LR.y = (lowY - ABILITY_DISTANCE >= 0) ? lowY - ABILITY_DISTANCE : 0;
		cout << "Upper Left: " << BF_UL << "     Lower Right: " << BF_LR << endl;
	}
	else
	{
		m_color.set(ofRandom(0, 128), ofRandom(32, 255), ofRandom(32, 255));
	}
}

void Coalition::draw()
{
	for (int i = 0; i < INDIVIDUAL_SIZE; ++i)
	{
		m_coalition.at(i).draw(m_color);
	}
}

bool Coalition::contain(const vector<ofVec2f> &vecArrayIndex, const ofVec2f &arrayIndex)
{
	for (int i = 0; i < vecArrayIndex.size(); ++i)
	{
		if (arrayIndex == vecArrayIndex[i])  // vecArrayIndex 包含了 arrayIndex
			return true;
	}
	return false;
}

bool Coalition::contain(const Coalition &enemy, const ofVec2f &arrayIndex)
{
	for (int i = 0; i < enemy.getCoalition().size(); ++i)
	{
		//ofile << arrayIndex << "-" << enemy.getCoalition().at(i).getArrayIndex() << "|";
		if (arrayIndex == enemy.getCoalition().at(i).getArrayIndex())  // 和coalition中的第i个Tank的二维坐标相等
		{
			//ofile << "true" << endl;
			return true;
		}
	}
	//ofile << "false" << endl;
	return false;
}

const vector<Tank>& Coalition::getCoalition() const
{
	return m_coalition;
}

const double Coalition::getSimpleEvaluate() const
{
	return m_simpleEvaluate;
}

void Coalition::setSimpleEvaluate(const double evaluate)
{
	m_simpleEvaluate = evaluate;
}

/*
	@para enemy: 敌军
	@para me:    我军
	return int: meSeeEnemy - enemySeeMe (评估值)
	if meSeeEnemy > enemySeeMe
	then meSeeEnemy - enemySeeMe > 0，返回一个正值，就以为“好”
*/
int Coalition::simpleEvalute(const Coalition & enemy, const Coalition & me)
{
	int meSeeEnemy = 0;
	int enemySeeMe = 0;
	
	vector<Tank> meTanks = me.getCoalition();
	vector<Tank> enemyTanks = enemy.getCoalition();
	ofVec2f meTank, enemyTank;

	// 我军某Tank，一旦能够看得到一个敌军Tank，就计数
	for (int i = 0; i < meTanks.size(); ++i)
	{
		meTank = meTanks[i].getArrayIndex();
		for (int j = 0; j < enemyTanks.size(); ++j)
		{
			enemyTank = enemyTanks[j].getArrayIndex();
			if (ofDist(meTank.x, meTank.y, enemyTank.x, enemyTank.y) <= ABILITY_DISTANCE)
			{
				++meSeeEnemy;
				break;
			}
		}
	}

	// 敌军某Tank，一旦能够看得到一个我军Tank，就计数
	for (int i = 0; i < enemyTanks.size(); ++i)
	{
		enemyTank = enemyTanks[i].getArrayIndex();
		for (int j = 0; j < meTanks.size(); ++j)
		{
			meTank = meTanks[j].getArrayIndex();
			if (ofDist(enemyTank.x, enemyTank.y, meTank.x, meTank.y) <= ABILITY_DISTANCE)
			{
				++enemySeeMe;
				break;
			}
		}
	}

	return meSeeEnemy - enemySeeMe;
}

string Coalition::toString(string evaluateKind)
{
	string msg("");
	if (evaluateKind == "simpleEvaluate")
	{
		msg += "Evaluate = ";
		msg += ofToString(m_simpleEvaluate);
	}
	return msg;
}

