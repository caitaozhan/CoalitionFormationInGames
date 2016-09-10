#include "Coalition.h"

double Coalition::target = 34.0;
int Coalition::INDIVIDUAL_SIZE = 48;
uniform_real_distribution<double> Coalition::urd_0_1 = uniform_real_distribution<double>(0.0, 1.0);
uniform_int_distribution<int> Coalition::uid_x = uniform_int_distribution<int>(0, 1);
uniform_int_distribution<int> Coalition::uid_y = uniform_int_distribution<int>(0, 1);

Coalition::Coalition()
{
	m_coalition.resize(0);
	m_simpleEvaluate = m_fitness = m_weight = 0;
}

Coalition::Coalition(const Coalition & c)
{
	m_coalition = c.getCoalition();
	m_color = c.getColor();
	m_simpleEvaluate = c.getSimpleEvaluate();
	m_fitness = c.getFitness();
	m_weight = c.getWeight();
	m_abilityDistance = c.getAbilityDistance();
	m_isEnemy = c.getIsEnemy();
}

Coalition & Coalition::operator=(const Coalition & c)
{
	if (this == &c)    // 取地址
		return *this;
	
	m_coalition = c.getCoalition();
	m_color = c.getColor();
	m_simpleEvaluate = c.getSimpleEvaluate();
	m_fitness = c.getFitness();
	m_weight = c.getWeight();
	m_abilityDistance = c.getAbilityDistance();
	m_isEnemy = c.getIsEnemy();
	return *this;
}

void Coalition::initialize(int individualSize)
{
	m_coalition.resize(individualSize);
	m_simpleEvaluate = m_fitness = m_weight = 0;
}

void Coalition::initialize(int individualSize, int abilityDistance, bool isEnemy)
{
	m_coalition.resize(individualSize);
	m_abilityDistance = abilityDistance;
	m_isEnemy = isEnemy;
	m_simpleEvaluate = m_fitness = m_weight = 0;
}

// 这种方法，在 INDIVIDUAL_SIZE 比较小（比如 8）的时候，效率不错；但是如果 INDIVIDUAL_SIZE，比较大（比如 40）的时候，效率可能不佳
void Coalition::setup_8(double abilityDistance, bool isEnemy, const Coalition &enemy)
{
	m_abilityDistance = abilityDistance;
	m_isEnemy = isEnemy;

	vector<ofVec2f> vecArrayIndex;      // 保存生成的二维坐标
	ofVec2f startPoint;                                              
	if (isEnemy)						// enemy 随机选择一个初始二维坐标
	{
		startPoint = ofVec2f(int(urd_0_1(Global::dre)*Global::WIDTH), int(urd_0_1(Global::dre)*Global::HEIGHT));
	}
	else                                // 我军选地点的时候，不能和 enemy 重合
	{
		//startPoint = ofVec2f(int(ofRandom(Global::BF_UL.x, Global::BF_LR.x + 1)), (int)ofRandom(Global::BF_LR.y, Global::BF_UL.y + 1));// 随机选择一个初始二维坐标 
		startPoint = ofVec2f(uid_x(Global::dre), uid_y(Global::dre));// 随机选择一个初始二维坐标 
		while (contain(enemy, startPoint) == true)
		{
			startPoint.set(uid_x(Global::dre), uid_y(Global::dre));
		}
	}
	vecArrayIndex.push_back(startPoint);  // BUG: 随机找初始点的时候，也要检查是否和敌人区域重合；思维漏洞，检查日志检查了好久才找到

	while (vecArrayIndex.size() < INDIVIDUAL_SIZE)
	{
		int i = int(urd_0_1(Global::dre)*vecArrayIndex.size());      // 在已有的二维坐标中随机选一个
		ofVec2f startPoint = vecArrayIndex[i];
		vector<ofVec2f> newArrayIndex;

		for (int j = 0; j < 8; ++j)                                  // 找到八连通区域中所有没有占用的二维坐标 
		{
			ofVec2f tempArrayIndex;
			tempArrayIndex.x = startPoint.x + Global::MOVE_X[j];
			tempArrayIndex.y = startPoint.y + Global::MOVE_Y[j];
			if (Tank::checkInBoundary(tempArrayIndex))
			{
				if (isEnemy && contain(vecArrayIndex, tempArrayIndex) == false)
				{                                                     // 是敌人的情况下，生成的位置没有限制
					newArrayIndex.push_back(tempArrayIndex);
				}
				if (isEnemy == false && Tank::ckeckInBF(tempArrayIndex) &&
					contain(vecArrayIndex, tempArrayIndex) == false && contain(enemy, tempArrayIndex) == false)
				{                                                     // 我军的情况下，要求在Battle Field里面
					newArrayIndex.push_back(tempArrayIndex);          // 生成的位置不能和敌军的重合
				}
			}
		}
		
		if (newArrayIndex.size() != 0)
		{
			int choose = int(urd_0_1(Global::dre)*newArrayIndex.size());
			vecArrayIndex.push_back(newArrayIndex[choose]);
		}
	}
	
	for (int i = 0; i < INDIVIDUAL_SIZE; ++i)
	{
		m_coalition[i].setup(vecArrayIndex[i], abilityDistance, isEnemy);
	}

	setColor(isEnemy);

	if (isEnemy)
		update_BF(vecArrayIndex);

}

