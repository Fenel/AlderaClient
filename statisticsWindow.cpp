#include <SFML/Graphics.hpp>

#include "statisticsWindow.h"
#include "system.h"
#include "game.h"

extern System System;
extern Game Game;
extern std::string intToStr(int n);

StatisticsWindow::StatisticsWindow(int _posx, int _posy)
{
	this->posx = _posx;
	this->posy = _posy;

	this->statisticsWindowTexture.loadFromFile("data/statisticsWindow.bmp");
	this->statisticsWindowSprite.setTexture(this->statisticsWindowTexture);

	this->setPos(posx, posy);

	show = false;

	level.setFont(System.font);
	level.setCharacterSize(11);
	//level.setColor(sf::Color::Black);
	experience.setFont(System.font);
	experience.setCharacterSize(11);
	//experience.setColor(sf::Color::Black);
	vocation.setFont(System.font);
	vocation.setCharacterSize(11);
	//vocation.setColor(sf::Color::Black);
	health.setFont(System.font);
	health.setCharacterSize(11);
	//health.setColor(sf::Color::Black);
	mana.setFont(System.font);
	mana.setCharacterSize(11);
	//mana.setColor(sf::Color::Black);
	strength.setFont(System.font);
	strength.setCharacterSize(11);
	//strength.setColor(sf::Color::Black);
	dexterity.setFont(System.font);
	dexterity.setCharacterSize(11);
	//dexterity.setColor(sf::Color::Black);
	magicPower.setFont(System.font);
	magicPower.setCharacterSize(11);
	//magicPower.setColor(sf::Color::Black);
	statisticsPoints.setFont(System.font);
	statisticsPoints.setCharacterSize(11);
	//statisticsPoints.setColor(sf::Color::Black);
}

void StatisticsWindow::render()
{
	if(show)
	{
		System.App->draw(this->statisticsWindowSprite);
		System.App->draw(this->level);
		System.App->draw(this->experience);
		System.App->draw(this->vocation);
		System.App->draw(this->health);
		System.App->draw(this->mana);
		System.App->draw(this->strength);
		System.App->draw(this->dexterity);
		System.App->draw(this->magicPower);
		System.App->draw(this->statisticsPoints);
	}
}

bool StatisticsWindow::mouse(unsigned char type, int mouse_x, int mouse_y) //lokalne x i y myszki.
{
	if(type == 4) //puszczenie przycisku myszki na obszarze okna
	{
		if(mouse_x >= 206 && mouse_x <= 226 && mouse_y >= 1 && mouse_y <= 12)
		{ 
			this->show = false;
			return true;
		}
		if(mouse_x >= 199 && mouse_x <= 212)
		{ 
			if(mouse_y >= 155 && mouse_y <= 169) //si³a++
			{
				Game.sendAddStatPoint(1);
				return true;
			}
			if(mouse_y >= 179 && mouse_y <= 193) //zrêcznoœæ++
			{
				Game.sendAddStatPoint(2);
				return true;
			}
			if(mouse_y >= 203 && mouse_y <= 217) //si³a magii++
			{
				Game.sendAddStatPoint(3);
				return true;
			}
		}
	}
	return false;
}

void StatisticsWindow::setPos(int x, int y) 
{ 
	this->posx = x;
	this->posy = y;
	this->statisticsWindowSprite.setPosition(posx, posy);
};

void StatisticsWindow::update()
{
	level.setPosition(posx+98, posy+26);
	level.setString(intToStr(Game.myPlayerLevel));
	experience.setPosition(posx+98, posy+48);
	experience.setString(intToStr(Game.myPlayerExperience));
	vocation.setPosition(posx+98, posy+70);

	if(Game.myPlayerVocation == Vocation::VOCATION_MAGE)
		vocation.setString("Mage");
	else if(Game.myPlayerVocation == Vocation::VOCATION_ROGUE)
		vocation.setString("Rogue");
	else if(Game.myPlayerVocation == Vocation::VOCATION_WARRIOR)
		vocation.setString("Warrior");
	else if(Game.myPlayerVocation == Vocation::VOCATION_NONE)
		vocation.setString("No Vocation Yet");

	health.setPosition(posx+98, posy+96);
	string temp = intToStr(Game.myPlayerHealth);
	temp += "/";
	temp += intToStr(Game.myPlayerMaxHealth);
	health.setString(temp);
	temp.clear();
	mana.setPosition(posx+98, posy+120);
	temp = intToStr(Game.myPlayerMana);
	temp += "/";
	temp += intToStr(Game.myPlayerMaxMana);
	mana.setString(temp);

	strength.setPosition(posx+134, posy+155);
	strength.setString(intToStr(Game.myPlayerStrength));
	dexterity.setPosition(posx+134, posy+179);
	dexterity.setString(intToStr(Game.myPlayerDexterity));
	magicPower.setPosition(posx+134, posy+203);
	magicPower.setString(intToStr(Game.myPlayerMagicPower));
	statisticsPoints.setPosition(posx+155, posy+228);
	statisticsPoints.setString(intToStr(Game.myPlayerStatisticsPoints));

}