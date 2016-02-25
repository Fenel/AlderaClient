#ifndef __NPCWINDOW_H__
#define __NPCWINDOW_H__

#include <string>
#include <deque>
#include <SFML\Graphics.hpp>

using namespace std;

struct shopOffer
{
	string name;
	unsigned int id;
	unsigned int price;
	sf::Text sfName;
	sf::Text sfPrice;
};

class NPCWindow
{
public:
	NPCWindow(int _posx, int _posy);
	void render();
	bool mouse(unsigned char type, int mouse_x, int mouse_y);//lokalne x i y myszki
	int posx, posy;
	void setPos(int x, int y);
	void update();
	bool show;
	bool shop;
	void newConversation(string npcName, unsigned char npcLooktype);
	void addSaid(string text);
	void addChoice(string text);
	void addSells(unsigned int itemId, unsigned int price, string name);
	void addBuys(unsigned int itemId, unsigned int price, string name);
	void clearSells() { sells.clear(); };
	void clearBuys()  { buys.clear();  };
private:
	sf::Sprite npcWindowSprite;
	sf::Texture npcWindowTexture;
	sf::Sprite npcShopWindowSprite;
	sf::Texture npcShopWindowTexture;
	sf::Sprite npcSprite;
	sf::Sprite playerSprite;

	sf::Text npcName;
	sf::Text playerName;

	deque<sf::Text*> said;
	deque<sf::Text*> choices;
	deque<shopOffer> sells;
	deque<shopOffer> buys;
	bool transparent;

	int currentSell;
	int currentBuy;
protected:

};

#endif