// Complete Random 生成一个联盟，可以想象效果不佳
void Coalition::setup_CR(double abilityDistance, bool isEnemy, const Coalition &enemy)
{
	m_abilityDistance = abilityDistance;
	m_isEnemy = isEnemy;

	vector<ofVec2f> vecArrayIndex;
	ofVec2f temp;
	
	while (vecArrayIndex.size() < INDIVIDUAL_SIZE)
	{
		temp = ofVec2f(uid_x(Global::dre), uid_y(Global::dre));

		if (isEnemy && contain(vecArrayIndex, temp) == false)
			vecArrayIndex.push_back(temp);

		if (isEnemy == false && contain(vecArrayIndex, temp) == false && contain(enemy, temp) == false)
			vecArrayIndex.push_back(temp);
	}

	for (int i = 0; i < INDIVIDUAL_SIZE; ++i)
	{
		m_coalition[i].setup(vecArrayIndex[i], abilityDistance, isEnemy);
	}

	setColor(isEnemy);

	if (isEnemy)
		update_BF(vecArrayIndex);
}

//  用于固定敌军的编队
void Coalition::setup_file(double abilityDistance, bool isEnemy, const string &filename)
{
	m_abilityDistance = abilityDistance;
	m_isEnemy = isEnemy;

	ifstream ifile(filename);
	if (!ifile)
	{
		cerr << "fail to open " << filename << " at Coalition::setup_file";
		return;
	}
	int enemyNum;
	ifile >> enemyNum;
	m_coalition.resize(enemyNum);         // update：敌人和我军的坦克数量可以不相等
	
	vector<ofVec2f> vecArrayIndex;
	ofVec2f temp;
	while (ifile >> temp)
	{
		vecArrayIndex.push_back(move(temp));
	}
	ifile.close();

	for (int i = 0; i < enemyNum; ++i)
	{
		m_coalition[i].setup(vecArrayIndex[i], abilityDistance, isEnemy);
	}

	setColor(isEnemy);

	if (isEnemy)
		update_BF(vecArrayIndex);
}

void Coalition::setColor(bool isEnemy)
{
	if (isEnemy)
		m_color.set(ofColor::red);
	else
		m_color.set(ofColor::black);
}

void Coalition::setAbilityDistance(double abilityDistance)
{
	m_abilityDistance = abilityDistance;
}

void Coalition::setIsEnemy(bool isEnemy)
{
	m_isEnemy = isEnemy;
}

void Coalition::setCoalition(int i, const Tank &t)
{
	m_coalition[i] = t;
}

