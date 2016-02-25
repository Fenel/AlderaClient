#include <SFML/Graphics.hpp>
#include "ContainerWindow.h"
#include "system.h"
#include "game.h"
#include "baseItems.h"

//extern sf::RenderWindow *App;
extern System System;
extern Game Game;
extern BaseItems BaseItems;

ContainerWindow::ContainerWindow(unsigned int _type, Container *_container, int _posx, int _posy)
{
	this->posx = _posx;
	this->posy = _posy;
	container = _container;
	this->containerWindowSprite.setTexture(Game.containerWindowTexture);

	this->title.setFont(System.font);
    this->title.setString(BaseItems.getItemType(_type)->name);
	this->title.setCharacterSize(11);

	this->setPos(posx, posy);
}

void ContainerWindow::render()
{
	System.App->draw(this->containerWindowSprite);
	System.App->draw(this->title);

	unsigned int counter = 0;
	for(unsigned int y = 0; y <= 35*1; y+=35)
	{
	   for(unsigned int x = 0; x <= 35*3; x+=35)
	   {
		   if(container->getItemHoldingCount() > counter)//((x/36)+(y/36)))
		   {
			  System.renderItem(x + posx + 4, y + posy + 15, container->getItem(counter));//((x/36)+(y/36)));
			  counter++;
		   }
		}
	} 
}

bool ContainerWindow::mouse(unsigned char type, int mouse_x, int mouse_y) //lokalne x i y myszki.
{
	unsigned int counter = 0; 

	if(mouse_x >= 132 && mouse_x <= 143 && mouse_y >= 1 && mouse_y <= 12)
	{
		Game.closeContainerWindow = true;
		return true;
	}

	for(int y = 16; y <= 16+35*1; y+=35)
	for(int x = 4; x <= 4+35*3; x+=35)
	{
		if(mouse_x > x && mouse_x < x+32
		&& mouse_y > y && mouse_y < y+32)
		{ 
			if(container->getItemHoldingCount() > counter)
			{
				if(type == 0)
				{
					Tile *tile2 = dynamic_cast<Tile*>(container->parent);
					if(tile2)
						Game.requestItemStats(0, tile2->pos.x, tile2->pos.y, tile2->pos.z, counter);
				}
				if(type == 1) //z³apanie itemu w plecaku
				{
					System.hold = true;
					//Game.mouseHolding = container->getItem(counter);
					Game.mouseHoldingParentId = 0;
					//Game.mouseHoldingParentCounter = counter;
					Game.sendEquipItem(container->getItem(counter), counter, 20);
					return true;
				}
				if(type == 3) 
				{
					//Prawy klawisz myszki, coœ z u¿yciem zrobiæ ;) .
					return true;
				}
			}
			if(Game.mouseHolding) //je¿eli gracz przeci¹gn¹³ item nad pole plecaka
			{
				/*
				Item *item = dynamic_cast<Item*>(Game.mouseHolding);
				if(type == 4) 
				{
					Tile *tile = dynamic_cast<Tile*>(item->parent);
					Container *container = dynamic_cast<Container*>(item->parent);
					if(tile)
						Game.sendEquipItem(item, tile->getIndexOfThing(item), 20);
					else if(container)
						Game.sendEquipItem(item, container->getItemIndex(item), 20);
					else
						Game.sendEquipItem(item, Game.mouseHoldingParentCounter, 20); //20 = sta³y numer podstawowego plecaka gracza
					System.hold = false;
					Game.mouseHolding = NULL;
					break;
				}
				*/
				return true;
			}
		}
		counter++;
	}
	return false;
}

void ContainerWindow::setPos(int x, int y) 
{ 
	this->posx = x;
	this->posy = y;
	this->containerWindowSprite.setPosition(posx, posy);
	this->title.setPosition(float(posx + 3 +(128/2)-(BaseItems.getItemType(container->mID)->name.size()*7/2)), float(posy));
}

bool ContainerWindow::isReadyToClose()
{
	if(this->container->getItemHoldingCount() == 0)
		return true;
	else
		return false;
}