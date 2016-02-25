#ifndef __EQUWINDOW_H__
#define __EQUWINDOW_H__

class Item;

class EquWindow
{
public:
	EquWindow(int _posx, int _posy);
	void render();
	void update();
	bool mouse(unsigned char type, int mouse_x, int mouse_y);//lokalne x i y myszki
	int posx, posy;
	void setPos(int x, int y);
	bool show;

	Item *lRingEar;
	Item *rRingEar;
	Item *head;
	Item *necklace;
	Item *backpack;
	Item *armor;
	Item *shield;
	Item *weapon;
	Item *belt;
	Item *legs;
	Item *boots;
	Item *lRing;
	Item *rRing;
	Item *gloves;
	Item *arrows;
private:
	sf::Texture equWindowTexture;
	sf::Sprite equWindowSprite;
	sf::RectangleShape healthBar;
	sf::RectangleShape manaBar;
};

#endif