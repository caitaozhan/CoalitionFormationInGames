#include "ofApp.h"
#include "../Coalition/Global.h"

const int ofApp::MAX_EXPERIMENT = 15;
const int ofApp::MAX_UPDATE = 500;

//--------------------------------------------------------------
void ofApp::setup(){

	ofTranslate(ofGetWidth() / 2, ofGetHeight() / 2);

	//Set up vertices and colors
	for (int y = 0; y < HEIGHT; ++y)
	{
		for (int x = 0; x < WIDTH; ++x)
		{
			m_mesh.addVertex(ofPoint(Tank::xArrayIndx2Coordi(x), Tank::yArrayIndx2Coordi(y) , 0));  //数组下标 --> 几何坐标
			m_mesh.addColor(ofColor(128, 128, 255));
		}
	}
	//Set up triangles' indices
	for (int y = 0; y < HEIGHT - 1; ++y)
	{
		for (int x = 0; x < WIDTH - 1; ++x)
		{
			int i1 = x + WIDTH*y;
			int i2 = x + 1 + WIDTH*y;
			int i3 = x + WIDTH*(y + 1);
			int i4 = x + 1 + WIDTH*(y + 1);
			m_mesh.addTriangle(i1, i2, i3);
			m_mesh.addTriangle(i2, i4, i3);
		}
	}
	setNormals(m_mesh);
	ofEnableDepthTest();

	BF_UL = ofVec2f(0, HEIGHT - 1);
	BF_LR = ofVec2f(WIDTH - 1, 0);

	LOG_PM.open("../log/32^2,pop=32,ind=32_param/log_simpleEvaluate.txt");
	LOG_ANALYSE.open("../log/32^2,pop=32,ind=32_param/log_analyze.txt");

	m_enemy.initialize(INDIVIDUAL_SIZE);                   // 修正BUG：之前 m_enemy 调用重载的默认构造函数，导致vector大小=0
	//m_enemy.setup_8(ABILITY_DISTANCE, true, Coalition()); 
	//m_enemy.writeLog();
	//m_enemy.setup_CR(ABILITY_DISTANCE, true, Coalition());
	m_enemy.setup_file(ABILITY_DISTANCE, true, "../sample/4_case_20.txt");

	m_population.resize(POPULATION_SIZE);                  // 初始化 m_population
	
	for (int i = 0; i < m_population.size(); ++i)
	{
		Coalition::logNumber++;
		m_population[i].initialize(INDIVIDUAL_SIZE);       // 修正BUG：之前 m_population[i] 调用重载的默认构造函数，导致vector大小=0
		m_population[i].setup_CR(ABILITY_DISTANCE, false, m_enemy);
	}

	m_bestCoalition = getBestCoalition();                  // 从初始化的种群中获得最好的种群

	PROBABILITY_MATRIX.resize(HEIGHT);                     // 初始化 概率矩阵
	vector<double> tmpVector(WIDTH, 0.0);
	for (auto & vec_double : PROBABILITY_MATRIX)
	{
		vec_double = tmpVector;
	}
	updateWeight();   // 初始化的种群 --> 计算其权值
	updatePMatrix();  // 初始化的种群的权值 --> 生成一个初始化的概率矩阵
	m_update = false;
	m_updateCounter = 0;

	m_appearTarget = false;
	m_experimentTimes = 0;
}