void Coalition::draw()
{
	for (int i = 0; i < m_coalition.size(); ++i)
	{
		m_coalition.at(i).draw(m_color);
	}
}

/*
	@para vecArrayIndex: 一个二维坐标vector
	@para arrayIndex: 一个二维坐标
	return 一个二维坐标vector中是否contian一个二维坐标
*/
bool Coalition::contain(const vector<ofVec2f> &vecArrayIndex, const ofVec2f &arrayIndex)
{
	for (int i = 0; i < vecArrayIndex.size(); ++i)
	{
		if (arrayIndex == vecArrayIndex[i])  // vecArrayIndex 包含了 arrayIndex
			return true;
	}
	return false;
}

/*
	@para enemy: 一个敌人的联盟
	@para arrayIndex: 一个二维坐标
	return 一个Coalition中是否contain一个arrayIndex
*/
bool Coalition::contain(const Coalition &c, const ofVec2f &arrayIndex)
{
	for (int i = 0; i < c.getCoalition().size(); ++i)
	{
		if (arrayIndex == c.getCoalition().at(i).getArrayIndex())  // 和coalition中的第i个Tank的二维坐标相等
			return true;
	}
	return false;
}

const vector<Tank>& Coalition::getCoalition() const
{
	return m_coalition;
}

const Tank & Coalition::getCoalition(int i) const
{
	return m_coalition[i];
}

const double Coalition::getSimpleEvaluate() const
{
	return m_simpleEvaluate;
}

const double Coalition::getFitness() const
{
	return m_fitness;
}

const double Coalition::getWeight() const
{
	return m_weight;
}

const ofColor & Coalition::getColor() const
{
	return m_color;
}

const int Coalition::getSize() const
{
	return m_coalition.size();
}

const double Coalition::getAbilityDistance() const
{
	return m_abilityDistance;
}

const bool Coalition::getIsEnemy() const
{
	return m_isEnemy;
}

bool Coalition::isZero(double d)
{
	if (d<Global::EPSILON && d>-Global::EPSILON)
		return true;

	return false;
}

void Coalition::pushBackTank(const Tank &t)
{
	m_coalition.push_back(t);
}

void Coalition::setSimpleEvaluate(const double evaluate)
{
	m_simpleEvaluate = evaluate;
}

void Coalition::setFitness(double fit)
{
	m_fitness = fit;
}

void Coalition::setWeight(double weight)
{
	m_weight = weight;
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
			if (ofDist(meTank.x, meTank.y, enemyTank.x, enemyTank.y) <= Tank::ABILITY_DISTANCE)
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
			if (ofDist(enemyTank.x, enemyTank.y, meTank.x, meTank.y) <= Tank::ABILITY_DISTANCE)
			{
				++enemySeeMe;
				break;
			}
		}
	}

	return meSeeEnemy - enemySeeMe;
}

/*
	BUG: 这个方法加一个 inline 这门就跪了？
	从联盟的估值，转换到适应值
	@para evalute: 本联盟的估值
	@para maxEvaluate: 所有联盟的最大估值
	@para minEvaluate: 所有联盟的最小估值
	return 适应值 (越大越好)
*/
double Coalition::calculateFitness(double evaluate, double maxEvaluate, double minEvaluate)
{
	return (evaluate - minEvaluate + 1) / (maxEvaluate - minEvaluate + 1);
}

double Coalition::calculateWeight(double fitness)
{
	//return fitness;   // 直接上适应值，用以拉开差距
	//return sqrt(fitness);

	//return fitness*fitness;
	return 1.0 / (1 + pow(Global::E, -fitness)); 
}

/*
	自我描述
	@para evaluteKind: 评估种类
	return string 自我描述
*/
string Coalition::toString(string evaluateKind)
{
	string msg("");
	if (evaluateKind == "simpleEvaluate")
	{
		msg.append("E: ");
		msg.append(ofToString(m_simpleEvaluate));
		/*msg.append(", F: ");
		msg.append(ofToString(m_fitness));
		msg.append(", W: ");
		msg.append(ofToString(m_weight));*/
	}
	return msg;
}

