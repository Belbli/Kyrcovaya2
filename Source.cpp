#define _CRT_SECURE_NO_WARNINGS
#include"Button.h"
#include "FilesFuntions.h"
#include<math.h>
#include<vector>
#include<iostream>
#include<soil.h>
#include<ctime>
using namespace std;



#define MENUPROC 100
#define GAMEPROC 101
#define SUBMENUPROC 102
#define SELECTLVLPROC 103
#define TRAINMODE 104
#define INITPROC 105 
#define SELECTMODEPROC 106
#define NORMALMODE 107
#define SHOWRECORDS 108
#define ENTERTEXT 109
#define EXIT 110

using namespace std;

Button playBtn, recordsBtn, exitBtn, nextLvlBtn, menuBtn, replayBtn, lvlBtns[10], gameModeButtons[2], backBtn;
int windowWidth = 480, windowHeight = 600;
int x = -1, y = -1, btnWidth = 150, btnState, btnStart, btnPressed = -1, plateWidth = windowWidth / 5, xLeftPlatePos, xRightPlatePos, lifes = 3;
int xMousePos = 0, yPlatePos = 3 * windowHeight / 4 + 5, yPrevPlatePos = 0, trainModeLvlPassed = 0, availableLvls = 0;
int blockAmount = 0, score = 0, mode, recordsRows = 0;
bool destroyWnd = false, nextLvl = false, activeKey = false;
float xAngle = 2.0, yAngle = 1.0;
int blockHeight = 20, blockWidth = 50, process = INITPROC, lvlsPassed, lvls, bonusAmount = 0, ballsAmount = 0, normalModeLvlPassed = 0;
//50 20
unsigned char keyS;
unsigned int BG;

struct Ball {
	int radius = 7;
	int x;
	int y;
	bool move;
}ball;

struct Bonus {
	int x;
	int y;
	int bonusType;
	bool move;
}bonus;

struct block {
	int xb;
	int yb;
	int durability; 
	GLuint tex;
} item;

struct Record {
	int priority;
	char name[20];
	int score;
}record;

vector<block> blocks;
vector<Bonus> bonuses;
vector<Ball> balls;
vector<Record> records;

void MouseMove(int ax, int ay){
	xMousePos = ax;
}

void display();
void timer(int);
void reshape(int, int);
void initBlocks(char *lvlPath);
void setLvlsInfo();
int readFile(char *path);
//void saveProgress();
void addRecord();
void saveRecords();
void keyBoardFunc(unsigned char key, int x, int y);
void clearData();

void init(){
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(0, windowWidth, windowHeight, 0, 0, 1);
	glMatrixMode(GL_MODELVIEW);
	
}

GLuint bgTexture, playBtnTex, recBtnTex, exitBtnTex, menuBtnTex,
lvlBtnTex[10], normalBtnTex, trainBtnTex, nextLvlBtnTex, backBtnTex, blocksTex[4];

GLuint load_texture(const char *apFileName) {
	GLuint texture;
	texture = SOIL_load_OGL_texture(apFileName,
		SOIL_LOAD_AUTO,
		SOIL_CREATE_NEW_ID,
		SOIL_FLAG_MIPMAPS | SOIL_FLAG_INVERT_Y | SOIL_FLAG_NTSC_SAFE_RGB | SOIL_FLAG_COMPRESS_TO_DXT);
	return texture;
}

void loadLvlBtnsTextures() {
	char buff[3];
	for (int i = 0; i < 10; i++) {
		char path[15] = "";
		char namePart[10] = "lvlBtn_";
		_itoa(i + 1, buff, 10);
		strcat(namePart, buff);
		strcat(path, namePart);
		strcat(path, ".jpg");
		lvlBtnTex[i] = load_texture(path);
	}
}

void loadBlocksTextures() {
	char buff[3];
	for (int i = 0; i < 4; i++) {
		char path[12] = "block_";
		_itoa(i + 1, buff, 10);
		strcat(path, buff);
		strcat(path, ".jpg");
		blocksTex[i] = load_texture(path);
	}
}

void loadImages() {
	bgTexture = load_texture("2.png");
	menuBtnTex = load_texture("menuBtn.jpg");
	playBtnTex = load_texture("playBtn.jpg");
	recBtnTex = load_texture("recBtn.jpg");
	exitBtnTex = load_texture("exitBtn.jpg");
	trainBtnTex = load_texture("trainModeBtn.jpg");
	nextLvlBtnTex = load_texture("nextLvlBtn.jpg");
	normalBtnTex = load_texture("normalModeBtn.jpg");
	backBtnTex = load_texture("backBtn.jpg");
	loadBlocksTextures();
	loadLvlBtnsTextures();
}

void MouseMoveClick(int btn, int state, int ax, int ay){
	x = ax;
	y = ay;
	btnStart = btn;
	btnState = state;
}

int main(int argc, char *argv[]){
	setlocale(LC_ALL, "RUSSIAN");
	srand(time(NULL));
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE);
	glutInitWindowPosition(50, 50);
	glutInitWindowSize(windowWidth, windowHeight);
	glutCreateWindow("Arcanoid");
	loadImages();
	glutDisplayFunc(display);
	glutReshapeFunc(reshape);
	glutPassiveMotionFunc(MouseMove);
	glutMouseFunc(MouseMoveClick);
	glutKeyboardFunc(keyBoardFunc);
	glutTimerFunc(0, timer, 0);
	init();
	glutMainLoop();
	return 0;
}