//--------------------------------------------------------------
void ofApp::update(){
	
	if (m_update)
	{
		m_updateCounter++;

		if (m_updateCounter == ofApp::MAX_UPDATE)   // 每一次实验进化 MAX_UPDATE 代
		{
			if (m_appearTarget == false)            // MAX_UPDATE 次之内没有找到 target
			{
				cout << "target not found @" << m_updateCounter << '\n';
				LOG_ANALYSE << "target not found @" << m_updateCounter << '\n';
			}
			m_experimentTimes++;                    // 做完了一次实验
			cout << m_experimentTimes << "次实验\n------\n";
			writeLogAnalyse(m_updateCounter);
			resetMe();
			m_updateCounter = 0;                    // 为下一次实验做准备
			m_appearTarget = false;
		}

		if (m_experimentTimes == ofApp::MAX_EXPERIMENT)  // 准备做 MAX_EXPERIMENT 次实验
		{
			cout << "end of experiment!" << endl;
			LOG_ANALYSE.close();                  // 先关闭，再由另外一个类打开“临界文件”
			AnalyzeLog analyzeLog;                // 曾经 AnalyzeLog 是一个独立的project
			analyzeLog.analyze();                 // 新建了一个filter(筛选器),合并到此项目了
												  
			m_update = 0;
		}

		//if (++m_updateCounter % 10 == 0)  //  每隔更新10代分析平均 Evalation
			//writeLogAnalyse(m_updateCounter);
		updatePopluation();          //  新的全局概率矩阵 --> 更新种群位置
		updateWeight();              //  新的种群位置     --> 更新种群的权值
		updatePMatrix();             //  新的种群权值     --> 更新全局的概率矩阵
		updateBestCoalition();       //  更新最好的Coalition
		writeLogMatrix(m_updateCounter);
	}
}

//--------------------------------------------------------------
void ofApp::draw(){
	
	
	m_easyCam.begin();

	string msg = "fps: " + ofToString(ofGetFrameRate(), 2);
	ofDrawBitmapString(msg, ofPoint(100, 100));
	
	m_mesh.drawWireframe();
	m_enemy.draw();
	m_bestCoalition.draw();
	for (int i = 0; i < m_population.size() / 2; ++i)
	{
		ofDrawBitmapString(m_population[i].toString("simpleEvaluate"), -120, -10 * i + 100);
	}
	for (int i = m_population.size() / 2, j = 0; i < m_population.size(); ++i, ++j)
	{
		ofDrawBitmapString(m_population[i].toString("simpleEvaluate"), -100, -10 * j + 100);
	}
	
	m_easyCam.end();
	
}

const Coalition & ofApp::getBestCoalition() const
{
	int bestIndex = 0;
	int bestValue = -m_population[0].getSize();  // 越大越好，初始值设为最小
	for (int i = 0; i < m_population.size(); i++)
	{
		if (m_population.at(i).getSimpleEvaluate() > bestValue)
		{
			bestIndex = i;
			bestValue = m_population.at(i).getSimpleEvaluate();
		}
	}
	return m_population.at(bestIndex);
}

void ofApp::resetMe()
{
	for (int i = 0; i < m_population.size(); ++i)
	{
		m_population[i].setup_CR(ABILITY_DISTANCE, false, m_enemy);  // 更新联盟里所有 tank 的位置
		m_population[i].setIsStangate(true);                         // 修复一个BUG
		m_population[i].setStagnate0(0);
	}
	updateWeight();   // 新的位置 --> 新的 weight
	updatePMatrix();
	m_bestCoalition = getBestCoalition();
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){

	if (key == 'm')   // coalition Setup
	{
		resetMe();
	}
	else if (key == 'e')
	{
		m_enemy.setup_8(ABILITY_DISTANCE, true, m_enemy);
		m_enemy.writeLog();
	}
	else if (key == 'u')
	{
		if (m_update == false)
		{
			m_update = true;
			ofApp::m_experimentTimes = 0;
			ofApp::m_updateCounter = 0;
			cout << "update = " << m_update << endl;
		}
		else
		{ 
			m_update = false;
			cout << "update = " << m_update << endl;
		}
		/*
		updatePopluation();   //  新的全局概率矩阵 --> 更新种群位置
		updateWeight();       //  新的种群位置     --> 更新种群的权值
		updatePMatrix();      //  新的种群权值     --> 更新全局的概率矩阵
		*/
	}
}

//--------------------------------------------------------------
void ofApp::keyReleased(int key){

}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y ){

}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mouseEntered(int x, int y){

}

//--------------------------------------------------------------
void ofApp::mouseExited(int x, int y){

}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h){

}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg){

}

