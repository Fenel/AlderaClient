#include <SFML/Graphics.hpp>
#ifdef _DEBUG
	#include <iostream>
#endif
#include "itemStorage.h"
#include "system.h"
#include "game.h"

//extern sf::RenderWindow *App;
extern System System;
extern Game Game;

ItemStorage::ItemStorage(unsigned int _type) : Container(_type)
{
	maxSize = 30;
	totalWeight = 0;
	
	storageItemsT.loadFromFile("data/itemStorage.bmp");
	storageItems.setTexture(storageItemsT);

	#ifdef _DEBUG
		std::cout << "ItemStorage setted." << std::endl;
	#endif
}

ItemStorage::~ItemStorage()
{
	itemList.clear();
}

void ItemStorage::render()
{
	storageItems.setPosition(posx,posy);
	System.App->draw(storageItems);

	unsigned int counter = 0;
	for(unsigned int y = 0; y <= 35*5; y+=35)
	{
	   for(unsigned int x = 0; x <= 35*4; x+=35)
	   {
		   if(this->getItemHoldingCount() > counter)//((x/36)+(y/36)))
		   {
			  System.renderItem(x + posx+4, y + posy+15, this->getItem(counter));//((x/36)+(y/36)));
			  counter++;
		   }
		}
	} 
}

void ItemStorage::mouse(unsigned char type, int mouse_x, int mouse_y) //lokalne x i y myszki.
{
	unsigned int counter = 0; 

	for(int y = 16; y <= 16+35*5; y+=35)
	for(int x = 4; x <= 4+35*4; x+=35)
	{
		if(mouse_x > x && mouse_x < x+32
		&& mouse_y > y && mouse_y < y+32)
		{ 
			if(type == 0)
			{
				Game.requestItemStats(20, 0, 0, 0, static_cast<unsigned char>(counter));
			}
			if(this->getItemHoldingCount() > counter)
			{
				if(type == 1) //z³apanie itemu w plecaku
				{
					System.hold = true;
					Game.mouseHolding = this->getItem(counter);
					Game.mouseHoldingParentId = 20;
					Game.mouseHoldingParentCounter = counter;
					break;
				}
				if(type == 3) 
				{
					Game.sendUseItem(20, counter, 0, 0, 0, 0, 0);
					//Prawy klawisz myszki, coœ z u¿yciem zrobiæ ;) .
					break;
				}
			}
			if(Game.mouseHolding) //je¿eli gracz przeci¹gn¹³ item nad pole plecaka
			{
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
						Game.sendEquipItem(item, Game.mouseHoldingParentId, 20); //20 = sta³y numer podstawowego plecaka gracza
					System.hold = false;
					Game.mouseHolding = NULL;
					break;
				}
			}
		}
		counter++;
	}
}
