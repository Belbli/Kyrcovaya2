#pragma once
#include <GL\glut.h>

class Button
{
private:
	int cx;
	int cy;
	int width;
	int height;
	GLuint text;

public:
	Button();
	void Button::CreateButton();
	void Button::setButtonPosition(int cx, int cy, int width, int height, GLuint text);
	bool Button::isClicked(float xMousePos, float yMousePos);
	~Button();
};