/*
更新权值的过程：
1. 更新 评估值
2. 更新 适应值
3. 更新 权值
*/
void ofApp::updateWeight()
{
	for (Coalition &c : m_population)                  // 更新每一个联盟的评估值
	{
		c.setSimpleEvaluate(Coalition::simpleEvalute(m_enemy, c));
		c.resetAtStagnate0(m_enemy, m_updateCounter);
	}
	
	int maxE = -INDIVIDUAL_SIZE;
	int minE = INDIVIDUAL_SIZE;
	for (Coalition &c : m_population)                  // 找到种群里所有联盟的最大最小估值
	{
		if (c.getSimpleEvaluate() > maxE)
			maxE = c.getSimpleEvaluate();

		if (c.getSimpleEvaluate() < minE)
			minE = c.getSimpleEvaluate();
	}
	for (Coalition &c : m_population)                  // 估值 --> 适应值 --> 权值
	{
		c.setFitness(c.calculateFitness(c.getSimpleEvaluate(), maxE, minE));
		c.setWeight(c.calculateWeight(c.getFitness()));
	}
}

/*
更新Propability Matrix (这是个全局变量)
无法保证每一行的概率和 = 1 --> 没有除以一个“总和”之类的必要

*/
void ofApp::updatePMatrix()
{
	for (auto &vec_double : PROBABILITY_MATRIX)  // 问题：vector有没有一行代码解决？
	{
		for (double &p : vec_double)
		{
			p = SMALL_NUMBER;		             // 不再清零，初始化一个很小的数
		}
	}
	
	for (const Tank &t : m_enemy.getCoalition()) // 敌人的地方，还是零
	{
		int x = t.getArrayIndex().x;
		int y = t.getArrayIndex().y;
		PROBABILITY_MATRIX[x][y] = 0;
	}

	for (const Coalition &c : m_population)
	{
		for (const Tank &tank : c.getCoalition())
		{
			int x = tank.getArrayIndex().x;
			int y = tank.getArrayIndex().y;
			PROBABILITY_MATRIX[x][y] += c.getWeight();
		}
	}
	//writeLogMatrix(m_updateCounter);
}

bool ofApp::isZero(double d)
{
	if (d<EPSILON && d>-EPSILON)
		return true;
	
	return false;
}

void ofApp::writeLogMatrix(int updateCounter)
{
	LOG_PM << '\n' << "update counter: " << updateCounter << '\n';
	for (int y = HEIGHT - 1; y >= 0; --y)
	{
		for (int x = 0; x < WIDTH - 1; ++x)
		{
			if (isZero(PROBABILITY_MATRIX[x][y]))
			{
				LOG_PM << setprecision(0);
			}
			else
			{
				LOG_PM << setprecision(3);
			}
			LOG_PM << left << setw(7) << PROBABILITY_MATRIX[x][y];  // 历史遗留问题，(y,x) --> (x,y)
		}
		LOG_PM << '\n';
	}
	LOG_PM << '\n' << "*******************" << endl;
}

/*
   @param:  更新的次数
   @return: 此时整个population的平均估值
*/
int ofApp::writeLogAnalyse(int updateCounter)
{
	double sum = 0.0;
	double avg = 0.0;
	for (const Coalition &c : m_population)
	{
		sum += c.getSimpleEvaluate();
	}
	avg = sum / m_population.size();
	LOG_ANALYSE << updateCounter << ": " << avg << "\n\n";
	return avg;
}

