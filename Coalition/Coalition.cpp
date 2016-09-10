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
	if (this == &c)    // ȡ��ַ
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

// ���ַ������� INDIVIDUAL_SIZE �Ƚ�С������ 8����ʱ��Ч�ʲ���������� INDIVIDUAL_SIZE���Ƚϴ󣨱��� 40����ʱ��Ч�ʿ��ܲ���
void Coalition::setup_8(double abilityDistance, bool isEnemy, const Coalition &enemy)
{
	m_abilityDistance = abilityDistance;
	m_isEnemy = isEnemy;

	vector<ofVec2f> vecArrayIndex;      // �������ɵĶ�ά����
	ofVec2f startPoint;                                              
	if (isEnemy)						// enemy ���ѡ��һ����ʼ��ά����
	{
		startPoint = ofVec2f(int(urd_0_1(Global::dre)*Global::WIDTH), int(urd_0_1(Global::dre)*Global::HEIGHT));
	}
	else                                // �Ҿ�ѡ�ص��ʱ�򣬲��ܺ� enemy �غ�
	{
		//startPoint = ofVec2f(int(ofRandom(Global::BF_UL.x, Global::BF_LR.x + 1)), (int)ofRandom(Global::BF_LR.y, Global::BF_UL.y + 1));// ���ѡ��һ����ʼ��ά���� 
		startPoint = ofVec2f(uid_x(Global::dre), uid_y(Global::dre));// ���ѡ��һ����ʼ��ά���� 
		while (contain(enemy, startPoint) == true)
		{
			startPoint.set(uid_x(Global::dre), uid_y(Global::dre));
		}
	}
	vecArrayIndex.push_back(startPoint);  // BUG: ����ҳ�ʼ���ʱ��ҲҪ����Ƿ�͵��������غϣ�˼ά©���������־����˺þò��ҵ�

	while (vecArrayIndex.size() < INDIVIDUAL_SIZE)
	{
		int i = int(urd_0_1(Global::dre)*vecArrayIndex.size());      // �����еĶ�ά���������ѡһ��
		ofVec2f startPoint = vecArrayIndex[i];
		vector<ofVec2f> newArrayIndex;

		for (int j = 0; j < 8; ++j)                                  // �ҵ�����ͨ����������û��ռ�õĶ�ά���� 
		{
			ofVec2f tempArrayIndex;
			tempArrayIndex.x = startPoint.x + Global::MOVE_X[j];
			tempArrayIndex.y = startPoint.y + Global::MOVE_Y[j];
			if (Tank::checkInBoundary(tempArrayIndex))
			{
				if (isEnemy && contain(vecArrayIndex, tempArrayIndex) == false)
				{                                                     // �ǵ��˵�����£����ɵ�λ��û������
					newArrayIndex.push_back(tempArrayIndex);
				}
				if (isEnemy == false && Tank::ckeckInBF(tempArrayIndex) &&
					contain(vecArrayIndex, tempArrayIndex) == false && contain(enemy, tempArrayIndex) == false)
				{                                                     // �Ҿ�������£�Ҫ����Battle Field����
					newArrayIndex.push_back(tempArrayIndex);          // ���ɵ�λ�ò��ܺ͵о����غ�
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

// Complete Random ����һ�����ˣ���������Ч������
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

//  ���ڹ̶��о��ı��
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
	m_coalition.resize(enemyNum);         // update�����˺��Ҿ���̹���������Բ����
	
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
bool Coalition::contain(const Coalition &c, const ofVec2f &arrayIndex)
{
	for (int i = 0; i < c.getCoalition().size(); ++i)
	{
		if (arrayIndex == c.getCoalition().at(i).getArrayIndex())  // ��coalition�еĵ�i��Tank�Ķ�ά�������
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
			if (ofDist(meTank.x, meTank.y, enemyTank.x, enemyTank.y) <= Tank::ABILITY_DISTANCE)
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
	BUG: ���������һ�� inline ���ž͹��ˣ�
	�����˵Ĺ�ֵ��ת������Ӧֵ
	@para evalute: �����˵Ĺ�ֵ
	@para maxEvaluate: �������˵�����ֵ
	@para minEvaluate: �������˵���С��ֵ
	return ��Ӧֵ (Խ��Խ��)
*/
double Coalition::calculateFitness(double evaluate, double maxEvaluate, double minEvaluate)
{
	return (evaluate - minEvaluate + 1) / (maxEvaluate - minEvaluate + 1);
}

double Coalition::calculateWeight(double fitness)
{
	//return fitness;   // ֱ������Ӧֵ�������������
	//return sqrt(fitness);

	//return fitness*fitness;
	return 1.0 / (1 + pow(Global::E, -fitness)); 
}

/*
	��������
	@para evaluteKind: ��������
	return string ��������
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
	for (int i = 0; i < vecArrayIndex.size(); ++i)           // Ѱ��ս����Χ
	{
		if (vecArrayIndex[i].x < lowX)
			lowX = vecArrayIndex[i].x;                       // �ҵ��� low �� X
		else if (vecArrayIndex[i].x > highX)
			highX = vecArrayIndex[i].x;                      // �ҵ��� high �� X�� ����BUG���±����

		if (vecArrayIndex[i].y < lowY)
			lowY = vecArrayIndex[i].y;						 // �ҵ��� low �� Y
		else if (vecArrayIndex[i].y > highY)
			highY = vecArrayIndex[i].y;						 // �ҵ��� high �� Y
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
	�����ߣ� backupC
	���ܣ�   ��backupC[i]��Χ���� local search
	@para:  ���˵ı��
	@para:  �� i Ԫ�أ�̹�ˣ�
	return: һ����ά���꣬��Ϊ local search �Ľ��
*/
ofVec2f Coalition::localSearch_small(const Coalition & enemy, int i)
{
	ofVec2f startpoint = m_coalition[i].getArrayIndex();  // ��backupC[i]���С�ԭ�ء��ֲ�����
	vector<ofVec2f> newArrayIndex;
	newArrayIndex.push_back(startpoint);                  // ��֤ newArrayIndex ����������һ�������ѡ��
	
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
	�����ߣ� backupC
	���ܣ�   ������backupC��Χ���� local search
	@para:  ���˵ı��
	return: һ����ά���꣬��Ϊ local search �Ľ��
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
				newArrayIndex.push_back(tempArrayIndex);                                       // ������vector��Сһ��
				newProbability.push_back(PROBABILITY_MATRIX[tempArrayIndex.y][tempArrayIndex.x]); // ����PM�ĸ���
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
��������˴����±����PROBABILITY_MATRIX���������Ρ�������һ��BattleField��С���Ρ�
Ҫ������������Բoffset��

�����Ż����ۺ͵�ʱ��ʹ��һά����

��ʹ�����̶ģ�
*/
ofVec2f Coalition::getPlaceFromPMatrix(const vector<vector<double>> &PROBABILITY_MATRIX, const vector<double> &SUM_OF_ROW, const double &TOTAL)
{
	int x1 = Global::BF_UL.x, x2 = Global::BF_LR.x;
	int y1 = Global::BF_LR.y, y2 = Global::BF_UL.y;

	double choose = urd_0_1(Global::dre)*TOTAL;                              // ����һ�������
	// ���ҵ��У����ҵ���
	int row = lower_bound(SUM_OF_ROW.begin(), SUM_OF_ROW.end(), choose) - SUM_OF_ROW.begin();  // �� --> y
	if (row >= 1)
		choose -= SUM_OF_ROW[row - 1];                                    // ����˼ά�������ܣ������˴�BUG������Ҫ�� choose �ڵ� row �е�λ��
	vector<double> sumOfChosenRow(x2 - x1 + 1, 0);
	sumOfChosenRow[0] = PROBABILITY_MATRIX[row + y1][x1];                 // ����BUG���±����
	for (int i = 1; i < sumOfChosenRow.size(); ++i)
	{
		sumOfChosenRow[i] = sumOfChosenRow[i - 1] + PROBABILITY_MATRIX[row + y1][x1 + i];  // �޸�BUG���±���� + ˼ά������
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