void timer(int){
	glutPostRedisplay();
	glutTimerFunc(1000 / 100, timer, 0);
	display();
}

void drawTexture(int aX, int aY, int aW, int aH, GLuint aTextID) {
	glColor3f(1.0, 1.0, 1.0);
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, aTextID);
	glBegin(GL_QUADS);
	glTexCoord2i(0, 0); glVertex2i(aX + aW, aY + aH);
	glTexCoord2i(1, 0); glVertex2i(aX, aY + aH);
	glTexCoord2i(1, 1); glVertex2i(aX , aY);
	glTexCoord2i(0, 1); glVertex2i(aX + aW, aY);
	glEnd();
	glDisable(GL_TEXTURE_2D);
}

void renderBM(float x, float y, void *font, char *str) {
	char *c;
	glRasterPos2f(x, y);
	for (c = str; *c != '\0'; c++) {
		glutBitmapCharacter(font, *c);
	}
}

void printText(int xPos, int yPos, int info) {
	glColor3f(0.0, 0.0, 0.0);
	char str[15];
	_itoa(info, str, 10);
	renderBM(xPos, yPos, GLUT_BITMAP_TIMES_ROMAN_24, str);
}

void printText(int xPos, int yPos, char *str) {
	glColor3f(0.0, 0.0, 0.0);
	renderBM(xPos, yPos, GLUT_BITMAP_TIMES_ROMAN_24, str);
}

void enterRecordName(unsigned char key) {
	static int x = windowWidth/2, s = 0;
	printText(windowWidth/2 - strlen("Enter Your Name : "), windowHeight/4, "Enter Your Name : ");
	if (activeKey) {
		if (key == 8 && s > 0) {
			s--;
			record.name[s] = '\0';
			x += 8;
		}
		else {
			record.name[s] = key;
			s++;
			record.name[s] = '\0';
			x -= 8;
		}
		activeKey = false;
	}
	printText(x, windowHeight/2, record.name);
}

void keyBoardFunc(unsigned char key, int x, int y) {
	if (key == 27 && process == MENUPROC) {
		exit(0);
	}
	if (key == 27) {
		clearData();
		process = MENUPROC;
	}
	if (key == 13 && process == ENTERTEXT) {
		addRecord();
		saveRecords();
		process = SHOWRECORDS;
	}
	keyS = key;
	activeKey = true;
}

void setLvlsInfo() {//Считывание кол-ва пройденных уровней и общего кол-ва уровней
	availableLvls = readFile("progress.txt");
	lvls = readFile("lvlsAmount.txt");
}

//void saveProgress() {
//	FILE *file = fopen("progress.txt", "w+");
//	if (file != NULL) {
//		fprintf(file, "%d", availableLvls);
//		fclose(file);
//	}
//}

void sortRecords() {
	for (int i = 0; i < recordsRows; i++) {
		for (int j = i; j < recordsRows; j++) {
			records[i].priority = i + 1;
			if (records[i].score < records[j].score)
				std::swap(records[i], records[j]);
		}
	}
}

void saveRecords() {
	sortRecords();
	FILE *file = fopen("records.txt", "w");
	if (file != NULL) {
		for (int i = 0; i < recordsRows; i++) {
			fprintf(file, "%d.%s %d\n", records[i].priority, records[i].name, records[i].score);
		}
	}
}

void readRecords() {
	FILE *file = fopen("records.txt", "r");
	if (file != NULL) {
		while (!feof(file)) {
			if (fscanf(file, "%d.%s %d\n", &record.priority, &record.name, &record.score) == 3) {
				recordsRows++;
				records.push_back(record);
			}
		}
	}
}

void showRecords() {
	sortRecords();
	int xPos = 60, yPos = 40;
	if (recordsRows > 0){
		for (int i = 0; i < recordsRows; i++) {
			printText(xPos, yPos, records[i].priority);
			xPos += 30;
			printText(xPos, yPos, records[i].name);
			xPos += 3 * windowWidth / 5;
			printText(xPos, yPos, records[i].score);
			yPos += 30;
			xPos = 60;
		}
	}
	else
		printText(xPos, yPos, "Рекордов не установлено");
	backBtn.CreateButton();
	if (backBtn.isClicked(x, y) && btnStart == GLUT_LEFT_BUTTON && btnState == GLUT_UP) {
		process = MENUPROC;
		btnStart = -1;
	}
}

//int readFile(char *path) {
//	int output = -1;
//	FILE *file = fopen(path, "r");
//	if (file != NULL) {
//		while (fscanf(file, "%d", &output)) {
//			fclose(file);
//			return output;
//		}
//	}
//	return output;
//}

