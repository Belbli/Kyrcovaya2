#define _CRT_SECURE_NO_WARNINGS
#include<GL/glut.h>
#include"Button.h"
#include<math.h>
#include<vector>
#include<iostream>
#include<soil.h>
#include<ctime>
using namespace std;


#define PLAYBTN 1
#define RECORDSBTN 2
#define EXITBTN 0
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

Button playBtn, recordsBtn, exitBtn , nextLvlBtn, menuBtn, replayBtn, lvlBtns[10], gameModeButtons[2];
int width = 480, height = 600;
int x = -1, y = -1, btnWidth = 150, btnState,btnStart, btnPressed = -1, plateWidth = width/5, xLeftPlatePos, xRightPlatePos, lifes = 3;
int xMousePos = 0, yPlatePos = 3 * height / 4 + 5, yPrevPlatePos = 0;
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
void saveProgress();
void addRecord();
void saveRecords();
void keyBoardFunc(unsigned char key, int x, int y);
void clearData();

void init(){
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(0, width, height, 0, 0, 1);
	glMatrixMode(GL_MODELVIEW);
	
}

GLuint bgTexture, playBtnTex, recBtnTex, exitBtnTex, menuBtnTex, lvlBtnTex[10], normalBtnTex, trainBtnTex;

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

void loadImages() {
	bgTexture = load_texture("2.png");
	menuBtnTex = load_texture("menuBtn.jpg");
	playBtnTex = load_texture("playBtn.jpg");
	recBtnTex = load_texture("recBtn.jpg");
	exitBtnTex = load_texture("exitBtn.jpg");
	trainBtnTex = load_texture("trainModeBtn.jpg");
	normalBtnTex = load_texture("normalModeBtn.jpg");
	loadLvlBtnsTextures();
}

void MouseMoveClick(int btn, int state, int ax, int ay){
	x = ax;
	y = ay;
	btnStart = btn;
	btnState = state;
}

int main(int argc, char *argv[]){
	srand(time(NULL));
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE);
	glutInitWindowPosition(50, 50);
	glutInitWindowSize(width, height);
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
	static int x = width/2, s = 0;
	printText(width / 2 + 100, height / 4, "������� ��� ������� : ");
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
	printText(x, height/2, record.name);
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

void setLvlsInfo() {//���������� ���-�� ���������� ������� � ������ ���-�� �������
	lvlsPassed = readFile("progress.txt");
	lvls = readFile("lvlsAmount.txt");
}

void saveProgress() {
	FILE *file = fopen("progress.txt", "w+");
	if (file != NULL) {
		fprintf(file, "%d", lvlsPassed);
		fclose(file);
	}
}

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
	int xPos = 60, yPos = 40;
	if (recordsRows > 0){
		for (int i = 0; i < recordsRows; i++) {
			printText(xPos, yPos, records[i].priority);
			xPos += 30;
			printText(xPos, yPos, records[i].name);
			xPos += 3 * width / 5;
			printText(xPos, yPos, records[i].score);
			yPos += 30;
			xPos = 60;
		}
	}
	else
		printText(xPos, yPos, "�������� �� �����������");
	menuBtn.CreateButton(15);
	if (menuBtn.isClicked(x, y) && btnStart == GLUT_LEFT_BUTTON && btnState == GLUT_UP) {
		process = MENUPROC;
		btnStart = -1;
	}
}

int readFile(char *path) {
	int output = -1;
	FILE *file = fopen(path, "r");
	if (file != NULL) {
		while (fscanf(file, "%d", &output)) {
			fclose(file);
			return output;
		}
	}
	return output;
}