/*
更新种群的位置
*/
void ofApp::updatePopluation()
{
	for (Coalition &c : m_population)     // c          是论文中的 Si
	{
		Coalition backupC(c);             // backupC    是论文中的 Xi
		Coalition constructC;             // constructC 是论文中的 Xt。 问题：从写代码的角度，这个 Xt 可以不要
		for (int i = 0; i < backupC.getSize(); ++i)
		{
			bool localSearch = false;
			if (ofRandom(0, 1) < PL)      // todo: 这里还有一个&&
			{
				if (ofRandom(0, 1) < LS)
				{// 不做变化，直接复制过去
					constructC.pushBackTank(backupC.getCoalition(i));
				}
				else
				{// local search
					localSearch = true;
					ofVec2f arrayIndex;
					arrayIndex = backupC.localSearch_big_PM(m_enemy);
					Tank newTank;
					newTank.setup(arrayIndex, ABILITY_DISTANCE, false);
					constructC.pushBackTank(newTank);
				}
			}
			else
			{
				ofVec2f arrayIndex;
				do
				{ 
					arrayIndex = Coalition::getPlaceFromPMatrix();  // 问题：可供选择的点越来越少，可能一些很好的点，就“消失”了
				} while (c.contain(backupC, arrayIndex) == true || c.contain(m_enemy, arrayIndex) == true);  // 修复一个bug
				// 当新选的点，如果是该联盟中已存在的点的话，继续选；如果可选择的点很少的话，循环次数较多
				Tank newTank;
				newTank.setup(arrayIndex, ABILITY_DISTANCE, false);
				constructC.pushBackTank(newTank);
			}
			if (backupC.getCoalition()[i] != constructC.getCoalition()[i])
			{
				backupC.setCoalition(i, constructC.getCoalition(i));
			}

			int evaluateBackupC = Coalition::simpleEvalute(m_enemy, backupC);
			int evaluateC = Coalition::simpleEvalute(m_enemy, c);
			if (evaluateBackupC > evaluateC)  // 如果 backup 比 m_population 中的 c 更好，就更新 c
			{
				c = backupC;
			}
			else if(evaluateBackupC == evaluateC && localSearch == true)  // 相等，但是如果是 local search
			{
				if (ofRandom(0, 2) < 1.0)                                 // 给 50% 概率更新
				{
					c = backupC;
				}
			}
		}
		//c.writeLog();
	}
}

// TODO: 
void ofApp::updateBestCoalition()
{
	for (const Coalition & c : m_population)
	{
		if (c.getSimpleEvaluate() > m_bestCoalition.getSimpleEvaluate())
		{
			m_bestCoalition = c;
			cout << "Best @" << m_updateCounter << "  " << c.getSimpleEvaluate() << '\n';
			if (m_appearTarget == false && isZero(c.getSimpleEvaluate() - Coalition::target))
			{// 分别在控制台和log中输出找到targe时候的进化代数
				LOG_ANALYSE << "Best @"<< m_updateCounter << "  " << c.getSimpleEvaluate() << '\n';
				m_appearTarget = true;
			}
		}
	}
}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo){ 

}


//Universal function which sets normals for the triangle mesh
void setNormals(ofMesh &mesh)
{
	//The number of the vertices
	int nV = mesh.getNumVertices();

	//The number of the triangles
	int nT = mesh.getNumIndices() / 3;

	//cout << nV << "  " << nT << endl;  //10,000 & 19,602

	vector<ofPoint> norm(nV);   //Array for the normals

								//Scan all the triangles. For each triangle add its
								//normal to norm's vectors of triangle's vertices
	for (int t = 0; t < nT; ++t)
	{
		//Get indices of the triangle t
		int i1 = mesh.getIndex(3 * t);
		int i2 = mesh.getIndex(3 * t + 1);
		int i3 = mesh.getIndex(3 * t + 2);

		//Get vertices of the triangle
		const ofPoint& v1 = mesh.getVertex(i1);
		const ofPoint& v2 = mesh.getVertex(i2);
		const ofPoint& v3 = mesh.getVertex(i3);

		//Compute the triangle's normal
		ofPoint dir = ((v2 - v1).crossed(v3 - v1)).normalized();

		//Accumulate it to norm array for i1, i2, i3
		norm[i1] += dir;
		norm[i2] += dir;
		norm[i3] += dir;
	}

	//Normalize the normal's length
	for (int i = 0; i < nV; ++i)
	{
		norm[i].normalize();
	}

	//Set the normals to mesh
	mesh.clearNormals();
	mesh.addNormals(norm);
}