char *loadLvl(char *path) {
	FILE *file;
	file = fopen(path, "r");
	if (file != NULL) {
		fseek(file, 0, SEEK_END);
		int size = ftell(file);
		int buffSize = windowWidth / blockWidth;
		fseek(file, 0, SEEK_SET);
		char *buff = (char*)calloc(buffSize, sizeof(char));
		char *dest = (char*)calloc(size, sizeof(char));
		while (fgets(buff, buffSize, file) != NULL) {
			strcat(dest, buff);
		}
		free(buff);
		fclose(file);
		return dest;
	}
	else
		process = SELECTLVLPROC;
	return NULL;
}

void setBonus(int blockNumber) {
	bonus.bonusType = rand() % 3;
	bonus.x = blocks[blockNumber].xb;
	bonus.y = blocks[blockNumber].yb;
	bonus.move = false;
	bonusAmount++;
}

void initBonuses() {
	int num;
	for (int i = 0; i < blockAmount; i++) {
		num = rand() % 7;
		if (num % 6 == 0) {
			setBonus(i);
			bonuses.push_back(bonus);
		}
	}
}

void initMenuButtons() {
	int btnHeight = 40;
	int lineSpace = btnHeight + 50;
	playBtn.setButtonPosition(windowWidth / 2 - btnWidth / 2, windowHeight / 4, btnWidth, btnHeight, playBtnTex);
	recordsBtn.setButtonPosition(windowWidth / 2 - btnWidth / 2, windowHeight / 4 + lineSpace, btnWidth, btnHeight, recBtnTex);
	exitBtn.setButtonPosition(windowWidth / 2 - btnWidth / 2, windowHeight / 4 + 2 * lineSpace, btnWidth, btnHeight, exitBtnTex);
}

void showMenu(){
	initMenuButtons();
	playBtn.CreateButton();
	recordsBtn.CreateButton();
	exitBtn.CreateButton();
}

void initSubMenuButtons() {
	int subMenuBtnH = 40, subMenuBtnW = 80;
	menuBtn.setButtonPosition(windowWidth / 2 - subMenuBtnW, 3*windowHeight / 4, subMenuBtnW, subMenuBtnH, menuBtnTex);
	nextLvlBtn.setButtonPosition(windowWidth / 2 + subMenuBtnW, 3*windowHeight / 4, subMenuBtnW, subMenuBtnH, nextLvlBtnTex);
	backBtn.setButtonPosition(windowWidth / 2 - subMenuBtnW/2, 3 * windowHeight / 4, subMenuBtnW, subMenuBtnH, backBtnTex);
}

void addSubMenuButtons() {
	nextLvlBtn.CreateButton();
	menuBtn.CreateButton();
}

void addBall() {
	ball.x = xLeftPlatePos + plateWidth / 2;
	ball.y = yPlatePos - ball.radius;
	ball.move = false;
	balls.push_back(ball);
	ballsAmount++;
}

void loadLvl(int i) {
	char lvlPath[10] = "lvl_", lvl[4] = "";
	_itoa(i + 1, lvl, 10);
	strcat(lvlPath, lvl);
	strcat(lvlPath, ".txt");
	process = GAMEPROC;
	btnStart = -1;
	yAngle = 1.0;
	if (mode == TRAINMODE || i == 0) {
		lifes = 3;
		score = 0;
	}
	initBlocks(lvlPath);
	initBonuses();
	addBall();
	glutSetCursor(GLUT_CURSOR_NONE);
}

void subMenu() {
	char *lvlPath;
	
	addSubMenuButtons();
	printText(windowWidth / 2 - strlen("Score : 10000")*6, windowHeight / 4, "Score : ");
	printText(windowWidth / 2, windowHeight / 4, score);
	if (menuBtn.isClicked(x, y) && btnStart == GLUT_LEFT_BUTTON && btnState == GLUT_UP) {
		process = MENUPROC;
		btnStart = -1;
	}
	else if (nextLvlBtn.isClicked(x, y) && btnStart == GLUT_LEFT_BUTTON && btnState == GLUT_UP) {
		loadLvl(trainModeLvlPassed);
		btnStart = -1;
	}
}

void initLvlMenu() {
	int lvlBtnH = 60, lvlBtnW = 80, xSpacing = 120, ySpacing = 30, t = 0;
	for (int i = 0; i < 10; i++) {
		if ((i) % 2 == 0 && i != 0) {
			ySpacing += 3 * lvlBtnH / 2;
			xSpacing = 120;
			t = 0;
		}
		xSpacing += t * (lvlBtnW + 80);
		lvlBtns[i].setButtonPosition(xSpacing, ySpacing, lvlBtnW, lvlBtnH, lvlBtnTex[i]);
		t++;
	}
}

void addLvlMenu() {
	for (int i = 0; i < 10; i++)
		lvlBtns[i].CreateButton();
	backBtn.CreateButton();
}

void initGameModeMenu() {
	int modeBtnH = 40, modeBtnW = 100, xSpacing = windowWidth / 2 - modeBtnW, ySpacing = windowHeight / 2 - modeBtnH/2;
	gameModeButtons[0].setButtonPosition(xSpacing - 20, ySpacing, modeBtnW, modeBtnH, normalBtnTex);
	xSpacing += 30 + modeBtnW;
	gameModeButtons[1].setButtonPosition(xSpacing - 20, ySpacing, modeBtnW, modeBtnH, trainBtnTex);
}

