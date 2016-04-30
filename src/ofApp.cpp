#include "ofApp.h"
#include "Global.h"

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

	// ofile.open("log_simpleEvaluate.txt");
	
	m_enemy.initialize(INDIVIDUAL_SIZE);                   // 修正BUG：之前 m_enemy 调用重载的默认构造函数，导致vector大小=0
	m_enemy.setup_8(ABILITY_DISTANCE, true, Coalition()); 
	//m_enemy.setup_CR(ABILITY_DISTANCE, true, Coalition());

	m_population.resize(POPULATION_SIZE);                  // 初始化 m_population
	for (int i = 0; i < m_population.size(); ++i)
	{
		m_population[i].initialize(INDIVIDUAL_SIZE);       // 修正BUG：之前 m_population[i] 调用重载的默认构造函数，导致vector大小=0
		m_population[i].setup_CR(ABILITY_DISTANCE, false, m_enemy);
	}

	PROBABILITY_MATRIX.resize(HEIGHT);                     // 初始化 概率矩阵
	vector<double> tmpVector(WIDTH, 0.0);
	for (auto & vec_double : PROBABILITY_MATRIX)
	{
		vec_double = tmpVector;
	}
	updateWeight();   // 初始化的种群 --> 计算其权值
	updatePMatrix();  // 初始化的种群的权值 --> 生成一个初始化的概率矩阵
}

//--------------------------------------------------------------
void ofApp::update(){




}

//--------------------------------------------------------------
void ofApp::draw(){

	m_easyCam.begin();
	m_mesh.drawWireframe();
	m_enemy.draw();
	for (int i = 0; i < m_population.size(); ++i)
	{
		m_population[i].draw();
		ofDrawBitmapString(m_population[i].toString("simpleEvaluate"), -150, -15 * i + 80);
		
	}
	m_easyCam.end();

}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){

	if (key == 'm')   // coalition Setup
	{
		for (int i = 0; i < m_population.size(); ++i)
		{
			m_population[i].setup_CR(ABILITY_DISTANCE, false, m_enemy);  // 更新联盟里所有 tank 的位置
		}
		updateWeight();   // 新的位置 --> 新的 weight
		updatePMatrix();  
	}
	else if (key == 'e')
	{
		m_enemy.setup_8(ABILITY_DISTANCE, true, m_enemy);
	}
	else if (key == 'u')
	{
		updatePopluation();   //  新的全局概率矩阵 --> 更新种群位置
		updateWeight();       //  新的种群位置     --> 更新种群的权值
		updatePMatrix();      //  新的种群权值     --> 更新全局的概率矩阵
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
			p = 0;								 // 清零
		}
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
			if (ofRandom(0, 1) < PL)      // todo: 这里还有一个&&
			{
				
				constructC.pushBackTank(backupC.getCoalition(i));
			}
			else
			{
				ofVec2f arrayIndex = Coalition::getPlaceFromPMatrix();  // todo: 防止重复
				Tank newTank;
				newTank.setup(arrayIndex, ABILITY_DISTANCE, false);
				constructC.pushBackTank(newTank);
			}
			if (backupC.getCoalition()[i] != constructC.getCoalition()[i])
			{
				backupC.setCoalition(i, constructC.getCoalition(i));
			}
			if (Coalition::simpleEvalute(m_enemy, backupC) > Coalition::simpleEvalute(m_enemy, c))
			{// 如果 backup 比 m_population 中的 c 更好了，就更新 c
				c = backupC;
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
