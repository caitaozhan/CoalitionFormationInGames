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
	if (this == &c)    // ȡ��ַ
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
	logName.append(ofToString(Coalition::logNumber));  // ��ʼ���� Coalition �Լ�����־
	logName.append(".txt");
	m_logPlace.open(logName);
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
		startPoint = ofVec2f((int)ofRandom(0, 16), (int)ofRandom(0, 16));
	}
	else                                // �Ҿ�ѡ�ص��ʱ�򣬲��ܺ� enemy �غ�
	{
		startPoint = ofVec2f((int)ofRandom(BF_UL.x, BF_LR.x + 1), (int)ofRandom(BF_LR.y, BF_UL.y + 1));// ���ѡ��һ����ʼ��ά���� 
		while (contain(enemy, startPoint) == true)
		{
			startPoint.set((int)ofRandom(BF_UL.x, BF_LR.x + 1), (int)ofRandom(BF_LR.y, BF_UL.y + 1));
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
				if (isEnemy == false && Tank::ckeckInBF(tempArrayIndex) &&
					contain(vecArrayIndex, tempArrayIndex) == false && contain(enemy, tempArrayIndex) == false)
				{                                                     // �Ҿ�������£�Ҫ����Battle Field����
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
	return 1.0 / (1 + pow(E, -fitness));
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
	int lowX = WIDTH, highX = 0, lowY = HEIGHT, highY = 0;
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
	BF_UL.x = (lowX - ABILITY_DISTANCE >= 0) ? lowX - ABILITY_DISTANCE : 0;
	BF_UL.y = (highY + ABILITY_DISTANCE <= HEIGHT - 1) ? highY + ABILITY_DISTANCE : HEIGHT - 1;
	BF_LR.x = (highX + ABILITY_DISTANCE <= WIDTH - 1) ? highX + ABILITY_DISTANCE : WIDTH - 1;
	BF_LR.y = (lowY - ABILITY_DISTANCE >= 0) ? lowY - ABILITY_DISTANCE : 0;
	cout << "Upper Left: " << BF_UL << "     Lower Right: " << BF_LR << endl;
}

/*
��������˴����±����PROBABILITY_MATRIX���������Ρ�������һ��BattleField��С���Ρ�
Ҫ������������Բoffset��

�����Ż����ۺ͵�ʱ��ʹ��һά����

��ʹ�����̶ģ�
*/
ofVec2f Coalition::getPlaceFromPMatrix()
{
	int x1 = BF_UL.x, x2 = BF_LR.x;
	int y1 = BF_LR.y, y2 = BF_UL.y;
	double sumTotal = 0.0;                           // �ܺ�
	vector<double> sumOfRow(y2 - y1 + 1, 0.0);       // �ۻ�������֮��
	for (int y = y1; y <= y2; ++y)
	{
		if (y - 1 >= y1)                             // �ȼ���ǰ���еĺ�        
		{
			sumOfRow[y - y1] = sumOfRow[y - y1 - 1]; // ����BUG���±����
		}
		for (int x = x1; x <= x2; ++x)
		{
			sumTotal += PROBABILITY_MATRIX[y][x];
			sumOfRow[y - y1] += PROBABILITY_MATRIX[y][x];  // ����BUG���±����
		}
	}
	double choose = ofRandom(0, sumTotal);                              // ����һ�������
	// ���ҵ��У����ҵ���
	int row = lower_bound(sumOfRow.begin(), sumOfRow.end(), choose) - sumOfRow.begin();  // �� --> y
	if (row >= 1)
		choose -= sumOfRow[row - 1];                                    // ����˼ά�������ܣ������˴�BUG������Ҫ�� choose �ڵ� row �е�λ��
	vector<double> sumOfChosenRow(x2 - x1 + 1, 0);
	sumOfChosenRow[0] = PROBABILITY_MATRIX[row + y1][x1];               // ����BUG���±����
	for (int i = 1; i < sumOfChosenRow.size(); ++i)
	{
		sumOfChosenRow[i] = sumOfChosenRow[i - 1] + PROBABILITY_MATRIX[row + y1][x1 + i];  // �޸�BUG���±���� + ˼ά������
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

