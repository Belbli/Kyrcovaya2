#include<GL/glut.h>
#include"Button.h"
#include<math.h>
#include<vector>

#define WIDTH 640
#define HEIGHT 480
#define PLAYBTN 1
#define RECORDSBTN 2
#define EXITBTN 0

using namespace std;

Button playBtn, recordsBtn, exitBtn;

int x = -1, y = -1, btnWidth = 150, btnHeight = 40, btnState,btnStart, btnPressed = -1, plateWidth = 80, xLeftPlatePos, xRightPlatePos;
int xMousePos = 0, yPlatePos = 400, xBallPos = 350, yBallPos = yPlatePos;
int stateY = 1, stateX = 1;
int blockAmount = 15;
bool destroyWnd = false, ballMove = true;

struct block {
	int x;
	int y;
	//int draw;
} item;

vector<block> blocks;

void MouseMove(int ax, int ay)
{
	xMousePos = ax;
}

void display();
void timer(int);
void reshape(int, int);
void initCoordinates();

void init()
{
	glClearColor(0.0, 0.0, 1.0, 1.0);
	glOrtho(-360, 360, -360, 360, -100, 100);
}

void MouseMoveClick(int btn, int state, int ax, int ay)
{
	x = ax;
	y = ay;
	btnStart = btn;
	btnState = state;
}

int main(int argc, char *argv[])
{
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
	initCoordinates();
	glutMainLoop();
}

void timer(int)
{
	glutPostRedisplay();
	glutTimerFunc(1000 / 100, timer, 0);
}

void showMenu() {
	int lineSpace = btnHeight + 30;
	playBtn.setButtonPosition(WIDTH / 4 + btnWidth / 2, HEIGHT / 4, btnWidth, btnHeight);
	playBtn.CreateButton(PLAYBTN);

	recordsBtn.setButtonPosition(WIDTH / 4 + btnWidth / 2, HEIGHT / 4 + lineSpace, btnWidth, btnHeight);
	recordsBtn.CreateButton(RECORDSBTN);

	exitBtn.setButtonPosition(WIDTH / 4 + btnWidth / 2, HEIGHT / 4 + 2 * lineSpace, btnWidth, btnHeight);
	exitBtn.CreateButton(EXITBTN);
}

void menu() {
	if (btnPressed == -1) {
		showMenu();
		if (playBtn.isClicked(x, y)) {
			glutSetWindowTitle("Mouse move");
			btnPressed = 1;
			glClear(GL_COLOR_BUFFER_BIT);
		}
		else
			glutSetWindowTitle("ove");

		if (exitBtn.isClicked(x, y))
			destroyWnd = true;
	}
}

void ballMotion()
{
	float angle = 2.0;
	if (!ballMove) {
		xBallPos = xLeftPlatePos + plateWidth / 2;
		yBallPos = yPlatePos - 6;
	}
	else{
		if (yBallPos == yPlatePos && xBallPos > xMousePos - 40 && xBallPos < xMousePos + 50)
		{
			stateY = 1;
			angle = abs(xBallPos - xMousePos) / 5;
		}
		if (yBallPos <= 0)
			stateY = -1;
		if (xBallPos <= 0)
			stateX = 1;
		if (xBallPos >= WIDTH)
			stateX = -1;
		switch (stateX)
		{
		case -1:
			xBallPos -= angle;
			break;
		case 1:
			xBallPos += angle;
			break;
		}
		switch (stateY)
		{
		case -1:
			yBallPos += 2;
			break;
		case 1:
			yBallPos -= 2;
			break;
		}
	}
	
}

void collision()
{
	int deleteBlock = 0, index = -1;
	for (int i = 0; i < blockAmount; i++){
		if(yBallPos >= blocks[i].y && yBallPos <= blocks[i].y + 20)
			if (xBallPos + 10 == blocks[i].x || xBallPos - 20 == blocks[i].x ) {
				stateX *= -1;
				deleteBlock = 1;
				index = i;
				break;
			}
		if(xBallPos >= blocks[i].x && xBallPos <= blocks[i].x + 20)
			if (yBallPos + 10 == blocks[i].y || yBallPos - 20 == blocks[i].y) {
				stateY *= -1;
				deleteBlock = 1;
				index = i;
				break;
			}
		}
	if (deleteBlock == 1) {
		blocks.erase(blocks.begin() + index);
		blockAmount--;
	}

}

void initCoordinates()
{
	for (int i = 0; i < blockAmount; i++)
	{
		item.x = i * 25 + 10;
		item.y = 200;
		blocks.push_back(item);
	}
}

void ViewBlocks()
{
	glColor3f(1.0, 0.0, 1.0);
	glPointSize(20);
	for (int i = 0; i < blockAmount; i++)
	{
		glBegin(GL_POINTS);
		glVertex2f(blocks[i].x, blocks[i].y);
		glEnd();
	}

}

void platePosition() {
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

void drawBall() {
	ballMotion();
	glBegin(GL_POINTS);
	glColor3f(1.0, 1.0, 0.0);
	
	glVertex2f(xBallPos, yBallPos);
	glEnd();
}

void display()
{
	glClear(GL_COLOR_BUFFER_BIT);
	glLoadIdentity();
	menu();
	if (btnPressed == 1) {
		collision();
		ViewBlocks();

		glPointSize(10);
		drawBall();
		drawPlate();
	}
	glutSwapBuffers();
	if (destroyWnd)
		glutDestroyWindow(1);
}

void reshape(int w, int h)
{
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluOrtho2D(0, WIDTH, HEIGHT,0);
	glMatrixMode(GL_MODELVIEW);
}