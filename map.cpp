#include <SFML/Graphics.hpp>

#include <iostream>
#include <time.h>
#include <string>
#include <fstream>
#include <Windows.h>

#include "map.h"
#include "baseItems.h"
#include "creature.h"
#include "game.h"
#include "system.h"
#include "containerWindow.h"

extern BaseItems BaseItems;
extern Game Game;
extern System System;
extern void coutCurrentTime();
extern std::string intToStr(int n);
//extern sf::RenderTexture *renderedMapTexture;

int MAP_MAX_DIMENSIONS = 640;
Map::Map()
{
	posx = 15;
	posy = 15;
	#ifdef _DEBUG
		std::cout << "Map instance hooked." << std::endl;
	#endif
}

unsigned int Map::posToIndex(unsigned int x, unsigned int y, unsigned char z)
{
   unsigned int R = 0, multiplier = 1;

   R += multiplier * x;
   multiplier *= MAP_MAX_DIMENSIONS;

   R += multiplier * y;
   multiplier *= MAP_MAX_DIMENSIONS;

   R += multiplier * int(z);
   multiplier *= 15;  
              
   return R;
}

void Map::indexToPos(unsigned int &x, unsigned int &y, unsigned char &z, unsigned int index)
{
   unsigned int R = 1;

   x = index % MAP_MAX_DIMENSIONS;
   index /= MAP_MAX_DIMENSIONS;
      
   y = index % MAP_MAX_DIMENSIONS;
   index /= MAP_MAX_DIMENSIONS;
      
   z = char(index%15);            
}

void Map::setTile(Position pos, Tile *tile)
{
    setTile(pos.x,pos.y,pos.z,tile);
}

void Map::setTile(unsigned short x, unsigned short y, unsigned char z, Tile *tile)
{
    if(tiles_tab[posToIndex(x, y, z)])
    {
       //delete tiles_tab[posToIndex(x, y, z)];
    }
	tiles_tab[posToIndex(x, y, z)] = tile; 
}

Tile* Map::getTile(Position pos)
{
    return getTile(pos.x,pos.y,pos.z);
}

Tile* Map::getTile(unsigned short x, unsigned short y, unsigned char z)
{
    if(tiles_tab[posToIndex(x, y, z)])
    {
       return tiles_tab[posToIndex(x, y, z)];
    }
    return NULL;
}


void Map::getSpectators(Position& centerPos, std::deque<Creature*> monsters)
{ 	
   unsigned char z1, z2;
   
   if(centerPos.z >= 7)
   {
      z1 = 7;
      z2 = 14;
   }
   else
   {
      z1 = centerPos.z - 1;
      z2 = centerPos.z + 2;
      if(z2>6)
	  {
		  z2=6;
	  }
   }
      
   for(z1;z1<z2;z1++)
   {
      for(unsigned int x = centerPos.x - 9; x < centerPos.x + 9; x++)
	  {
         for(unsigned int y = centerPos.y - 7; y < centerPos.y + 7; y++)
         {
            if(getTile(x,y,z1) != NULL)
	        {
				monsters.assign(getTile(x,y,z1)->getCreatures().begin(),getTile(x,y,z1)->getCreatures().end());
			}
		}
	  }
   }
}

void Map::render()
{
   if(Game.myPlayerZ >= 7)
   {
      for(unsigned char zx = 7; zx <= 14; zx++)
	   { 
           if((!tiles_tab[posToIndex(Game.myPlayerX-1, Game.myPlayerY-1, 8)]  || Game.myPlayerZ == 8)  && (!tiles_tab[posToIndex(Game.myPlayerX-1, Game.myPlayerY-1, 9)]  || Game.myPlayerZ == 9) 
           && (!tiles_tab[posToIndex(Game.myPlayerX-1, Game.myPlayerY-1, 10)] || Game.myPlayerZ == 10) && (!tiles_tab[posToIndex(Game.myPlayerX-1, Game.myPlayerY-1, 11)] || Game.myPlayerZ == 11) 
           && (!tiles_tab[posToIndex(Game.myPlayerX-1, Game.myPlayerY-1, 12)] || Game.myPlayerZ == 12) && (!tiles_tab[posToIndex(Game.myPlayerX-1, Game.myPlayerY-1, 13)] || Game.myPlayerZ == 13) 
           && (!tiles_tab[posToIndex(Game.myPlayerX-1, Game.myPlayerY-1, 14)] || Game.myPlayerZ == 14) || zx <= Game.myPlayerZ)
         {
           for(unsigned char step = 0; step <= 4; step++)    
           {
            for(unsigned int x = Game.myPlayerX - 9; x <= Game.myPlayerX + 10 ;x++)
            {
               for(unsigned int y = Game.myPlayerY - 7; y <= Game.myPlayerY + 8; y++)
               {
                   if(tiles_tab[posToIndex(x, y, zx)] && (step != 4 || Game.myPlayerZ == zx))
                   {
					   tiles_tab[posToIndex(x, y, zx)]->render(0, 0, Game.myPlayerX - 9, Game.myPlayerY - 7, step);
                   }
                }
            } 
           }           
         } 
		 System.renderShootEffects(zx);
      }
   }
   else
   {
	  for(unsigned char zx = Game.myPlayerZ - 2; zx <= Game.myPlayerZ; zx++)
      {    
		 //Game.lightMapTexture->clear(sf::Color(0, 0, 0, 200));
         for(unsigned char step = 0; step <= 5; step++)    
         {
            for(unsigned int x = Game.myPlayerX - 9; x <= Game.myPlayerX + 10 ;x++)
            {
               for(unsigned int y = Game.myPlayerY - 7; y <= Game.myPlayerY + 8; y++)
               {
                   if(tiles_tab[posToIndex(x, y, zx)] && (step < 4 || Game.myPlayerZ == zx))
                   {
                      tiles_tab[posToIndex(x, y, zx)]->render(0, 0, Game.myPlayerX - 9, Game.myPlayerY - 7, step);
                   }
                }
            }  
		 }
		 //Game.renderShadow();
		 System.renderShootEffects(zx);
      }
	  Game.renderShadow();
	  Game.lightMapTexture->clear(sf::Color(0, 0, 0, 200));
   }
}

