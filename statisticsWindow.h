#ifndef __STATISTICSWINDOW_H__
#define __STATISTICSWINDOW_H__


class StatisticsWindow
{
public:
	StatisticsWindow(int _posx, int _posy);
	void render();
	bool mouse(unsigned char type, int mouse_x, int mouse_y);//lokalne x i y myszki
	int posx, posy;
	void setPos(int x, int y);
	void update();
	bool show;
private:
	sf::Sprite statisticsWindowSprite;
	sf::Texture statisticsWindowTexture;

	sf::Text level;
	sf::Text experience;
	sf::Text vocation;
	sf::Text health;
	sf::Text mana;
	sf::Text strength;
	sf::Text dexterity;
	sf::Text magicPower;
	sf::Text statisticsPoints;
protected:
};

#endif