void showGameModeMenu() {
	for (int i = 0; i < 2; i++) {
		gameModeButtons[i].CreateButton();
	}
	backBtn.CreateButton();
}

void selectGameMode() {
	showGameModeMenu();
	if (btnStart == GLUT_LEFT_BUTTON) {
		for (int i = 0; i < 2; i++) {
			if (gameModeButtons[i].isClicked(x, y) && btnState == GLUT_UP) {
				if (i == 0) {
					process = NORMALMODE;
					normalModeLvlPassed = 0;
					mode = NORMALMODE;
				}
				if (i == 1) {
					process = TRAINMODE;
					mode = TRAINMODE;
				}
				return;
			}
			if (backBtn.isClicked(x, y) && btnState == GLUT_UP)
				process = MENUPROC;
		}
	}
}

void menu(){
	showMenu();
	if (playBtn.isClicked(x, y) && btnStart == GLUT_LEFT_BUTTON && btnState == GLUT_UP) {
		process = SELECTMODEPROC;
		score = 0;
	}
	else if (recordsBtn.isClicked(x, y) && btnStart == GLUT_LEFT_BUTTON && btnState == GLUT_UP)
		process = SHOWRECORDS;
	else if (exitBtn.isClicked(x, y) && btnStart == GLUT_LEFT_BUTTON && btnState == GLUT_UP)
		process = EXIT;
	
}

void initFunctions() {
	readRecords();
	initLvlMenu();
	initMenuButtons();
	setLvlsInfo();
	initSubMenuButtons();
	initGameModeMenu();
	process = MENUPROC;
}

bool levelPassed() {
	if (blockAmount == 0)
		return true;
	return false;
}

bool hasBonus(int blockIndx, int &bonusIndex) {
	for (int i = 0; i < bonusAmount; i++) {
		if (blocks[blockIndx].xb == bonuses[i].x && blocks[blockIndx].yb == bonuses[i].y) {
			bonusIndex = i;
			return true;
		}
	}
	return false;
}

void reboundDir(){
	if (yPrevPlatePos < xLeftPlatePos && xAngle > 0 || yPrevPlatePos > xLeftPlatePos && xAngle < 0)
		yAngle *= -1;
	else if (yPrevPlatePos < xLeftPlatePos && xAngle < 0 || yPrevPlatePos > xLeftPlatePos && xAngle > 0) {
		yAngle *= -1;
		xAngle *= -1;
	}
	else
		yAngle *= -1;
}

bool verticalRebound(int blockIndex) {
	for (int i = 0; i < ballsAmount; i++) {
		if(balls[i].y + balls[i].radius == blocks[blockIndex].yb || balls[i].y - balls[i].radius - blockHeight == blocks[blockIndex].yb)
		if (blocks[blockIndex].yb && balls[i].x >= blocks[blockIndex].xb && balls[i].x <= blocks[blockIndex].xb + blockWidth || balls[i].y - balls[i].radius == blocks[blockIndex].yb + blockHeight && balls[i].x >= blocks[blockIndex].xb && balls[i].x <= blocks[blockIndex].xb + blockWidth) {
			yAngle *= -1;
			cout << "xBall = " << balls[i].x << "  yBall = " << balls[i].y << "  blockX = " << blocks[i].xb << "  blockY = " << blocks[i].yb << endl;
			return true;
		}
	}
	return false;
}


bool isTouch(int i, int j) {
	if (balls[i].y + balls[i].radius >= blocks[j].yb
		&& balls[i].y - balls[i].radius <= blocks[j].yb + blockHeight
		&& balls[i].x + balls[i].radius >= blocks[j].xb
		&& balls[i].x - balls[i].radius <= blocks[j].xb + blockWidth)
		return true;
	return false;
}

int setScore(int lifes) {
	if (lifes == 1)
		return 10;
	if (lifes == 2)
		return 20;
	return 30;
}

void collision(){
	int index;
	for (int i = 0; i < ballsAmount; i++) {
		for (int j = 0; j < blockAmount; j++) {
			if (isTouch(i, j)) {
				if (!verticalRebound(j))
					xAngle *= -1;
				blocks[j].durability--;
				blocks[j].tex = blocksTex[blocks[j].durability-1];
				score += setScore(lifes);
			}
			if (blocks[j].durability == 0) {
				if (hasBonus(j, index))
					bonuses[index].move = true;
				blocks.erase(blocks.begin() + j);
				blockAmount--;
			}
		}
	}
}

void setBlockAmount(char *lvl) {
	for (int i = 0; i < strlen(lvl); i++) {
		if (lvl[i] != '0' && lvl[i] != '\n')
			blockAmount++;
	}
}

bool isLvlOpen(int lvl) {
	if (lvl <= availableLvls)
		return true;
	return false;
}

void selectLvl() {
	if (btnStart == GLUT_LEFT_BUTTON) {
		for (int i = 0; i < 10; i++) {
			if (lvlBtns[i].isClicked(x, y) && btnState == GLUT_UP) {
				if (isLvlOpen(i + 1)) {
					if (availableLvls = i + 1)
						nextLvl = true;
					loadLvl(i);
					trainModeLvlPassed = i;
				}
				break;
			}
			if (backBtn.isClicked(x, y) && btnState == GLUT_UP)
				process = SELECTMODEPROC;
		}
	}
}