void Map::mouse(unsigned char type, int mouse_x, int mouse_y)//x i y myszki na mapie
{    
	for(int y = 0; y <= 13*32; y+=32)
	for(int x = 0; x <= 17*32; x+=32)
	{
		if(mouse_x > x && mouse_x < x+32
		&& mouse_y > y && mouse_y < y+32)
		{ 
			if(type == 0)
			{
				Game.requestItemStats(0, Game.myPlayerX-8+(x/32), Game.myPlayerY-6+(y/32), Game.myPlayerZ, 0);
			}
			if(Game.mouseHolding) //je¿eli gracz przeci¹gn¹³ item nad pole
			{
				Item *item = dynamic_cast<Item*>(Game.mouseHolding);
				if(type == 4 && item) //puszczenie lewego klawisza myszki
				{
					Game.sendThrowItem(item, Game.myPlayerX-8+(x/32), Game.myPlayerY-6+(y/32), Game.myPlayerZ);
					System.hold = false;
					Game.mouseHolding = NULL;
					Game.mouseHoldingParentId = -1;
					break;
				}
			}
			else
			{
				if(type == 1) //z³apanie rzeczy lewym klawiszem myszki
				{
					System.hold = true;
					if(this->tiles_tab[posToIndex(Game.myPlayerX-8+(x/32), Game.myPlayerY-6+(y/32), Game.myPlayerZ)])
					{
						Game.mouseHolding = this->tiles_tab[posToIndex(Game.myPlayerX-8+(x/32), Game.myPlayerY-6+(y/32), Game.myPlayerZ)]->getTopItem();
						Game.mouseHoldingParentId = 0;
					}
					break;
				}
			}
			if(type == 6)
			{
				Tile *tilet = this->tiles_tab[posToIndex(Game.myPlayerX-8+(x/32), Game.myPlayerY-6+(y/32), Game.myPlayerZ)];
				if(tilet)
				{
					CreatureList creatures = tilet->getCreatures();
					Item *item = tilet->getTopItem();
					if(!creatures.empty())
					{
						if(creatures.back() != Game.myPlayerTarget)
							Game.sendAttackCreature(creatures.back()->getId(), false);
						else
							Game.myPlayerTarget = NULL;
					}
					else if(item->getContainer())
					{
						Tile *tile = dynamic_cast<Tile*>(item->parent);
						if(tile && (tile->pos.x >= Game.myPlayer->getPosX() -1) && (tile->pos.x <= Game.myPlayer->getPosX() +1) 
						&& (tile->pos.y >= Game.myPlayer->getPosY() -1) && (tile->pos.y <= Game.myPlayer->getPosY() +1) 
						&& (tile->pos.z == Game.myPlayer->getPosZ()))
						{
							Container *container = dynamic_cast<Container*>(item);

							if(Game.containerWindow && Game.containerWindow->container == container)
							{
								Game.closeContainerWindow = true;
							}
							else if(Game.containerWindow)
							{
								delete Game.containerWindow;
								Game.containerWindow = new ContainerWindow(item->mID, container, this->posx + mouse_x - 72, this->posy + mouse_y - 100);
							}
							else
							{
								Game.containerWindow = new ContainerWindow(item->mID, container, this->posx + mouse_x - 72, this->posy + mouse_y - 100);
							}
						}
					}
					else if(BaseItems.getItemType(item->mID)->useable)
					{
						/////////////// TUTAJ U¯YCIE ITEMU USEABLE -> CZYLI GRACZ U¯YWA ITEM NA CZYMŒ
					}
					else
					{
						Game.sendUseItem(0, Game.myPlayerX-8+(x/32), Game.myPlayerY-6+(y/32), Game.myPlayerZ, Game.myPlayerX-8+(x/32), Game.myPlayerY-6+(y/32), Game.myPlayerZ);
					}
				}
			}
		}
	}
}

void Map::removeCreatureFromTiles(unsigned int cid)
{
	for(unsigned int z = 0; z <= 14 ;z++)
	{
		for(unsigned int x = Game.myPlayerX - 9; x <= Game.myPlayerX + 10 ;x++)
		{
		   for(unsigned int y = Game.myPlayerY - 7; y <= Game.myPlayerY + 8; y++)
		   {                  
			   if(tiles_tab[posToIndex(x, y, z)])
			   {
				  tiles_tab[posToIndex(x, y, z)]->removeCreatureByID(cid);
			   }
			}
		}
	}
}
