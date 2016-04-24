#include "Coalition.h"

Coalition::Coalition()
{
	m_coalition.resize(INDIVIDUAL_SIZE);
}

// ���ַ������� INDIVIDUAL_SIZE �Ƚ�С������ 8����ʱ��Ч�ʲ���������� INDIVIDUAL_SIZE���Ƚϴ󣨱��� 40����ʱ��Ч�ʿ��ܲ���
void Coalition::setup_8(double abilityDistance, bool isEnemy, const Coalition &enemy)
{
	vector<ofVec2f> vecArrayIndex;      // �������ɵĶ�ά����
	ofVec2f startPoint;                                              
	if (isEnemy)						// enemy ���ѡ��һ����ʼ��ά����
	{
		startPoint = ofVec2f((int)ofRandom(0, 16), (int)ofRandom(0, 16));
	}
	else                                // �Ҿ�ѡ�ص��ʱ�򣬲��ܺ� enemy �غ�
	{
		startPoint = ofVec2f((int)ofRandom(BF_UL.x, BF_LR.x + 1), (int)ofRandom(BF_LR.y, BF_UL.y + 1));// ���ѡ��һ����ʼ��ά���� 
		while (contain(enemy, startPoint) == true)
		{
			startPoint.set((int)ofRandom(BF_UL.x, BF_LR.x), (int)ofRandom(BF_LR.y, BF_UL.y));
		}
	}
	vecArrayIndex.push_back(startPoint);  // BUG: ����ҳ�ʼ���ʱ��ҲҪ����Ƿ�͵��������غϣ�˼ά©���������־����˺þò��ҵ�

	while (vecArrayIndex.size() < INDIVIDUAL_SIZE)
	{
		int i = (int)ofRandom(0, vecArrayIndex.size());              // �����еĶ�ά���������ѡһ��
		ofVec2f startPoint = vecArrayIndex[i];
		vector<ofVec2f> newArrayIndex;

		for (int j = 0; j < 8; ++j)                                  // �ҵ�����ͨ����������û��ռ�õĶ�ά���� 
		{
			ofVec2f tempArrayIndex;
			tempArrayIndex.x = startPoint.x + MOVE_X[j];
			tempArrayIndex.y = startPoint.y + MOVE_Y[j];
			if (Tank::checkInBoundary(tempArrayIndex))
			{
				if (isEnemy && contain(vecArrayIndex, tempArrayIndex) == false)
				{                                                     // �ǵ��˵�����£����ɵ�λ��û������
					newArrayIndex.push_back(tempArrayIndex);
				}
				if (isEnemy == false && contain(vecArrayIndex, tempArrayIndex) == false && contain(enemy, tempArrayIndex) == false)
				{                                                     // �Ҿ��������
					newArrayIndex.push_back(tempArrayIndex);          // ���ɵ�λ�ò��ܺ͵о����غ�
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

// Complete Random ����һ�����ˣ���������Ч������
void Coalition::setup_CR(double abilityDistance, bool isEnemy, const Coalition &enemy)
{
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

void Coalition::setColor(bool isEnemy)
{
	if (isEnemy)
		m_color.set(ofColor::red);
	else
		m_color.set(ofRandom(0, 128), ofRandom(32, 255), ofRandom(32, 255));
}

void Coalition::draw()
{
	for (int i = 0; i < INDIVIDUAL_SIZE; ++i)
	{
		m_coalition.at(i).draw(m_color);
	}
}

/*
	@para vecArrayIndex: һ����ά����vector
	@para arrayIndex: һ����ά����
	return һ����ά����vector���Ƿ�contianһ����ά����
*/
bool Coalition::contain(const vector<ofVec2f> &vecArrayIndex, const ofVec2f &arrayIndex)
{
	for (int i = 0; i < vecArrayIndex.size(); ++i)
	{
		if (arrayIndex == vecArrayIndex[i])  // vecArrayIndex ������ arrayIndex
			return true;
	}
	return false;
}

/*
	@para enemy: һ�����˵�����
	@para arrayIndex: һ����ά����
	return һ��Coalition���Ƿ�containһ��arrayIndex
*/
bool Coalition::contain(const Coalition &enemy, const ofVec2f &arrayIndex)
{
	for (int i = 0; i < enemy.getCoalition().size(); ++i)
	{
		//ofile << arrayIndex << "-" << enemy.getCoalition().at(i).getArrayIndex() << "|";
		if (arrayIndex == enemy.getCoalition().at(i).getArrayIndex())  // ��coalition�еĵ�i��Tank�Ķ�ά�������
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

const double Coalition::getFitness() const
{
	return m_fitness;
}

const double Coalition::getWeight() const
{
	return m_weight;
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
	@para enemy: �о�
	@para me:    �Ҿ�
	return int: meSeeEnemy - enemySeeMe (����ֵ)
	if meSeeEnemy > enemySeeMe
	then meSeeEnemy - enemySeeMe > 0������һ����ֵ������Ϊ���á�
*/
int Coalition::simpleEvalute(const Coalition & enemy, const Coalition & me)
{
	int meSeeEnemy = 0;
	int enemySeeMe = 0;
	
	vector<Tank> meTanks = me.getCoalition();
	vector<Tank> enemyTanks = enemy.getCoalition();
	ofVec2f meTank, enemyTank;

	// �Ҿ�ĳTank��һ���ܹ����õ�һ���о�Tank���ͼ���
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

	// �о�ĳTank��һ���ܹ����õ�һ���Ҿ�Tank���ͼ���
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

inline double Coalition::calculateFitness(double evaluate, double maxEvaluate, double minEvaluate)
{
	return (maxEvaluate - evaluate + 1) / (maxEvaluate - minEvaluate + 1);
}

inline double Coalition::calculateWeight(double fitness)
{
	return 1.0 / (1 + pow(E, -fitness));
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

void Coalition::update_BF(vector<ofVec2f> vecArrayIndex)
{
	int lowX = WIDTH, highX = 0, lowY = HEIGHT, highY = 0;
	for (int i = 0; i < vecArrayIndex.size(); ++i)           // Ѱ��ս����Χ
	{
		if (vecArrayIndex[i].x < lowX)
			lowX = vecArrayIndex[i].x;                       // �ҵ��� low �� X
		else if (vecArrayIndex[i].x > highX)
			highX = vecArrayIndex[i].x;                      // �ҵ��� high �� X

		if (vecArrayIndex[i].y < lowY)
			lowY = vecArrayIndex[i].y;						 // �ҵ��� low �� Y
		else if (vecArrayIndex[i].y > highY)
			highY = vecArrayIndex[i].y;						 // �ҵ��� high �� Y
	}
	BF_UL.x = (lowX - ABILITY_DISTANCE >= 0) ? lowX - ABILITY_DISTANCE : 0;
	BF_UL.y = (highY + ABILITY_DISTANCE <= HEIGHT - 1) ? highY + ABILITY_DISTANCE : HEIGHT - 1;
	BF_LR.x = (highX + ABILITY_DISTANCE <= WIDTH - 1) ? highX + ABILITY_DISTANCE : WIDTH - 1;
	BF_LR.y = (lowY - ABILITY_DISTANCE >= 0) ? lowY - ABILITY_DISTANCE : 0;
	cout << "Upper Left: " << BF_UL << "     Lower Right: " << BF_LR << endl;
}

