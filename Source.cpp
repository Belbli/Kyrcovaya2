#define _CRT_SECURE_NO_WARNINGS
#include<GL/glut.h>
#include"Button.h"
#include<math.h>
#include<vector>
#include<iostream>
using namespace std;

#define WIDTH 480
#define HEIGHT 600
#define PLAYBTN 1
#define RECORDSBTN 2
#define EXITBTN 0
#define MENUPROC 100
#define GAMEPROC 101
#define SUBMENUPROC 102
#define EXIT 110

using namespace std;

Button playBtn, recordsBtn, exitBtn , nextLvlBtn, menuBtn, replayBtn;

int x = -1, y = -1, btnWidth = 150, btnState,btnStart, btnPressed = -1, plateWidth = 80, xLeftPlatePos, xRightPlatePos;
int xMousePos = 0, yPlatePos = 3 * HEIGHT / 4 + 5, xBallPos = 350, yBallPos = yPlatePos, yPrevPlatePos = 0, ballR = 7;
int blockAmount = 0, score = 0;
bool destroyWnd = false, ballMove = false;
float xAngle = -2.0, yAngle = -1.0;
int blockHeight = 120, blockWidth = 150, process = 100;
//50 20
struct block {
	int xb;
	int yb;
	int durability;
} item;

vector<block> blocks;

void MouseMove(int ax, int ay){
	xMousePos = ax;
}

void display();
void timer(int);
void reshape(int, int);
void initBlocks();

void init(){
	glClearColor(0.8, 0.7, 0.7, 1.0);
	glOrtho(-360, 360, -360, 360, -100, 100);
}

void MouseMoveClick(int btn, int state, int ax, int ay){
	x = ax;
	y = ay;
	btnStart = btn;
	btnState = state;
}

int main(int argc, char *argv[]){
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE);

	glutInitWindowPosition(50, 50);
	glutInitWindowSize(WIDTH, HEIGHT);
	glutCreateWindow("OpenGL");
	glutDisplayFunc(display);
	glutReshapeFunc(reshape);
	glutPassiveMotionFunc(MouseMove);
	glutMouseFunc(MouseMoveClick);
	glutTimerFunc(0, timer, 0);
	init();
	glutMainLoop();
}

void timer(int){
	glutPostRedisplay();
	glutTimerFunc(1000 / 100, timer, 0);
}

void renderBM(float x, float y, void *font, char *str){
	char *c;
	glRasterPos2f(x, y);
	for (c = str; *c != '\0'; c++) {
		glutBitmapCharacter(font, *c);
	}
}

void printText(int score){
	glColor3f(1.0, 1.0, 1.0);
	char str[15];
	_itoa(score, str, 10);
	renderBM(WIDTH/2, yPlatePos + 100, GLUT_BITMAP_TIMES_ROMAN_24, str);
}

void printText(char *str) {
	glColor3f(1.0, 1.0, 1.0);
	renderBM(WIDTH / 2 - 80, HEIGHT / 2, GLUT_BITMAP_TIMES_ROMAN_24, str);
}

char *loadLevel(char *path) {
	FILE *file;
	file = fopen(path, "r");

	fseek(file, 0, SEEK_END);
	int size = ftell(file);
	int buffSize = WIDTH / blockWidth;
	fseek(file, 0, SEEK_SET);
	char *buff = (char*)calloc(buffSize, sizeof(char));
	char *dest = (char*)calloc(size, sizeof(char));
	while (fgets(buff, buffSize, file) != NULL) {
		//fgets(buff, buffSize, file);
		strcat(dest, buff);
	}
	return dest;
}

void initMenuButtons() {
	int btnHeight = 40;
	int lineSpace = btnHeight + 30;

	playBtn.setButtonPosition(WIDTH / 2 - btnWidth / 2, HEIGHT / 4, btnWidth, btnHeight);
	recordsBtn.setButtonPosition(WIDTH / 2 - btnWidth / 2, HEIGHT / 4 + lineSpace, btnWidth, btnHeight);
	exitBtn.setButtonPosition(WIDTH / 2 - btnWidth / 2, HEIGHT / 4 + 2 * lineSpace, btnWidth, btnHeight);
}

void showMenu(){
	initMenuButtons();
	playBtn.CreateButton(PLAYBTN);
	recordsBtn.CreateButton(RECORDSBTN);
	exitBtn.CreateButton(EXITBTN);
}

void initSubMenuButtons() {
	int subMenuBtnH = 40, subMenuBtnW = 80;
	menuBtn.setButtonPosition(WIDTH / 2 - subMenuBtnW / 2, HEIGHT / 2, subMenuBtnW, subMenuBtnH);
	nextLvlBtn.setButtonPosition(WIDTH / 2 + subMenuBtnW, HEIGHT / 2, subMenuBtnW, subMenuBtnH);
}

void addSubMenuButtons() {
	nextLvlBtn.CreateButton(16);
	menuBtn.CreateButton(15);
}

void subMenu() {
	char *lvlPath;
	ShowCursor(true);
	glClear(GL_COLOR_BUFFER_BIT);
	initSubMenuButtons();
	addSubMenuButtons();
	if (menuBtn.isClicked(x, y) && btnStart == GLUT_LEFT_BUTTON && btnState == GLUT_UP) {
		process = MENUPROC;
		btnStart = -1;
	}
	else if (nextLvlBtn.isClicked(x, y) && btnStart == GLUT_LEFT_BUTTON && btnState == GLUT_UP) {
		initBlocks();
		process = GAMEPROC;
		btnStart = -1;
		ballMove = false;
		score = 0;
	}
}

