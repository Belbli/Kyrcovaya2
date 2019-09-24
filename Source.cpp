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

int x = -1, y = -1, btnWidth = 150, btnHeight = 40, btnState, btnPressed = -1, plateWidth = 80;
int xPlatePos = 0, yPlatePos = 400, xBallPos = 350, yBallPos = 350;
int stateY = 1, stateX = 1;
int blockAmount = 15;
bool destroyWnd = false;

struct block {
	int x;
	int y;
	//int draw;
} item;

vector<block> blocks;

void MouseMove(int ax, int ay)
{
	x = ax;
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
	glutTimerFunc(1000 / 60, timer, 0);
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

float angle = 2.0;

void ballMotion()
{
	if (yBallPos == yPlatePos && xBallPos > x - 40 && xBallPos < x + 50)
	{
		stateY = 1;
		angle = tan(1.57 - 5.0 / abs(xBallPos - x));
		if (angle < 0)
			angle *= -1;
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
	if (x > plateWidth/2 && x < WIDTH - plateWidth/2)
		glTranslatef(x, 0.0, 0.0);
	else if (x <= plateWidth / 2)
		glTranslatef(plateWidth / 2, 0.0, 0.0);
	else
		glTranslatef(WIDTH - plateWidth / 2, 0.0, 0.0);

}
//px py
void collision() //столкновение
{
	for (int i = 0; i < blockAmount; i++)
		if (abs(xBallPos - blocks[i].x) <= 20 && abs(yBallPos - blocks[i].y) <= 20)
		{
			if (xBallPos + 10 == blocks[i].x || xBallPos - 20 == blocks[i].x && abs(yBallPos - blocks[i].y) <= 20)
				stateX *= -1;
			else if (yBallPos - 10 == blocks[i].y || yBallPos + 20 == blocks[i].y && abs(xBallPos - blocks[i].x) <= 20)
				stateY *= -1;
			blocks.erase(blocks.begin() + i);
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



void display()
{
	glClear(GL_COLOR_BUFFER_BIT);
	glLoadIdentity();
	menu();
	if (btnPressed == 1) {
		collision();
		ViewBlocks();
		glColor3f(1.0, 1.0, 0.0);
		glPointSize(10);

		glBegin(GL_POINTS);
		glVertex2f(xBallPos, yBallPos);
		glEnd();

		ballMotion();

		glBegin(GL_POLYGON);

		glColor3f(1.0, 0.0, 0.0);
		glVertex2f(-40.0, 20.0 + yPlatePos);
		glVertex2f(-40.0, 0.0 + yPlatePos);
		glVertex2f(40.0, 0.0 + yPlatePos);
		glVertex2f(40.0, 20.0 + yPlatePos);

		glEnd();
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