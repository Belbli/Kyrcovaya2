#define _CRT_SECURE_NO_WARNINGS
#include<GL/glut.h>
#include"Button.h"
#include<math.h>
#include<vector>
#include<iostream>
#include<ctime>
using namespace std;

//#define width 480
//#define height 600
#define PLAYBTN 1
#define RECORDSBTN 2
#define EXITBTN 0
#define MENUPROC 100
#define GAMEPROC 101
#define SUBMENUPROC 102
#define SELECTLVLPROC 103
#define INITPROC 104
#define EXIT 110

using namespace std;

Button playBtn, recordsBtn, exitBtn , nextLvlBtn, menuBtn, replayBtn, lvlBtns[10];
int width = 480, height = 600;
int x = -1, y = -1, btnWidth = 150, btnState,btnStart, btnPressed = -1, plateWidth = width/5, xLeftPlatePos, xRightPlatePos, lifes = 3;
int xMousePos = 0, yPlatePos = 3 * height / 4 + 5, yPrevPlatePos = 0;
int blockAmount = 0, score = 0;
bool destroyWnd = false, nextLvl = false;
float xAngle = 2.0, yAngle = 1.0;
int blockHeight = 20, blockWidth = 50, process = INITPROC, lvlsPassed, lvls, bonusAmount = 0, ballsAmount = 0;
//50 20

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

vector<block> blocks;
vector<Bonus> bonuses;
vector<Ball> balls;

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

