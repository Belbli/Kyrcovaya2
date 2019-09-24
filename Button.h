#pragma once
class Button
{
private:
	int cx;
	int cy;
	int width;
	int height;
	unsigned int btnId;
	//float xMousePos;
	//float yMousPos;
	//bool clicked = false;

public:
	Button();
	void Button::CreateButton(int btnId);
	void Button::setButtonPosition(int cx, int cy, int width, int height);
	bool Button::isClicked(float xMousePos, float yMousePos);
	//void CreateButton(float cx, float cy, float width, float height);
	~Button();
};

