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

int x = -1, y = -1, btnWidth = 150, btnHeight = 40, btnState, btnPressed = -1;
bool destroyWnd = false;

void display();
void reshape(int, int);

void init()
{
	glClearColor(1.0, 1.0, 1.0, 1.0);
	glOrtho(-360, 360, -360, 360, -100, 100);
}

void MouseMove(int btn, int state, int ax, int ay)
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
	glutMouseFunc(MouseMove);
	init();
	glutMainLoop();
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

void display()
{
	glClear(GL_COLOR_BUFFER_BIT);
	glLoadIdentity();
	menu();
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