void init(){
	glClearColor(0.8, 0.7, 0.7, 1.0);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(0, width, height, 0, 0, 1);
	glMatrixMode(GL_MODELVIEW);
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
	glutCreateWindow("OpenGL");
	glutSetCursor(GLUT_CURSOR_LEFT_ARROW);
	glutDisplayFunc(display);
	glutReshapeFunc(reshape);
	glutPassiveMotionFunc(MouseMove);
	glutMouseFunc(MouseMoveClick);
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

void renderBM(float x, float y, void *font, char *str){
	char *c;
	glRasterPos2f(x, y);
	for (c = str; *c != '\0'; c++) {
		glutBitmapCharacter(font, *c);
	}
}

void printText(int xPos, int yPos, int info){
	glColor3f(1.0, 1.0, 1.0);
	char str[15];
	_itoa(info, str, 10);
	renderBM(xPos, yPos, GLUT_BITMAP_TIMES_ROMAN_24, str);
}

void printText(char *str) {
	glColor3f(1.0, 1.0, 1.0);
	renderBM(width / 2 - 80, height / 2, GLUT_BITMAP_TIMES_ROMAN_24, str);
}

void setLvlsInfo() {//Считывание кол-ва пройденных уровней и общего кол-ва уровней
	lvlsPassed = readFile("progress.txt");
	lvls = readFile("lvlsAmount.txt");
	cout << "Completed";
}

bool isLvlOpen(int lvl) {
	if (lvl <= lvlsPassed)
		return true;
	return false;
}

void saveProgress() {
	FILE *file = fopen("progress.txt", "w+");
	if (file != NULL) {
		fprintf(file, "%d", lvlsPassed);
		fclose(file);
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
		fclose(file);
		return dest;
	}
	else
		process = SELECTLVLPROC;
	return NULL;
}

void initMenuButtons() {
	int btnHeight = 40;
	int lineSpace = btnHeight + 30;

	playBtn.setButtonPosition(width / 2 - btnWidth / 2, height / 4, btnWidth, btnHeight);
	recordsBtn.setButtonPosition(width / 2 - btnWidth / 2, height / 4 + lineSpace, btnWidth, btnHeight);
	exitBtn.setButtonPosition(width / 2 - btnWidth / 2, height / 4 + 2 * lineSpace, btnWidth, btnHeight);
}

void showMenu(){
	initMenuButtons();
	playBtn.CreateButton(PLAYBTN);
	recordsBtn.CreateButton(RECORDSBTN);
	exitBtn.CreateButton(EXITBTN);
}

void initSubMenuButtons() {
	int subMenuBtnH = 40, subMenuBtnW = 80;
	menuBtn.setButtonPosition(width / 2 - subMenuBtnW / 2, height / 2, subMenuBtnW, subMenuBtnH);
	nextLvlBtn.setButtonPosition(width / 2 + subMenuBtnW, height / 2, subMenuBtnW, subMenuBtnH);
}

void addSubMenuButtons() {
	nextLvlBtn.CreateButton(16);
	menuBtn.CreateButton(15);
}

void subMenu() {
	char *lvlPath;
	glutSetCursor(GLUT_CURSOR_LEFT_ARROW);
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
	int lvlBtnH = 60, lvlBtnW = 80, xSpacing = 100, ySpacing = 0, t = 0;
	for (int i = 0; i < 10; i++) {
		if ((i) % 2 == 0) {
			ySpacing += 3 * lvlBtnH / 2;
			xSpacing = 0;
			t = 0;
		}
		xSpacing += t * (lvlBtnW + 10);
		lvlBtns[i].setButtonPosition(xSpacing, ySpacing, lvlBtnW, lvlBtnH);
		t++;
	}
}

void addLvlMenu() {
	for (int i = 0; i < 10; i++)
		lvlBtns[i].CreateButton(i + 1);
}

void menu(){
	showMenu();
	if (playBtn.isClicked(x, y) && btnStart == GLUT_LEFT_BUTTON && btnState == GLUT_UP) {
		process = SELECTLVLPROC;
		btnStart = -1;
		score = 0;
	}
	else if (exitBtn.isClicked(x, y) && btnStart == GLUT_LEFT_BUTTON && btnState == GLUT_UP)
		process = EXIT;
	
}

void initFunctions() {
	initLvlMenu();
	initMenuButtons();
	setLvlsInfo();
	initSubMenuButtons();
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

void collision(){
	int index;
	for (int i = 0; i < ballsAmount; i++) {
		for (int j = 0; j < blockAmount; j++) {
			if (balls[i].y + balls[i].radius >= blocks[j].yb && balls[i].y - balls[i].radius <= blocks[j].yb + blockHeight && balls[i].x + balls[i].radius >= blocks[j].xb && balls[i].x - balls[i].radius <= blocks[j].xb + blockWidth) {
				if (!verticalRebound(j))
					xAngle *= -1;
				blocks[j].durability--;
				score += 20;
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
	bonus.bonusType = rand() % 4;
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

void selectLvl() {
	char lvlPath[10] = "lvl_", lvl[4] = "";
	if (btnStart == GLUT_LEFT_BUTTON) {
		for (int i = 0; i < 10; i++) {
			if (lvlBtns[i].isClicked(x, y) && btnState == GLUT_UP) {
				_itoa(i + 1, lvl, 10);
				if (isLvlOpen(i+1)) {
					if (lvlsPassed = i + 1)
						nextLvl = true;
					strcat(lvlPath, lvl);
					strcat(lvlPath, ".txt");
					process = GAMEPROC;
					btnStart = -1;
					lifes = 3;
					initBlocks(lvlPath);
					initBonuses();
					addBall();
					glutSetCursor(GLUT_CURSOR_NONE);
				}
				else process = SELECTLVLPROC;
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

void lvlPassed() {
	process = SUBMENUPROC;
	balls.clear();
	ballsAmount = 0;
	blocks.clear();
	blockAmount = 0;
	if(nextLvl)
		lvlsPassed++;
	nextLvl = false;
	yAngle = 1.0;
	saveProgress();
	bonuses.clear();
	bonusAmount = 0;
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
		lvlPassed();
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
			if (balls[i].y + balls[i].radius == yPlatePos && balls[i].x >= xLeftPlatePos && balls[i].x <= xRightPlatePos + balls[i].radius) {
				float angle = abs(xRightPlatePos - plateWidth / 2 - balls[i].x) / 10 + 1;
				if (xAngle < 0)
					xAngle = -angle;
				else
					xAngle = angle;
				reboundDir();
			}
			if (balls[i].x <= 0 || balls[i].x >= width)
				xAngle *= -1;

			if (balls[i].y <= 0)
				yAngle *= -1;
			if (balls[i].y > yPlatePos + 40) {
				balls[i].move = false;
				btnStart = -1;
				lifes--;
				if (lifes == 0)
					process = SUBMENUPROC;
			}
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

void drawBonus(int bonusIndex) {
	if (bonuses[bonusIndex].y < yPlatePos + 10) {
		glColor3f(0.0, 1.0, 0.0);
		glPointSize(20);
		glBegin(GL_POINTS);
		glVertex2f(bonuses[bonusIndex].x, bonuses[bonusIndex].y);
		glEnd();
		bonuses[bonusIndex].y++;
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

void game() {
	printGameInfo();
	collision();
	drawBall();
	activeBonuses();
	ViewBlocks();
	drawPlate();
	
}

void display(){
	ShowCursor(true);
	glClear(GL_COLOR_BUFFER_BIT);
	glLoadIdentity();
	switch (process) {
		case MENUPROC:
			menu();
			break;
		case SELECTLVLPROC:
			addLvlMenu();
			selectLvl();
			break;
		case SUBMENUPROC:
			subMenu();
			break;
		case INITPROC:
			initFunctions();
			break;
		case GAMEPROC:
			ShowCursor(false);
			game();
			break;
		case EXIT:
			glutDestroyWindow(1);
			break;
	}
	if(process != EXIT)
		glutSwapBuffers();
}

void reshape(int w, int h){
	int wh = w / h;

	width = w;
	height = h;

	glViewport(0, 0, w, h);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	//if (w <= h)
	//	gluOrtho2D(0, width, height * wh, 0);
	//else 
	//	gluOrtho2D(0, width * wh, height, 0);
	//gluPerspective(360, wh, 1, 10);
	gluOrtho2D(0, width, height, 0);
	glMatrixMode(GL_MODELVIEW);
	plateWidth = width / 4;
	yPlatePos = 2 * height / 3;
}