char *loadLvl(char *path) {
	FILE *file;
	file = fopen(path, "r");
	if (file != NULL) {
		fseek(file, 0, SEEK_END);
		int size = ftell(file);
		int buffSize = width / blockWidth;
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

void initMenuButtons() {
	int btnHeight = 40;
	int lineSpace = btnHeight + 50;
	playBtn.setButtonPosition(width / 2 - btnWidth / 2, height / 4, btnWidth, btnHeight, playBtnTex);
	recordsBtn.setButtonPosition(width / 2 - btnWidth / 2, height / 4 + lineSpace, btnWidth, btnHeight, recBtnTex);
	exitBtn.setButtonPosition(width / 2 - btnWidth / 2, height / 4 + 2 * lineSpace, btnWidth, btnHeight, exitBtnTex);
}

void showMenu(){
	initMenuButtons();
	playBtn.CreateButton(PLAYBTN);
	recordsBtn.CreateButton(RECORDSBTN);
	exitBtn.CreateButton(EXITBTN);
}

void initSubMenuButtons() {
	int subMenuBtnH = 40, subMenuBtnW = 80;
	menuBtn.setButtonPosition(width / 2 - subMenuBtnW / 2, 3*height / 4, subMenuBtnW, subMenuBtnH, menuBtnTex);
	/*nextLvlBtn.setButtonPosition(width / 2 + subMenuBtnW, height / 2, subMenuBtnW, subMenuBtnH);*/
}

void addSubMenuButtons() {
	nextLvlBtn.CreateButton(16);
	menuBtn.CreateButton(15);
}

void subMenu() {
	char *lvlPath;
	initSubMenuButtons();
	addSubMenuButtons();
	if (menuBtn.isClicked(x, y) && btnStart == GLUT_LEFT_BUTTON && btnState == GLUT_UP) {
		process = MENUPROC;
		btnStart = -1;
	}
	else if (nextLvlBtn.isClicked(x, y) && btnStart == GLUT_LEFT_BUTTON && btnState == GLUT_UP) {
		process = GAMEPROC;
		btnStart = -1;
		score = 0;
	}
}

void initLvlMenu() {
	int lvlBtnH = 60, lvlBtnW = 80, xSpacing = 120, ySpacing = 50, t = 0;
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
		lvlBtns[i].CreateButton(i + 1);
}

void initGameModeMenu() {
	int modeBtnH = 40, modeBtnW = 100, xSpacing = width / 2 - modeBtnW, ySpacing = height / 2 - modeBtnH/2;
	gameModeButtons[0].setButtonPosition(xSpacing - 20, ySpacing, modeBtnW, modeBtnH, normalBtnTex);
	xSpacing += 30 + modeBtnW;
	gameModeButtons[1].setButtonPosition(xSpacing - 20, ySpacing, modeBtnW, modeBtnH, trainBtnTex);
}

void showGameModeMenu() {
	for (int i = 0; i < 2; i++) {
		gameModeButtons[i].CreateButton(i + 10);
	}
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
		if (balls[i].y + balls[i].radius == blocks[blockIndex].yb && balls[i].x >= blocks[blockIndex].xb && balls[i].x <= blocks[blockIndex].xb + blockWidth || balls[i].y - balls[i].radius == blocks[blockIndex].yb + blockHeight && balls[i].x >= blocks[blockIndex].xb && balls[i].x <= blocks[blockIndex].xb + blockWidth) {
			yAngle *= -1;
			return true;
		}
	}
	return false;
}

bool hasBonus(int blockNum, int &index) {
	for (int i = 0; i < bonusAmount; i++) {
		if (blocks[blockNum].xb == bonuses[i].x && blocks[blockNum].yb == bonuses[i].y) {
			index = i;
			return true;
		}
	}
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
			if (balls[i].y + balls[i].radius >= blocks[j].yb && balls[i].y - balls[i].radius <= blocks[j].yb + blockHeight && balls[i].x + balls[i].radius >= blocks[j].xb && balls[i].x - balls[i].radius <= blocks[j].xb + blockWidth) {
				if (!verticalRebound(j))
					xAngle *= -1;
				blocks[j].durability--;
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

void setBlockAmount(char *lvl) {
	for (int i = 0; i < strlen(lvl); i++) {
		if (lvl[i] != '0' && lvl[i] != '\n')
			blockAmount++;
	}
}

void addBall() {
	ball.x = xLeftPlatePos + plateWidth / 2;
	ball.y = yPlatePos - ball.radius;
	ball.move = false;
	balls.push_back(ball);
	ballsAmount++;
}

bool isLvlOpen(int lvl) {
	if (lvl <= lvlsPassed)
		return true;
	return false;
}

void loadLvl(int i) {
	char lvlPath[10] = "lvl_", lvl[4] = "";
	_itoa(i + 1, lvl, 10);
		strcat(lvlPath, lvl);
		strcat(lvlPath, ".txt");
		process = GAMEPROC;
		btnStart = -1;
		if(mode == TRAINMODE || i == 0)
		lifes = 3;
		initBlocks(lvlPath);
		initBonuses();
		addBall();
		glutSetCursor(GLUT_CURSOR_NONE);
}

void selectLvl() {
	if (btnStart == GLUT_LEFT_BUTTON) {
		for (int i = 0; i < 10; i++) {
			if (lvlBtns[i].isClicked(x, y) && btnState == GLUT_UP) {
				if (isLvlOpen(i + 1)) {
					if (lvlsPassed = i + 1)
						nextLvl = true;
					loadLvl(i);
				}
				break;
			}
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
				blocks.push_back(item);
				t++;
				xSpasing += blockWidth + 5;
			}
			if (lvlMap[i] == '0')
				xSpasing += blockWidth + 5;
		}
	}
}

void switchColor(int color) {
	if (color == 1) 
		glColor3f(1.0, 0.0, 0.0);
	else if (color == 2) 
		glColor3f(0.7, 0.0, 0.0);
	else if (color == 3) 
		glColor3f(0.5, 0.0, 0.0);
	else
		glColor3f(0.3, 0.0, 0.0);
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
		if (nextLvl)
			lvlsPassed++;
		nextLvl = false;
		process = TRAINMODE;
		score = 0;
	}
	yAngle = 1.0;
	saveProgress();
	clearData();
}

void ViewBlocks(){
	for (int i = 0; i < blockAmount; i++){
		switchColor(blocks[i].durability);
		glBegin(GL_POLYGON);
		glVertex2f(blocks[i].xb, blocks[i].yb);
		glVertex2f(blocks[i].xb, blocks[i].yb + blockHeight);
		glVertex2f(blocks[i].xb + blockWidth, blocks[i].yb + blockHeight);
		glVertex2f(blocks[i].xb + blockWidth, blocks[i].yb);
		glEnd();
	}
	if (blockAmount == 0) {
		lvlPassed(mode);
	}
}

void platePosition() {
	yPrevPlatePos = xLeftPlatePos;
	if (xMousePos >= plateWidth / 2 && xMousePos <= width - plateWidth / 2) {
		xLeftPlatePos = xMousePos - plateWidth / 2;
		xRightPlatePos = xMousePos + plateWidth / 2;
	}
	else if (xMousePos <= plateWidth / 2) {
		xLeftPlatePos = 0;
		xRightPlatePos = plateWidth;
	}
	else {
		xLeftPlatePos = width - plateWidth;
		xRightPlatePos = width;
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
	if (x - r <= 0 || x + r >= width)
		xAngle *= -1;
	if (y + r <= 0)
		yAngle *= -1;
}

bool ballLose(int x, int y) {
	if (y > yPlatePos + 40) {
		btnStart = -1;
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
	printText(width / 2, height - 60, score);
	printText(width / 2, height - 30, lifes);
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
		collision();
		drawBall();
		activeBonuses();
		ViewBlocks();
		drawPlate();
	}
	else {
		if (isRecord() && mode == NORMALMODE) {
			process = ENTERTEXT;
			strcpy(record.name, "");
		}
		else
			process = MENUPROC;
		clearData();
	}
}

void display(){
	
	glClear(GL_COLOR_BUFFER_BIT);
	glLoadIdentity();
	drawTexture(0, 0, width, height, bgTexture);
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

	width = w;
	height = h;

	glViewport(0, 0, w, h);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluOrtho2D(0, width, height, 0);
	glMatrixMode(GL_MODELVIEW);
	plateWidth = width / 4;
	yPlatePos = 2 * height / 3;
}