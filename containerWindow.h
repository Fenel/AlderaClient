#ifndef __CONTAINERWINDOW_H__
#define __CONTAINERWINDOW_H__

#include "container.h"

class ContainerWindow
{
public:
	ContainerWindow(unsigned int _type, Container *_container, int _posx, int _posy);
	void render();
	bool mouse(unsigned char type, int mouse_x, int mouse_y);//lokalne x i y myszki
	int posx, posy;
	void setPos(int x, int y);
	Container *container;
	bool isReadyToClose();
private:
	sf::Sprite containerWindowSprite;
	sf::Text title;
protected:
};

#endif