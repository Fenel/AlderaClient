#ifndef __HOTKEYS_H__
#define __HOTKEYS_H__

#include <SFML/Graphics.hpp>
#include <deque>

struct CooldownStruct
{
	sf::RectangleShape rectangle;
	time_t lastUsed;
	unsigned int cooldown;
};

class Hotkeys
{
public:
	Hotkeys(int _posx, int _posy);
	void loadSpellsIcons();
	void render();
	bool mouse(unsigned char type, int mouse_x, int mouse_y);//lokalne x i y myszki
	int posx, posy;
	void setPos(int x, int y);
	void setLastUse(unsigned char spellID);
	void cancelCooldown(unsigned char spellID);
private:
	sf::Texture guiHotkeysTexture;
	sf::Sprite guiHotkeysSprite;
	std::deque<sf::Image> images;
	std::deque<sf::Texture> textures;
    std::deque<sf::Sprite> sprites;
	std::deque<CooldownStruct> cooldowns;
	time_t offensiveSpellsCooldown;
	time_t defensiveSpellsCooldown;
};

#endif