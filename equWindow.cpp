#include <SFML/Graphics.hpp>

#include "equWindow.h"
#include "system.h"
#include "game.h"
#include "baseItems.h"
#include "container.h"
#include "statisticsWindow.h"

//extern sf::RenderWindow *App;
extern System System;
extern Game Game;
extern BaseItems BaseItems;
extern StatisticsWindow StatisticsWindow;

EquWindow::EquWindow(int _posx, int _posy)
{
	this->posx = _posx;
	this->posy = _posy;

	this->equWindowTexture.loadFromFile("data/equWindow.bmp");
	this->equWindowSprite.setTexture(this->equWindowTexture);

	this->setPos(posx, posy);

	show = true;

	healthBar.setFillColor(sf::Color(234, 35, 0));
	healthBar.setSize(sf::Vector2f(14.0f, 124.0f));
	healthBar.setPosition(float(posx + 5), float(posy + 90));
	healthBar.setOutlineThickness(1.0f);
	healthBar.setOutlineColor(sf::Color::Black);
	manaBar.setFillColor(sf::Color(0, 113, 225));
	manaBar.setSize(sf::Vector2f(14.0f, 124.0f));
	manaBar.setPosition(float(posx + 26), float(posy + 90));
	manaBar.setOutlineThickness(1.0f);
	manaBar.setOutlineColor(sf::Color::Black);
}

void EquWindow::render()
{
	System.App->draw(this->equWindowSprite);

	unsigned int counter = 0;
	for(unsigned int y = 0; y <= 39*4; y+=39)
	{
	    for(unsigned int x = 0; x <= 39*2; x+=39)
	    {
			if(counter == 0 && this->lRingEar)
				System.renderItem(x + posx + 13 + 44, y + posy + 25, lRingEar); //+44 w zwi¹zku z przesuniêciem zawartoœci okna do prawej strony
			if(counter == 1 && this->head)
				System.renderItem(x + posx + 13 + 44, y + posy + 25, head);
			if(counter == 2 && this->rRingEar)
				System.renderItem(x + posx + 13 + 44, y + posy + 25, rRingEar);
			if(counter == 3 && this->necklace)
				System.renderItem(x + posx + 13 + 44, y + posy + 25, necklace);
			if(counter == 4 && this->armor)
				System.renderItem(x + posx + 13 + 44, y + posy + 25, armor);
			if(counter == 5 && this->backpack)
				System.renderItem(x + posx + 13 + 44, y + posy + 25, backpack);
			if(counter == 6 && this->weapon)
				System.renderItem(x + posx + 13 + 44, y + posy + 25, weapon);
			if(counter == 7 && this->belt)
				System.renderItem(x + posx + 13 + 44, y + posy + 25, belt);
			if(counter == 8 && this->shield)
				System.renderItem(x + posx + 13 + 44, y + posy + 25, shield);
			if(counter == 9 && this->lRing)
				System.renderItem(x + posx + 13 + 44, y + posy + 25, lRing);
			if(counter == 10 && this->legs)
				System.renderItem(x + posx + 13 + 44, y + posy + 25, legs);
			if(counter == 11 && this->rRing)
				System.renderItem(x + posx + 13 + 44, y + posy + 25, rRing);
			if(counter == 12 && this->gloves)
				System.renderItem(x + posx + 13 + 44, y + posy + 25, gloves);
			if(counter == 13 && this->boots)
				System.renderItem(x + posx + 13 + 44, y + posy + 25, boots);
			if(counter == 14 && this->arrows)
				System.renderItem(x + posx + 13 + 44, y + posy + 25, arrows);

			counter++;
		 }
	} 
	System.App->draw(healthBar);
	System.App->draw(manaBar);
}

void EquWindow::update()
{
	int hpSize = int(124.0f*float(Game.myPlayerHealthPercent)/100.0);
	healthBar.setSize(sf::Vector2f(14.0f, hpSize));
	healthBar.setPosition(float(posx + 5), float(posy + 90 + 124 - hpSize));

	int mpSize = int(124.0f*float(Game.myPlayerMana)/float(Game.myPlayerMaxMana));
	manaBar.setSize(sf::Vector2f(14.0f, mpSize));
	manaBar.setPosition(float(posx + 26), float(posy + 90 + 124 - mpSize));
}

bool EquWindow::mouse(unsigned char type, int mouse_x, int mouse_y) //lokalne x i y myszki.
{
	unsigned int counter = 0; 

	if(type == 4 && mouse_x >= 1 && mouse_x <= 43 && mouse_y >= 14 && mouse_y <= 28) //puszczenie lewego buttonu, odpalenie okna statystyk
	{
		StatisticsWindow.show = !StatisticsWindow.show;
		return true;
	}

	for(int y = 26; y <= 26+39*4; y+=39)
	for(int x = 57; x <= 57+39*2; x+=39)
	{
		if(mouse_x > x && mouse_x < x+32
		&& mouse_y > y && mouse_y < y+32)
		{ 
			if(type == 0)
			{
				Game.requestItemStats(counter+1, 0, 0, 0, 0);
			}
			if(type == 1) //z³apanie itemu w plecaku
			{
				System.hold = true;

				if(counter == 0)
					Game.mouseHolding = lRingEar;
				if(counter == 1)
					Game.mouseHolding = head;
				if(counter == 2)
					Game.mouseHolding = rRingEar;
				if(counter == 3)
					Game.mouseHolding = necklace;
				if(counter == 4)
					Game.mouseHolding = armor;
				if(counter == 5)
					Game.mouseHolding = backpack;
				if(counter == 6)
					Game.mouseHolding = weapon;
				if(counter == 7)
					Game.mouseHolding = belt;
				if(counter == 8)
					Game.mouseHolding = shield;
				if(counter == 9)
					Game.mouseHolding = lRing;
				if(counter == 10)
					Game.mouseHolding = legs;
				if(counter == 11)
					Game.mouseHolding = rRing;
				if(counter == 12)
					Game.mouseHolding = gloves;
				if(counter == 13)
					Game.mouseHolding = boots;
				if(counter == 14)
					Game.mouseHolding = arrows;

				Game.mouseHoldingParentId = counter+1;
				Game.mouseHoldingParentCounter = 255;
				return true;
			}
			if(type == 3) 
			{
				//Prawy klawisz myszki, coœ z u¿yciem zrobiæ ;) .
				return false;
			}
			if(Game.mouseHolding) //je¿eli gracz przeci¹gn¹³ item nad pole equ
			{
				Item *item = dynamic_cast<Item*>(Game.mouseHolding);
				if(type == 4) 
				{
					Tile *tile = dynamic_cast<Tile*>(item->parent);
					Container *container = dynamic_cast<Container*>(item->parent);
					if(tile)
						Game.sendEquipItem(item, tile->getIndexOfThing(item), counter+1);
					else if(container)
						Game.sendEquipItem(item, container->getItemIndex(item), counter+1);
					else
						Game.sendEquipItem(item, Game.mouseHoldingParentCounter, counter+1);

					System.hold = false;
					Game.mouseHolding = NULL;
					break;
				}
				return true;
			}
		}
		counter++;
	}
	return false;
}

void EquWindow::setPos(int x, int y) 
{ 
	this->posx = x;
	this->posy = y;
	this->equWindowSprite.setPosition(posx, posy);
};