void initBlocks(char *lvlPath) {
	char *lvlMap = loadLvl(lvlPath);
	if (lvlMap != NULL) {
		int ySpacing = 0, t = 0, xSpasing = 0, k=0;
		setBlockAmount(lvlMap);
		if (blockAmount > 0)
			ySpacing = 0;
		for (int i = 0; i < strlen(lvlMap); i++) {
			if (lvlMap[i] == '\n') {
				ySpacing += blockHeight + 5;
				t = 0;
				xSpasing = 0;
			}
			if (lvlMap[i] != '0' && lvlMap[i] != '\n') {
				item.xb = xSpasing;
				item.yb = ySpacing;
				item.durability = lvlMap[i] - 48;
				item.tex = blocksTex[item.durability-1];
				blocks.push_back(item);
				t++;
				xSpasing += blockWidth + 5;
			}
			if (lvlMap[i] == '0')
				xSpasing += blockWidth + 5;
		}
	}
}

void clearData() {
	balls.clear();
	ballsAmount = 0;
	blocks.clear();
	blockAmount = 0;
	bonuses.clear();
	bonusAmount = 0;
}

void lvlPassed(int mode) {
	if (mode == NORMALMODE) {
		process = NORMALMODE;
		normalModeLvlPassed++;
	}
	else {
		if (nextLvl) {
			availableLvls++;
			trainModeLvlPassed++;
		}
		nextLvl = false;
		process = SUBMENUPROC;
	}
	yAngle = 1.0;
	saveProgress(availableLvls);
	clearData();
}

void ViewBlocks(){
	for (int i = 0; i < blockAmount; i++){
		drawTexture(blocks[i].xb, blocks[i].yb, blockWidth, blockHeight, blocks[i].tex);
	}
	if (blockAmount == 0) {
		lvlPassed(mode);
	}
}

void platePosition() {
	yPrevPlatePos = xLeftPlatePos;
	if (xMousePos >= plateWidth / 2 && xMousePos <= windowWidth - plateWidth / 2) {
		xLeftPlatePos = xMousePos - plateWidth / 2;
		xRightPlatePos = xMousePos + plateWidth / 2;
	}
	else if (xMousePos <= plateWidth / 2) {
		xLeftPlatePos = 0;
		xRightPlatePos = plateWidth;
	}
	else {
		xLeftPlatePos = windowWidth - plateWidth;
		xRightPlatePos = windowWidth;
	}
}

void drawPlate() {
	glColor3f(1.0, 0.0, 0.0);
	platePosition();
	glRectf(xLeftPlatePos, yPlatePos, xRightPlatePos, yPlatePos + 20.0);
}

bool plateCollision(int x, int y, int r) {
	if (y + r == yPlatePos && x >= xLeftPlatePos && x <= xRightPlatePos + r)
		return true;
	return false;
}

void plateRebound() {
	float angle = abs(xMousePos - balls[0].x) / (plateWidth / 6);
	if (angle == 0)
		angle = 1;
	if (xAngle < 0)
		xAngle = -angle;
	else
		xAngle = angle;
	reboundDir();
}

void wallsRebound(int x, int y, int r) {
	if (x - r <= 0 || x + r >= windowWidth)
		xAngle *= -1;
	if (y + r <= 0)
		yAngle *= -1;
}

bool ballLose(int x, int y) {
	if (y > yPlatePos + 40) {
		btnStart = -1;
		yAngle = 1.0;
		lifes--;
		return true;
		if (lifes == 0)
			process = SUBMENUPROC;
	}
	return false;
}

void ballMotion(){
	if (btnStart == GLUT_LEFT_BUTTON) {
		balls[0].move = true;
		btnStart = -1;
	}
	for (int i = 0; i < ballsAmount; i++){
	if (balls[i].move == false) {
		balls[i].x = xLeftPlatePos + plateWidth / 2;
		balls[i].y = yPlatePos - balls[i].radius;
	}
	else{
		if(plateCollision(balls[i].x, balls[i].y, balls[i].radius))
			plateRebound();
		wallsRebound(balls[i].x, balls[i].y, balls[i].radius);
		
		if (ballLose(balls[i].x, balls[i].y))
			balls[i].move = false;
		balls[i].x += xAngle;
		balls[i].y += yAngle;
		}
	}
}

void drawBall(){
	ballMotion();
	glColor3f(0.0, 1.0, 0.0);
	glBegin(GL_TRIANGLE_FAN);
	for (int i = 0; i < ballsAmount; i++){
		for (int j = 0; j <= 50; j++) {
			float angle = 2.0 * 3.1415926 * float(j) / float(50);
			float dx = balls[i].radius * cosf(angle);
			float dy = balls[i].radius * sinf(angle);
			glVertex2f(balls[i].x + dx, balls[i].y + dy);
		}
	}
	glEnd();
}

void applyBonus(int bonusType) {
	if (bonusType == 0)
		plateWidth += plateWidth / 10;
	if (bonusType == 1)
		balls[0].radius = 9;
	if (bonusType == 2)
		lifes++;
}

