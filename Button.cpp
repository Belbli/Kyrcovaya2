#include "Button.h"
#include <iostream>
//#include <GL/glut.h>
using namespace std;

Button::Button(){
}

void Button::setButtonPosition(int cx, int cy, int width, int height, GLuint text) {
	this->cx = cx;
	this->cy = cy;
	this->width = width;
	this->height = height;
	this->text = text;
}

bool Button::isClicked(float xMousePos, float yMousePos){
	if (xMousePos >= cx && xMousePos <= cx + width && yMousePos >= cy && yMousePos <= cy + height) {
		return true;
	}
	return false;
	
}


void Button::CreateButton(int btnId) {
	this->btnId = btnId;
	/*glColor3f(1.0, 1.0, 0.0);
	glRectf(cx, cy, cx + width, cy + height);*/
	glColor3f(1.0, 1.0, 1.0);
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, text);
	glBegin(GL_QUADS);
	glTexCoord2i(1, 0); glVertex2i(cx + width, cy + height);
	glTexCoord2i(0, 0); glVertex2i(cx, cy + height);
	glTexCoord2i(0, 1); glVertex2i(cx, cy);
	glTexCoord2i(1, 1); glVertex2i(cx + width, cy);
	glEnd();
	glDisable(GL_TEXTURE_2D);
	
}

Button::~Button(){

}
