#include "Button.h"
#include <iostream>
#include <GL/glut.h>
using namespace std;

Button::Button(){
}

void Button::setButtonPosition(int cx, int cy, int width, int height) {
	this->cx = cx;
	this->cy = cy;
	this->width = width;
	this->height = height;
}

bool Button::isClicked(float xMousePos, float yMousePos){
	if (xMousePos >= this->cx && xMousePos <= this->cx + this->width && yMousePos >= this->cy && yMousePos <= this->cy + this->height) {
		return true;
	}
	return false;
	
}

void Button::CreateButton(int btnId) {
	this->btnId = btnId;

	glColor3f(1.0, 0.0, 0.0);

	glBegin(GL_POLYGON);
	glVertex2f(this->cx, this->cy);
	glVertex2f(this->cx + this->width, this->cy);
	glVertex2f(this->cx + this->width, this->cy + this->height);
	glVertex2f(this->cx, this->cy + this->height);
	
	glEnd();
}

Button::~Button(){

}
