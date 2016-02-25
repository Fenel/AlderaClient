#include <SFML/Graphics.hpp>

#include "hotkeys.h"
#include "system.h"
#include "game.h"

extern System System;
extern Game Game;

Hotkeys::Hotkeys(int _posx, int _posy)
{
	this->posx = _posx;
	this->posy = _posy;

	this->guiHotkeysTexture.loadFromFile("data/guiHotkeys.bmp");
	this->guiHotkeysSprite.setTexture(this->guiHotkeysTexture);

	this->setPos(posx, posy);
	this->loadSpellsIcons();
}

void Hotkeys::loadSpellsIcons()
{
	char tempChar[30]; 
	bool proceed = true;
	unsigned int image = 1;

	while(proceed)
	{
		sprintf(tempChar,"data/spellsIcons/%d.bmp",image);

		this->images.push_back(sf::Image());
		if(this->images.at(this->images.size()-1).loadFromFile(tempChar))
		{
			this->images.at(this->images.size()-1).createMaskFromColor(sf::Color(255,0,255));

			this->textures.push_back(sf::Texture());
			this->textures.at(this->textures.size()-1).loadFromImage(this->images.at(this->images.size()-1));

			this->sprites.push_back(sf::Sprite());
			this->sprites.at(sprites.size()-1).setTexture(this->textures.at(this->textures.size()-1));
			this->sprites.at(sprites.size()-1).setPosition(posx + 4 + (image-1)*35, posy + 4);
		}
		else
		{
			proceed = false;
		}
		++image;
	}
	for(unsigned int it = 1; it < image; it++)
	{
		cooldowns.push_back(CooldownStruct());
		cooldowns[it-1].lastUsed = 0;
		cooldowns[it-1].rectangle.setSize(sf::Vector2f(float(32), float(32)));
		cooldowns[it-1].rectangle.setFillColor(sf::Color(255, 0, 0, 192));
		cooldowns[it-1].rectangle.setPosition(posx + 4 + (it-1)*35, posy + 4);

		if(it == 1)
		{
			cooldowns[it-1].cooldown = 1000;
			cooldowns[it-1].rectangle.setFillColor(sf::Color(0, 0, 255, 192));
		}
		else if(it == 2 || it == 3 || it == 6 || it == 9)
			cooldowns[it-1].cooldown = 2000;
		else if(it == 3 || it == 4 || it == 5 || it == 7 || it == 8 || it == 10)
			cooldowns[it-1].cooldown = 3000;
		else
			cooldowns[it-1].cooldown = 2000;
	}
}

void Hotkeys::render()
{
	System.App->draw(this->guiHotkeysSprite);

	for(unsigned int it = 0; it < sprites.size(); it++)
	{
		System.App->draw(this->sprites[it]);
		if( (cooldowns[it].lastUsed + cooldowns[it].cooldown > clock()) )
		{
			float percent = ((clock() - cooldowns[it].lastUsed)/(cooldowns[it].cooldown/100.0));
			cooldowns[it].rectangle.setSize(sf::Vector2f(float(32), float(32.0 - (32.0 * percent / 100.0))));
			System.App->draw(this->cooldowns[it].rectangle);
		}
		else if( it+1 == 1 && this->defensiveSpellsCooldown + 1000 > clock() )
		{
			float percent = ((clock() - defensiveSpellsCooldown)/10);
			cooldowns[it].rectangle.setSize(sf::Vector2f(float(32), float(32.0 - (32.0 * percent / 100.0))));
			System.App->draw(this->cooldowns[it].rectangle);
		}
		else if( it+1 > 1 && this->offensiveSpellsCooldown + 2000 > clock() )
		{
			float percent = ((clock() - offensiveSpellsCooldown)/20);
			cooldowns[it].rectangle.setSize(sf::Vector2f(float(32), float(32.0 - (32.0 * percent / 100.0))));
			System.App->draw(this->cooldowns[it].rectangle);
		}
	}
}

bool Hotkeys::mouse(unsigned char type, int mouse_x, int mouse_y) //lokalne x i y myszki.
{
	return false;
}

void Hotkeys::setPos(int x, int y) 
{ 
	this->posx = x;
	this->posy = y;
	this->guiHotkeysSprite.setPosition(posx, posy);
}

void Hotkeys::setLastUse(unsigned char spellID)
{ 
	if((spellID - 1) < cooldowns.size() && (this->cooldowns[spellID - 1].lastUsed + this->cooldowns[spellID - 1].cooldown < clock()) )
	{
		if( (spellID == 1 && this->defensiveSpellsCooldown + 1000 < clock()) || (spellID > 1 && this->offensiveSpellsCooldown + 2000 < clock()) )
		{
			if( (spellID - 1) < cooldowns.size() )
				this->cooldowns[spellID - 1].lastUsed = clock();

			if(spellID == 1)
				this->defensiveSpellsCooldown = clock();
			else if(spellID > 1)
				this->offensiveSpellsCooldown = clock();
		}
	}
}

void Hotkeys::cancelCooldown(unsigned char spellID)
{
	if( (spellID - 1) < cooldowns.size() )
		this->cooldowns[spellID - 1].lastUsed = 0;

	if(spellID == 1)
		this->defensiveSpellsCooldown = 0;
	else if(spellID > 1)
		this->offensiveSpellsCooldown = 0;
}