#include "Coalition.h"

int Coalition::logNumber = 0;
double Coalition::target = 6.0;

Coalition::Coalition()
{
	m_coalition.resize(0);
	m_simpleEvaluate = m_fitness = m_weight = m_stagnate0 = 0;
	m_isStagnate = true;
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
	m_stagnate0 = c.getStagnate0();
	m_isStagnate = c.getIsStagnate();
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
	m_stagnate0 = c.getStagnate0();
	m_isStagnate = c.getIsStagnate();
	return *this;
}

void Coalition::initialize(int individualSize)
{
	m_coalition.resize(individualSize);
	m_simpleEvaluate = m_fitness = m_weight = m_stagnate0 = 0;
	m_isStagnate = true;

	string logName("../log/coalition_");
	logName.append(ofToString(Coalition::logNumber));  // 初始化该 Coalition 自己的日志
	logName.append(".txt");
	m_logPlace.open(logName);
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
		startPoint = ofVec2f((int)ofRandom(0, 16), (int)ofRandom(0, 16));
	}
	else                                // 我军选地点的时候，不能和 enemy 重合
	{
		startPoint = ofVec2f((int)ofRandom(BF_UL.x, BF_LR.x + 1), (int)ofRandom(BF_LR.y, BF_UL.y + 1));// 随机选择一个初始二维坐标 
		while (contain(enemy, startPoint) == true)
		{
			startPoint.set((int)ofRandom(BF_UL.x, BF_LR.x + 1), (int)ofRandom(BF_LR.y, BF_UL.y + 1));
		}
	}
	vecArrayIndex.push_back(startPoint);  // BUG: 随机找初始点的时候，也要检查是否和敌人区域重合；思维漏洞，检查日志检查了好久才找到

	while (vecArrayIndex.size() < INDIVIDUAL_SIZE)
	{
		int i = (int)ofRandom(0, vecArrayIndex.size());              // 在已有的二维坐标中随机选一个
		ofVec2f startPoint = vecArrayIndex[i];
		vector<ofVec2f> newArrayIndex;

		for (int j = 0; j < 8; ++j)                                  // 找到八连通区域中所有没有占用的二维坐标 
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
				if (isEnemy == false && Tank::ckeckInBF(tempArrayIndex) &&
					contain(vecArrayIndex, tempArrayIndex) == false && contain(enemy, tempArrayIndex) == false)
				{                                                     // 我军的情况下，要求在Battle Field里面
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
		temp = ofVec2f((int)ofRandom(BF_UL.x, BF_LR.x + 1), (int)ofRandom(BF_LR.y, BF_UL.y + 1));

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
	int n;
	ifile >> n;
	INDIVIDUAL_SIZE = n;
	vector<ofVec2f> vecArrayIndex;
	ofVec2f temp;
	while (ifile >> temp)
	{
		vecArrayIndex.push_back(temp);
	}
	ifile.close();

	for (int i = 0; i < INDIVIDUAL_SIZE; ++i)
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
		//m_color.set(ofRandom(0, 128), ofRandom(32, 255), ofRandom(32, 255));
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

void Coalition::setStagnate0(int s)
{
	m_stagnate0 = s;
}

void Coalition::setIsStangate(bool iS)
{
	m_isStagnate = iS;
}

void Coalition::draw()
{
	for (int i = 0; i < INDIVIDUAL_SIZE; ++i)
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

const int Coalition::getStagnate0() const
{
	return m_stagnate0;
}

const bool Coalition::getIsStagnate() const
{
	return m_isStagnate;
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
	return 1.0 / (1 + pow(E, -fitness));
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
	int lowX = WIDTH, highX = 0, lowY = HEIGHT, highY = 0;
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
	BF_UL.x = (lowX - ABILITY_DISTANCE >= 0) ? lowX - ABILITY_DISTANCE : 0;
	BF_UL.y = (highY + ABILITY_DISTANCE <= HEIGHT - 1) ? highY + ABILITY_DISTANCE : HEIGHT - 1;
	BF_LR.x = (highX + ABILITY_DISTANCE <= WIDTH - 1) ? highX + ABILITY_DISTANCE : WIDTH - 1;
	BF_LR.y = (lowY - ABILITY_DISTANCE >= 0) ? lowY - ABILITY_DISTANCE : 0;
	cout << "Upper Left: " << BF_UL << "     Lower Right: " << BF_LR << endl;
}

/*
这里出现了大量下标错误：PROBABILITY_MATRIX这个“大矩形”里面有一个BattleField“小矩形”
要搞清楚这里的相对差“offset”

可以优化：累和的时候，使用一维数组

不使用轮盘赌？
*/
ofVec2f Coalition::getPlaceFromPMatrix()
{
	int x1 = BF_UL.x, x2 = BF_LR.x;
	int y1 = BF_LR.y, y2 = BF_UL.y;
	double sumTotal = 0.0;                           // 总和
	vector<double> sumOfRow(y2 - y1 + 1, 0.0);       // 累积到该行之和
	for (int y = y1; y <= y2; ++y)
	{
		if (y - 1 >= y1)                             // 先加上前面行的和        
		{
			sumOfRow[y - y1] = sumOfRow[y - y1 - 1]; // 修正BUG：下标错误
		}
		for (int x = x1; x <= x2; ++x)
		{
			sumTotal += PROBABILITY_MATRIX[y][x];
			sumOfRow[y - y1] += PROBABILITY_MATRIX[y][x];  // 修正BUG：下标错误
		}
	}
	double choose = ofRandom(0, sumTotal);                              // 产生一个随机数
	// 先找到行，后找到列
	int row = lower_bound(sumOfRow.begin(), sumOfRow.end(), choose) - sumOfRow.begin();  // 行 --> y
	if (row >= 1)
		choose -= sumOfRow[row - 1];                                    // 这里思维不够缜密，出现了大BUG；现在要找 choose 在第 row 行的位置
	vector<double> sumOfChosenRow(x2 - x1 + 1, 0);
	sumOfChosenRow[0] = PROBABILITY_MATRIX[row + y1][x1];               // 修正BUG：下标错误
	for (int i = 1; i < sumOfChosenRow.size(); ++i)
	{
		sumOfChosenRow[i] = sumOfChosenRow[i - 1] + PROBABILITY_MATRIX[row + y1][x1 + i];  // 修复BUG：下标错误 + 思维不缜密
	}
	int column = lower_bound(sumOfChosenRow.begin(), sumOfChosenRow.end(), choose) - sumOfChosenRow.begin();

	return ofVec2f(column + x1, row + y1);  // (x, y)
}


void Coalition::writeLog()
{
	for (Tank &t : m_coalition)
	{
		m_logPlace << t.getArrayIndex() << '\n';
	}
	m_logPlace << '\n' << "-----------" << endl;
}