void menu(){
	char *lvlPath = NULL;
	ShowCursor(true);
	if (process == MENUPROC){
		showMenu();
		if (playBtn.isClicked(x, y) && btnStart == GLUT_LEFT_BUTTON && btnState == GLUT_UP) {
			process = GAMEPROC;
			glClear(GL_COLOR_BUFFER_BIT);
			btnStart = -1;
			initBlocks();
			score = 0;
		}
		else if (exitBtn.isClicked(x, y) && btnStart == GLUT_LEFT_BUTTON && btnState == GLUT_UP)
			process = EXIT;
	}
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
	if (yBallPos + ballR == blocks[blockIndex].yb && xBallPos >= blocks[blockIndex].xb && xBallPos <= blocks[blockIndex].xb + blockWidth || yBallPos - ballR == blocks[blockIndex].yb + blockHeight && xBallPos >= blocks[blockIndex].xb && xBallPos <= blocks[blockIndex].xb + blockWidth){
		yAngle *= -1;
		return true;
	}
	return false;
}

void collision(){
	for (int i = 0; i < blockAmount; i++){
		if (yBallPos + ballR >= blocks[i].yb && yBallPos - ballR <= blocks[i].yb + blockHeight && xBallPos + ballR >= blocks[i].xb && xBallPos - ballR <= blocks[i].xb + blockWidth) {
			if (!verticalRebound(i))
				xAngle *= -1;
			blocks[i].durability--;
			score += 20;
		}
		if (blocks[i].durability == 0) {
			blocks.erase(blocks.begin() + i);
			blockAmount--;
		}
	}
}

void setBlockAmount(char *lvl) {
	for (int i = 0; i < strlen(lvl); i++) {
		if (lvl[i] != '0' && lvl[i] != '\n')
			blockAmount++;
	}
}

void initBlocks() {
	char *lvlMap = loadLevel("lvl_11.txt");
	int ySpacing = 0, t = 0, xSpasing = 0;
	setBlockAmount(lvlMap);
	if (blockAmount > 0)
		ySpacing = 0;
	for (int i = 0; i < strlen(lvlMap); i++){
		if (lvlMap[i] == '\n') {
			ySpacing += blockHeight + 5;
			t = 0;
			xSpasing = 0;
		}
		if(lvlMap[i] != '0' && lvlMap[i] != '\n') {
			item.xb = xSpasing;
			item.yb = ySpacing;
			item.durability = lvlMap[i] - 48;
			blocks.push_back(item);
			t++;
			xSpasing += blockWidth + 5;
		}
		if(lvlMap[i] == '0')
			xSpasing += blockWidth + 5;
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
		glClear(GL_COLOR_BUFFER_BIT);
		process = SUBMENUPROC;
		ShowCursor(true);
		ballMove = false;
		blocks.clear();
		blockAmount = 0;
	}
}

void platePosition() {
	yPrevPlatePos = xLeftPlatePos;
	if (xMousePos >= plateWidth / 2 && xMousePos <= WIDTH - plateWidth / 2) {
		xLeftPlatePos = xMousePos - plateWidth / 2;
		xRightPlatePos = xMousePos + plateWidth / 2;
	}
	else if (xMousePos <= plateWidth / 2) {
		xLeftPlatePos = 0;
		xRightPlatePos = plateWidth;
	}
	else {
		xLeftPlatePos = WIDTH - plateWidth;
		xRightPlatePos = WIDTH;
	}
}

void drawPlate() {
	glBegin(GL_POLYGON);

	glColor3f(1.0, 0.0, 0.0);
	
	platePosition();

	glVertex2f(xLeftPlatePos, 20.0 + yPlatePos);
	glVertex2f(xLeftPlatePos, 0.0 + yPlatePos);
	glVertex2f(xRightPlatePos, 0.0 + yPlatePos);
	glVertex2f(xRightPlatePos, 20.0 + yPlatePos);

	glEnd();
}

void ballMotion(){
	if (btnStart == GLUT_LEFT_BUTTON) {
		ballMove = true;
		btnStart = -1;
	}
	if (ballMove == false) {
		xBallPos = xLeftPlatePos + plateWidth / 2;
		yBallPos = yPlatePos - 5;
	}
	else{
		if (yBallPos == yPlatePos && xBallPos >= xLeftPlatePos && xBallPos <= xRightPlatePos + 10) {
			float angle = abs(xRightPlatePos - plateWidth / 2 - xBallPos) / 10 + 1;
			if (xAngle < 0)
				xAngle = -angle;
			else
				xAngle = angle;
			reboundDir();
		}
		if (xBallPos <= 0 || xBallPos >= WIDTH)
			xAngle *= -1;

		if (yBallPos <= 0)
			yAngle *= -1;
		if (yBallPos > yPlatePos + 15) {
			ballMove = false;
			btnStart = -1;
		}
		xBallPos += xAngle;
		yBallPos += yAngle;
	}
}

void drawBall(){
	ballMotion();
	glColor3f(0.0, 1.0, 0.0);
	glBegin(GL_TRIANGLE_FAN);
	for (int i = 0; i <= 50; i++) {
		float angle = 2.0 * 3.1415926 * float(i) / float(50);
		float dx = ballR * cosf(angle);
		float dy = ballR * sinf(angle);
		glVertex2f(xBallPos + dx, yBallPos + dy);
	}
	glEnd();
}

void game() {
	ShowCursor(false);
	printText(score);
	collision();
	ViewBlocks();
	drawPlate();
	drawBall();
}

void display(){
	glClear(GL_COLOR_BUFFER_BIT);
	glLoadIdentity();
	switch (process) {
		case MENUPROC:
			menu();
			break;
		case SUBMENUPROC:
			subMenu();
			break;
		case GAMEPROC:
			glClear(GL_COLOR_BUFFER_BIT);
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
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluOrtho2D(0, WIDTH, HEIGHT,0);
	glMatrixMode(GL_MODELVIEW);
}