void Coalition::update_BF(const vector<ofVec2f> &vecArrayIndex)
{
	int lowX = Global::WIDTH, highX = 0, lowY = Global::HEIGHT, highY = 0;
	for (int i = 0; i < vecArrayIndex.size(); ++i)           // 寻找战场范围
	{
		if (vecArrayIndex[i].x < lowX)
			lowX = vecArrayIndex[i].x;                       // 找到最 low 的 X
		else if (vecArrayIndex[i].x > highX)
			highX = vecArrayIndex[i].x;                      // 找到最 high 的 X； 修正BUG：下标错误

		if (vecArrayIndex[i].y < lowY)
			lowY = vecArrayIndex[i].y;						 // 找到最 low 的 Y
		else if (vecArrayIndex[i].y > highY)
			highY = vecArrayIndex[i].y;						 // 找到最 high 的 Y
	}
	Global::BF_UL.x = (lowX - Tank::ABILITY_DISTANCE >= 0) ? lowX - Tank::ABILITY_DISTANCE : 0;
	Global::BF_UL.y = (highY + Tank::ABILITY_DISTANCE <= Global::HEIGHT - 1) ? highY + Tank::ABILITY_DISTANCE : Global::HEIGHT - 1;
	Global::BF_LR.x = (highX + Tank::ABILITY_DISTANCE <= Global::WIDTH - 1) ? highX + Tank::ABILITY_DISTANCE : Global::WIDTH - 1;
	Global::BF_LR.y = (lowY - Tank::ABILITY_DISTANCE >= 0) ? lowY - Tank::ABILITY_DISTANCE : 0;
	//cout << "Upper Left: " << Global::BF_UL << "     Lower Right: " << Global::BF_LR << endl;
	Coalition::uid_x = uniform_int_distribution<int>(Global::BF_UL.x, Global::BF_LR.x);
	Coalition::uid_y = uniform_int_distribution<int>(Global::BF_LR.y, Global::BF_UL.y);
}

/*
	调用者： backupC
	功能：   在backupC[i]周围进行 local search
	@para:  敌人的编队
	@para:  第 i 元素（坦克）
	return: 一个二维坐标，作为 local search 的结果
*/
ofVec2f Coalition::localSearch_small(const Coalition & enemy, int i)
{
	ofVec2f startpoint = m_coalition[i].getArrayIndex();  // 在backupC[i]进行“原地”局部搜索
	vector<ofVec2f> newArrayIndex;
	newArrayIndex.push_back(startpoint);                  // 保证 newArrayIndex 里面至少有一个点可以选择
	
	ofVec2f tempArrayIndex;
	for (int i = 0; i < 8; ++i)
	{
		tempArrayIndex.x = startpoint.x + Global::MOVE_X[i];
		tempArrayIndex.y = startpoint.y + Global::MOVE_Y[i];
		if (Tank::ckeckInBF(tempArrayIndex) && contain(*this, tempArrayIndex) == false
			&& contain(enemy, tempArrayIndex) == false)
		{
			newArrayIndex.push_back(tempArrayIndex);
		}
	}

	int choose = int(urd_0_1(Global::dre)*newArrayIndex.size());
	return newArrayIndex[choose];
}
 
/*
	调用者： backupC
	功能：   在整个backupC周围进行 local search
	@para:  敌人的编队
	return: 一个二维坐标，作为 local search 的结果
*/
ofVec2f Coalition::localSearch_big(const Coalition & enemy)
{
	vector<Tank> tanks = this->getCoalition();
	vector<ofVec2f> newArrayIndex;
	ofVec2f tempArrayIndex;
	for (int i = 0; i < tanks.size(); ++i)
	{
		for (int j = 0; j < 8; ++j)
		{
			tempArrayIndex.x = tanks[i].getArrayIndex().x + Global::MOVE_X[j];
			tempArrayIndex.y = tanks[i].getArrayIndex().y + Global::MOVE_Y[j];
			if (Tank::ckeckInBF(tempArrayIndex) && contain(*this, tempArrayIndex) == false
				&& contain(enemy, tempArrayIndex) == false /*&& contain(newArrayIndex, tempArrayIndex) == false*/)
			{
				newArrayIndex.push_back(tempArrayIndex);
			}
		}
	}
	int choose = int(urd_0_1(Global::dre)*newArrayIndex.size());
	return newArrayIndex[choose];
}