void drawBonus(int bonusIndex) {
	if (bonuses[bonusIndex].y < yPlatePos + 10) {
		glColor3f(0.0, 1.0, 0.0);
		glPointSize(20);
		glBegin(GL_POINTS);
		glVertex2f(bonuses[bonusIndex].x, bonuses[bonusIndex].y);
		glEnd();
		bonuses[bonusIndex].y++;
		if(plateCollision(bonuses[bonusIndex].x, bonuses[bonusIndex].y, 0))
			applyBonus(bonuses[bonusIndex].bonusType);
	}
	else {
		bonuses.erase(bonuses.begin() + bonusIndex);
		bonusAmount--;
	}
}

void activeBonuses() {
	for (int i = 0; i < bonusAmount; i++) {
		if (bonuses[i].move == true)
			drawBonus(i);
	}
}

void printGameInfo() {
	printText(windowWidth / 2, windowHeight - 60, score);
	printText(windowWidth / 2, windowHeight - 30, lifes);
}

bool isRecord() {
	if (recordsRows < 10)
		return true;
	for (int i = 0; i < recordsRows; i++) {
		if (score > records[i].score) {
			return true;
		}
	}
	return false;
}

void addRecord() {
	record.score = score;
	record.priority = recordsRows;
	if (recordsRows < 10) {
		recordsRows++;
	}
	else {
		records.erase(records.begin() + recordsRows - 1);
	}
	records.push_back(record);
}

void game() {
	if (lifes > 0) {
		printGameInfo();
		drawBall();
		collision();
		
		activeBonuses();
		drawPlate();
		ViewBlocks();
	}
	else {
		if (isRecord() && mode == NORMALMODE) {
			process = ENTERTEXT;
			strcpy(record.name, "");
		}
		else
			process = SUBMENUPROC;
		clearData();
	}
}

void display(){
	
	glClear(GL_COLOR_BUFFER_BIT);
	glLoadIdentity();
	drawTexture(0, 0, windowWidth, windowHeight, bgTexture);
	if(process != GAMEPROC)
		glutSetCursor(GLUT_CURSOR_LEFT_ARROW);
	switch (process) {
		case MENUPROC:
			menu();
			break;
		case TRAINMODE:
			addLvlMenu();
			selectLvl();
			break;
		case NORMALMODE:
			loadLvl(normalModeLvlPassed);
			break;
		case SELECTMODEPROC:
			selectGameMode();
			break;
		case SUBMENUPROC:
			subMenu();
			break;
		case INITPROC:
			initFunctions();
			break;
		case GAMEPROC:
			glutSetCursor(GLUT_CURSOR_NONE);
			game();
			break;
		case SHOWRECORDS:
			showRecords();
			break;
		case ENTERTEXT:
			enterRecordName(keyS);
			break;
		case EXIT:
			glutDestroyWindow(1);
			break;
	}
	if(process != EXIT)
		glutSwapBuffers();
	btnStart = -1;
	//ShowCursor(true);
}

void reshape(int w, int h){
	int wh = w / h;

	windowWidth = w;
	windowHeight = h;

	glViewport(0, 0, w, h);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluOrtho2D(0, windowWidth, windowHeight, 0);
	glMatrixMode(GL_MODELVIEW);
	plateWidth = windowWidth / 4;
	yPlatePos = 2 * windowHeight / 3;
}





