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
	unsigned int btnId;

public:
	Button();
	void Button::CreateButton(int btnId);
	void Button::setButtonPosition(int cx, int cy, int width, int height, GLuint text);
	bool Button::isClicked(float xMousePos, float yMousePos);
	~Button();
};