ofVec2f Coalition::localSearch_big_PM(const Coalition & enemy, const vector<vector<double>> &PROBABILITY_MATRIX)
{
	vector<Tank> tanks = this->getCoalition();
	vector<ofVec2f> newArrayIndex;
	vector<double>  newProbability;
	ofVec2f tempArrayIndex;
	for (int i = 0; i < tanks.size(); ++i)
	{
		for (int j = 0; j < 8; ++j)
		{
			tempArrayIndex.x = tanks[i].getArrayIndex().x + Global::MOVE_X[j];
			tempArrayIndex.y = tanks[i].getArrayIndex().y + Global::MOVE_Y[j];
			if (Tank::ckeckInBF(tempArrayIndex) && contain(*this, tempArrayIndex) == false
				&& contain(enemy, tempArrayIndex) == false /*&& contain(newArrayIndex, tempArrayIndex) == false*/)
			{
				newArrayIndex.push_back(tempArrayIndex);                                       // 这两个vector大小一样
				newProbability.push_back(PROBABILITY_MATRIX[tempArrayIndex.y][tempArrayIndex.x]); // 保存PM的概率
			}
		}
	}
	size_t size = newProbability.size();
	for (int i = 1; i < size; i++)
	{
		newProbability[i] += newProbability[i - 1];
	}
	double random = urd_0_1(Global::dre)*newProbability[size - 1];
	int choose = lower_bound(newProbability.begin(), newProbability.end(), random) - newProbability.begin();
	return newArrayIndex[choose];
}

/*
这里出现了大量下标错误：PROBABILITY_MATRIX这个“大矩形”里面有一个BattleField“小矩形”
要搞清楚这里的相对差“offset”

可以优化：累和的时候，使用一维数组

不使用轮盘赌？
*/
ofVec2f Coalition::getPlaceFromPMatrix(const vector<vector<double>> &PROBABILITY_MATRIX, const vector<double> &SUM_OF_ROW, const double &TOTAL)
{
	int x1 = Global::BF_UL.x, x2 = Global::BF_LR.x;
	int y1 = Global::BF_LR.y, y2 = Global::BF_UL.y;

	double choose = urd_0_1(Global::dre)*TOTAL;                              // 产生一个随机数
	// 先找到行，后找到列
	int row = lower_bound(SUM_OF_ROW.begin(), SUM_OF_ROW.end(), choose) - SUM_OF_ROW.begin();  // 行 --> y
	if (row >= 1)
		choose -= SUM_OF_ROW[row - 1];                                    // 这里思维不够缜密，出现了大BUG；现在要找 choose 在第 row 行的位置
	vector<double> sumOfChosenRow(x2 - x1 + 1, 0);
	sumOfChosenRow[0] = PROBABILITY_MATRIX[row + y1][x1];                 // 修正BUG：下标错误
	for (int i = 1; i < sumOfChosenRow.size(); ++i)
	{
		sumOfChosenRow[i] = sumOfChosenRow[i - 1] + PROBABILITY_MATRIX[row + y1][x1 + i];  // 修复BUG：下标错误 + 思维不缜密
	}
	int column = lower_bound(sumOfChosenRow.begin(), sumOfChosenRow.end(), choose) - sumOfChosenRow.begin();

	return ofVec2f(column + x1, row + y1);  // (x, y)
}

bool Coalition::decrease(const Coalition & c1, const Coalition & c2)
{
	if (c1.getSimpleEvaluate() > c2.getSimpleEvaluate())
		return true;

	return false;
}