//#include <iostream>
//#include <windows.h>  
//#include <gl\GL.h>
//#include <gl\GLU.h>
//#include <GL\glut.h> 
//#include "glaux.h"
//#pragma comment(lib,"glaux.lib")
//
//using namespace std;
//
//int n = 0;
//
//float angle = 0.0f;
//unsigned int Textures[1];
//GLuint texture[1];
//
//int sX = 0, sY = 0;
//bool MOUSE_CLICK = false;
//double rotate_y = 0;
//double rotate_x = 0;
//bool A = true;
//
//void MouseMove(int ax, int ay) {
//	ax -= 180;
//	//ay += 180;
//	rotate_x = ay;
//	rotate_y = ax;
//	glutPostRedisplay();
//}
//	
//	void resize(int w, int h) {
//		glutPostRedisplay();
//		if (h == 0)
//			h = 1;
//		float ratio = w * 1.0 / h;
//		glMatrixMode(GL_PROJECTION);//Изменяем матрицу на проекционную для работы с данными этой самой матрицы
//		glLoadIdentity();//Загружаем матрицу
//		glViewport(0, 0, (GLsizei)w, (GLsizei)h);
//		gluPerspective(45.0f, ratio, 0.1f, 90.0f);
//		glMatrixMode(GL_MODELVIEW);//Возвращаем матрицу к нормальному виду
//		glLoadIdentity();//Загружаем матрицу
//	}
//
//void InitTexture(unsigned int& texture1)
//{
//	GLfloat mat_specular[] = { 1, 1, 1, 1 };
//	glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, mat_specular);
//	glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, 128.0);
//	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
//
//	AUX_RGBImageRec* image1 = auxDIBImageLoad("22.bmp");
//	if (image1 == 0) exit(1);
//
//	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
//	glPixelZoom(1, 1);
//	glGenTextures(1, &texture1);
//
//
//	glBindTexture(GL_TEXTURE_2D, texture1);
//	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
//	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
//	gluBuild2DMipmaps(GL_TEXTURE_2D, GL_RGB, image1->sizeX, image1->sizeY, GL_RGB, GL_UNSIGNED_BYTE, image1->data);
//}
//
//void specialKeys(int key, int x, int y) {
//
//	if (key == GLUT_KEY_RIGHT)
//		rotate_y -= 5;
//
//	else if (key == GLUT_KEY_LEFT)
//		rotate_y += 5;
//
//	else if (key == GLUT_KEY_UP)
//		rotate_x += 5;
//
//	else if (key == GLUT_KEY_DOWN)
//		rotate_x -= 5;
//
//	glutPostRedisplay();
//
//}
//
//
//void Initialization() {
//	glClear(GL_COLOR_BUFFER_BIT);
//	glClearColor(1.0, 1.0, 1.0, 0);
//	glMatrixMode(GL_PROJECTION);
//	glLoadIdentity();
//	glShadeModel(GL_FLAT);
//	glEnable(GL_CULL_FACE);
//
//
//	glMatrixMode(GL_MODELVIEW);
//}
//
//void displays() {
//	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
//	glClearColor(1.0, 1.0, 1.0, 0.0);
//
//	glEnable(GL_TEXTURE_2D);
//	glBindTexture(GL_TEXTURE_2D, Textures[0]);
//
//	glLoadIdentity();
//
//	gluLookAt(0.0f, 10.0f, 20.0f,
//			0.0f, 0.0f, 0.0f,
//			0.0f, 5.0f, 4.0f);
//	
//	
//	glRotatef(rotate_x, 1.0, 0.0, 0.0);
//	glRotatef(rotate_y, 0.0, 1.0, 0.0);
//	//"Б"
//
//	/*glColor3d(1, 0, 0);
//	glBegin(GL_QUAD_STRIP);
//	glNormal3f(0.0, 0.0, 1.0);
//	glVertex3f(-2.1, 4.6, 1.01); glTexCoord2d(0, 0);
//	glVertex3f(-1, 4.6, 1.01); glTexCoord2d(0, 1);
//	glVertex3f(-2.1, -2.1, 1.01); glTexCoord2d(1, 1);
//	glVertex3f(-1, -2.1, 1.01); glTexCoord2d(1, 0);
//	glEnd();
//
//	glBegin(GL_QUAD_STRIP);
//	glColor3d(0, 1, 0);
//	glNormal3f(0.0, 0.0, 1.0);
//	glVertex3f(-2.1, 4.6, -0.01); glTexCoord2d(0, 0);
//	glVertex3f(-1, 4.6, -0.01); glTexCoord2d(0, 1);
//	glVertex3f(-2.1, -2.1, -0.01); glTexCoord2d(1, 1);
//	glVertex3f(-1, -2.1, -0.01); glTexCoord2d(1, 0);
//	glEnd();
//
//	glBegin(GL_QUAD_STRIP);
//	glColor3d(0, 0, 1);
//	glNormal3f(-2.1, 0.0, 0.0);
//	glVertex3f(-2.1, 4.6, -0.01); glTexCoord2d(0, 0);
//	glVertex3f(-2.1, 4.6, 1.01); glTexCoord2d(0, 1);
//	glVertex3f(-2.1, -2.1, -0.01); glTexCoord2d(1, 1);
//	glVertex3f(-2.1, -2.1, 1.01); glTexCoord2d(1, 0);
//	glEnd();
//
//	glBegin(GL_QUAD_STRIP);
//	glColor3d(0, 0, 1);
//	glNormal3f(-1.1, 0.0, 0.0);
//	glVertex3f(-2.1, 4.6, -0.01); glTexCoord2d(0, 0);
//	glVertex3f(-2.1, 4.6, 1.01); glTexCoord2d(0, 1);
//	glVertex3f(1.5, 4.6, -0.01); glTexCoord2d(1, 1);
//	glVertex3f(1.5, 4.6, 1.01); glTexCoord2d(1, 0);
//	glEnd();
//
//	glBegin(GL_QUAD_STRIP);
//	glColor3d(0, 0, 1);
//	glNormal3f(-1.1, 0.0, 0.0);
//	glVertex3f(-1.1, 4.6, -0.01); glTexCoord2d(0, 0);
//	glVertex3f(-1.1, 4.6, 1.01); glTexCoord2d(0, 1);
//	glVertex3f(-1.1, -2.1, -0.01); glTexCoord2d(1, 1);
//	glVertex3f(-1.1, -2.1, 1.01); glTexCoord2d(1, 0);
//	glEnd();
//
//	glColor3d(1, 0, 0);
//	glBegin(GL_QUAD_STRIP);
//	glNormal3f(0.0, 0.0, 1.0);
//	glVertex3f(-2.1, 4.6, 1.01); glTexCoord2d(0, 0);
//	glVertex3f(1.5, 4.6, 1.01); glTexCoord2d(0, 1);
//	glVertex3f(-2.1, 3.6, 1.01); glTexCoord2d(1, 1);
//	glVertex3f(1.5, 3.6, 1.01); glTexCoord2d(1, 0);
//	glEnd();
//
//	glColor3d(1, 0, 0);
//	glBegin(GL_QUAD_STRIP);
//	glNormal3f(0.0, 0.0, 1.0);
//	glVertex3f(-2.1, 4.6, -0.01); glTexCoord2d(0, 0);
//	glVertex3f(1.5, 4.6, -0.01); glTexCoord2d(0, 1);
//	glVertex3f(-2.1, 3.6, -0.01); glTexCoord2d(1, 1);
//	glVertex3f(1.5, 3.6, -0.01); glTexCoord2d(1, 0);
//	glEnd();
//
//	glBegin(GL_QUAD_STRIP);
//	glColor3d(1, 0, 1);
//	glNormal3f(-1.1, 0.0, 0.0);
//	glVertex3f(-2.1, 3.6, -0.01); glTexCoord2d(0, 0);
//	glVertex3f(-2.1, 3.6, 1.01); glTexCoord2d(0, 1);
//	glVertex3f(1.5, 3.6, -0.01); glTexCoord2d(1, 1);
//	glVertex3f(1.5, 3.6, 1.01); glTexCoord2d(1, 0);
//	glEnd();
//
//	glBegin(GL_QUAD_STRIP);
//	glColor3d(1, 0, 1);
//	glNormal3f(-1.1, 0.0, 0.0);
//	glVertex3f(-2.1, -2.1, -0.01); glTexCoord2d(0, 0);
//	glVertex3f(-2.1, -2.1, 1.01); glTexCoord2d(0, 1);
//	glVertex3f(-1.1, -2.1, -0.01); glTexCoord2d(1, 1);
//	glVertex3f(-1.1, -2.1, 1.01); glTexCoord2d(1, 0);
//	glEnd();
//
//	glBegin(GL_QUAD_STRIP);
//	glColor3d(0, 0, 1);
//	glNormal3f(-2.1, 0.0, 0.0);
//	glVertex3f(1.5, 4.6, -0.01); glTexCoord2d(0, 0);
//	glVertex3f(1.5, 4.6, 1.01); glTexCoord2d(0, 1);
//	glVertex3f(1.5, 3.6, -0.01); glTexCoord2d(1, 1);
//	glVertex3f(1.5, 3.6, 1.01); glTexCoord2d(1, 0);
//	glEnd();
//	*/
//
//	/*float r1 = 1.0, r2 = 2.0, r = 1.0;
//	glBegin(GL_POINTS);
//	for (float j = 0; j < 1; j += 0.01) {
//		for (float i = 0.0; i <= 2*3.1415; i += 0.01) {
//			glTexCoord2d(sin(i)*r1, cos(i)*r1);
//			glVertex3f((sin(i)*r1), (cos(i)*r1), j);
//			glVertex3f((sin(i)*r), (cos(i)*r), 0);
//			glVertex3f((sin(i)*r), (cos(i)*r), 1);
//			glVertex3f((sin(i)*r2), (cos(i)*r2), j);
//		}
//		r += 0.01;
//	}*/
//		
//		
//		
//			float r1 = 1.5, r2 = 2.5, r = 1.5;
//			float threePI = 3 * 3.1415, twoPI = 2*3.1415;
//	//"З"
//			glBegin(GL_POINTS);
//			for (float j = 0; j < 1; j += 0.01) {
//				for (float i = 0.0; i <= threePI / 2; i += 0.01) {
//					glTexCoord2d(sin(i)*r1, cos(i)*r1);
//					glVertex3f((sin(i)*r1), (cos(i)*r1), j);
//					glVertex3f((sin(i)*r), (cos(i)*r), 0);
//					glVertex3f((sin(i)*r), (cos(i)*r), 1);
//					glVertex3f((sin(i)*r2), (cos(i)*r2), j);
//				}
//				r += 0.01;
//			}
//			r = 1.5;
//			for (float j = 0; j < 1; j += 0.01) {
//				for (float i = twoPI / 2; i >= -threePI/12; i -= 0.01) {
//					glTexCoord2d(sin(i)*r1, cos(i)*r1);
//					glVertex3f((sin(i)*r1), 2 * r1 + (cos(i)*r1) + 1, j);
//					glVertex3f((sin(i)*r), 2 * r1 + (cos(i)*r) + 1, 0);
//					glVertex3f((sin(i)*r2), 2 * r2 + (cos(i)*r2) - 1, j);
//					glVertex3f((sin(i)*r), 2 * r1 + (cos(i)*r) + 1, 1);
//				}
//				r += 0.01;
//			}
//			glEnd();
//			glFlush();
//		
//	glutSwapBuffers();
//}
//
//
//int main(int argc, char* argv[]) {
//
//	glutInit(&argc, argv);
//	glutInitDisplayMode(GLUT_SINGLE | GLUT_RGBA);
//	glutInitWindowSize(1000, 1000);
//
//	glutCreateWindow("8");
//	glClearColor(1.0, 1.0, 1.0, 0);
//	glEnable(GL_DEPTH_TEST);
//	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL);
//	InitTexture(Textures[0]);
//	glutDisplayFunc(displays);
//	glutMotionFunc(MouseMove);
//	glutSpecialFunc(specialKeys);
//	glutReshapeFunc(resize);
//	glutMainLoop();
//
//
//	return 0;
//}

