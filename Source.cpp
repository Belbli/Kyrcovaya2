#define _CRT_SECURE_NO_WARNINGS
#include<GL/glut.h>
#include"Button.h"
#include<math.h>
#include<vector>
#include<iostream>
using namespace std;

#define WIDTH 480
#define HEIGHT 640
#define PLAYBTN 1
#define RECORDSBTN 2
#define EXITBTN 0
#define MENUPROC 100
#define GAMEPROC 101
#define EXIT 102

using namespace std;

Button playBtn, recordsBtn, exitBtn , nextLvlBtn, menuBtn, replayBtn;

int x = -1, y = -1, btnWidth = 150, btnState,btnStart, btnPressed = -1, plateWidth = 80, xLeftPlatePos, xRightPlatePos;
int xMousePos = 0, yPlatePos = 3 * HEIGHT / 4 + 5, xBallPos = 350, yBallPos = yPlatePos, yPrevPlatePos = 0, ballR = 7;
int blockAmount = 1, score = 0;
bool destroyWnd = false, ballMove = false;
float xAngle = -2.0, yAngle = -1.0;
int blockHeight = 150, blockWidth = 140, process = 100;

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
	//initBlocks();
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

void showMenu(){
	int btnHeight = 40;
	int lineSpace = btnHeight + 30;
	
	playBtn.setButtonPosition(WIDTH / 2 - btnWidth / 2, HEIGHT / 4, btnWidth, btnHeight);
	playBtn.CreateButton(PLAYBTN);

	recordsBtn.setButtonPosition(WIDTH / 2 - btnWidth / 2, HEIGHT / 4 + lineSpace, btnWidth, btnHeight);
	recordsBtn.CreateButton(RECORDSBTN);

	exitBtn.setButtonPosition(WIDTH / 2 - btnWidth / 2, HEIGHT / 4 + 2 * lineSpace, btnWidth, btnHeight);
	exitBtn.CreateButton(EXITBTN);
}
int t = 0;
void menu(){
	if (process == MENUPROC){
		showMenu();
		if (playBtn.isClicked(x, y) && btnStart == GLUT_LEFT_BUTTON && btnState == GLUT_UP) {
			process = GAMEPROC;
			glClear(GL_COLOR_BUFFER_BIT);
			btnStart = -1;
			t++;
			blockAmount = 1;
			initBlocks();
			
		}
		if (exitBtn.isClicked(x, y))
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
		if (blocks[i].durability == -1) {
			blocks.erase(blocks.begin() + i);
			blockAmount--;
		}
	}
}

void initBlocks(){
	blocks.reserve(blockAmount);
	for (int i = 0; i < blockAmount; i++){
		item.xb = i * blockWidth + i*5;
		item.yb = 200;
		item.durability = i % 4;
		blocks.push_back(item);
	}
}

void switchColor(int color) {
	if (color == 0) 
		glColor3f(1.0, 0.0, 0.0);
	else if (color == 1) 
		glColor3f(0.7, 0.0, 0.0);
	else if (color == 2) 
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
		process = MENUPROC;
		ShowCursor(true);
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
			ShowCursor(true);
			menu();
			break;
		case GAMEPROC:
			glClear(GL_COLOR_BUFFER_BIT);
			game();
			break;
		case EXIT:
			glutDestroyWindow(1);
			break;
	}
	/*	menu();
	if (btnPressed == 1){
		ShowCursor(false);
		printText(score);
		collision();
		ViewBlocks();

		drawPlate();
		drawBall();
	}
	if (levelPassed()) {
		ShowCursor(true);
		printText("Level Passed!");
		menuBtn.setButtonPosition(WIDTH / 2 - 70/2, HEIGHT / 2 + 50, 70, 40);
		menuBtn.CreateButton(10);
		if (menuBtn.isClicked(x, y) && btnStart == GLUT_LEFT_BUTTON && btnState == GLUT_UP) {
			menu();
			blockAmount = 10;
		}
	}
	*/
	/*if (destroyWnd)
		glutDestroyWindow(1);*/
	if(process != EXIT)
		glutSwapBuffers();
}

void reshape(int w, int h){
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluOrtho2D(0, WIDTH, HEIGHT,0);
	glMatrixMode(GL_MODELVIEW);
}