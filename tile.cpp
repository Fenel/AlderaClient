#include <iostream>
#include <deque>
#include <algorithm>
#include <string>

#include "tile.h"
#include "thing.h"
#include "system.h"
#include "itemType.h"

extern System System;

using namespace std;

CreatureList& Tile::getCreatures()
{  
    return creatures;
}


bool Tile::isCollision()
{
    if(ground && ground->isCollision())
    {
        return true;
	}

    if(!ground)
    {
        return true;
	}
    
    ItemVector::const_iterator iit;
	for(iit = topItems.begin(); iit != topItems.end(); ++iit)
    {
		if((*iit)->isCollision())
		   return true;
	}
	
    //if(!creatures.empty())
    //   return true;

	if(!creatures.empty())
		for(CreatureList::iterator it = creatures.begin(); it != creatures.end(); it++)
			if(*it && (*it)->active)
				return true;

	for(iit = downItems.begin(); iit != downItems.end(); ++iit)
    {
		if((*iit)->isCollision())
		   return true;
	}

	return false;
}

void Tile::addThing(Thing *thing)
{    

	thing->parent = this;

    Creature *creature = dynamic_cast<Creature *>(thing);
	if(creature)
	{
        creatures.push_back(creature);
		creature->setPos(this->pos.x,this->pos.y,this->pos.z);
	}
    else
	{
        Item *item = dynamic_cast<Item *>(thing);
		if(!item)
			return;
		
        if(item->isGroundTile())
		{
            if(ground)
                delete ground;
            ground = item;
		}
		else if(item->isAlwaysOnTop() || item->baseItem->alwaysOnTopOrder > 0)
		{
			topItems.push_back(item);
        } 
		else if(item->isSplash())
		{
			ItemVector::iterator it;
			for(it = downItems.begin(); it != downItems.end(); it++)
			{
				if((*it)->isSplash())
				{
					downItems.erase(it);
					delete (*it);
					break;
				}
			}
			downItems.insert(downItems.begin(),item);
		}
		else
			downItems.push_back(item);
    }
}

void Tile::createEffect(unsigned char type)
{
	effects.push_back(new Effect(type));
}

void Tile::createTextEffect(unsigned char color, string text)
{
	textEffects.push_back(new TextEffect(color, text));
}

unsigned char Tile::getIndexOfThing(Thing* thing)
{
	if(ground){
		if(ground == thing){
			return 0;
		}
	}

    unsigned char n = 0;
	ItemVector::const_iterator iit;
	for(iit = topItems.begin(); iit != topItems.end(); ++iit)
    {
		++n;
		if((*iit) == thing)
			return n;
	}

	CreatureList::const_iterator cit;
	for(cit = creatures.begin(); cit != creatures.end(); ++cit)
    {
		++n;
		if((*cit) == thing)
			return n;
	}

	for(iit = downItems.begin(); iit != downItems.end(); ++iit)
    {
		++n;
		if((*iit) == thing)
			return n;
	}

	return 255;
}

Thing *Tile::getThing(unsigned char stackpos)
{
    if(ground)
    {
		if(stackpos == 0)
		{
			return ground;
		}

		--stackpos;
	}

	if(stackpos < topItems.size())
		return topItems[stackpos];

	stackpos -= (unsigned char)topItems.size();

	//if(stackpos < creatures.size())////////////////////////////////////!!!!!!!!!//
	//	return creatures[stackpos];/////////////////////////////////////!!!!!!!!!//

	stackpos -= (unsigned char)creatures.size();

	if(stackpos < downItems.size())
		return downItems[stackpos];
    return NULL;
}

Item* Tile::getTopItem()
{
	if(topItems.size() > 0)
	{
		return topItems[topItems.size()-1];
	}
	if(downItems.size() > 0)
	{
		return downItems[downItems.size()-1];
	}
	if(ground)
    {
		return ground;
	}
    return NULL;
}

void Tile::transformThing(unsigned char stackpos, Thing *thing)
{
	Item* item = dynamic_cast<Item *>(thing);
	if(item == NULL)
		return;

	Item* oldItem = NULL;

	if(ground){
		if(stackpos == 0){
			oldItem = ground;
			ground = item;
		}

		--stackpos;
	}

	if(stackpos < (unsigned char)topItems.size()){
		ItemVector::iterator it = topItems.begin();
		it += stackpos;
		stackpos = 0;

		oldItem = (*it);
		it = topItems.erase(it);
		topItems.insert(it, item);
	}

	stackpos -= (unsigned char)topItems.size();

	if(stackpos < (unsigned char)creatures.size()){
		return;
	}

	stackpos -= (unsigned char)creatures.size();

	if(stackpos < (unsigned char)downItems.size()){
		ItemVector::iterator it = downItems.begin();
		it += stackpos;
		stackpos = 0;

		oldItem = (*it);
		it = downItems.erase(it);
		downItems.insert(it, item);
	}

	if(stackpos == 0){
		delete oldItem;
	}
}

bool Tile::removeThing(unsigned char stackpos)
{
    Thing *thing = getThing(stackpos);
    if(!thing)
        return false;

    Creature* creature = dynamic_cast<Creature*>(thing);
	if(creature)
    {
		CreatureList::iterator it = std::find(creatures.begin(), creatures.end(), thing);
    
		if(it != creatures.end())
            creatures.erase(it);
	}
	else 
    {
		Item* item = dynamic_cast<Item*>(thing);
        if(!item)
            return false;

		if(item == ground) 
        {
            delete ground;
			ground = NULL;
			return true;
		}

		ItemVector::iterator iit;

		if(item->baseItem->isAlwaysOnTop())
        {
			for(iit = topItems.begin(); iit != topItems.end(); ++iit)
            {
				if(*iit == item)
                {
					topItems.erase(iit);
					delete (*iit);
					return true;
				}
			}
		}		
		else 
        {
			for (iit = downItems.begin(); iit != downItems.end(); ++iit)
            {
				if(*iit == item)
                {
                    downItems.erase(iit);
					delete (*iit);
					return true;
				}
			}
		}
	}
	return false;
}

bool Tile::removeTopItem(unsigned short count)
{
	if(topItems.size() > 0)
	{
		topItems.erase(topItems.end()-1);
		return true;
	}
	if(downItems.size() > 0)
	{
		downItems.erase(downItems.end()-1);
		return true;
	}
	if(ground)
    {
		ground = NULL;
		return true;
	}
    return false;
}

bool Tile::removeCreatureByID(unsigned int _id)
{
   for (std::list<Creature*>::iterator it = creatures.begin(); it != creatures.end(); it++)
   {
      if((*it)->getId() == _id)
      {
         creatures.erase(it);
         return true;
      }
   }    
   return false;
	/*
   int temp = 0;
   for (deque<Creature*>::iterator it = creatures.begin(); it < creatures.end(); it++)
   {
      if((*it) && creatures[temp] && creatures[temp]->getId() == _id)
      {
         creatures.erase(it);
		 //delete (*it);
         return true;
      }
      temp++;
   }    
   return false;
   */
}

void Tile::render(int startWidth, int startHeight, int camera_x, int camera_y, unsigned char step)
{
	if(step == 0) //ground
    {
		offsetx = 0;
		offsety = 0;
		hasHeight = false;

        if(ground)
			System.renderItem(startWidth - camera_x*32 + pos.x*32,startHeight - camera_y*32 + pos.y*32, ground, true);
    }
    else if(step == 1) //down items
    {
		for(ItemVector::iterator it = topItems.begin(); it != topItems.end(); it++)
			if((*it) && (*it)->baseItem->alwaysOnTopOrder == 1 && (*it)->hasHeight())
			{
				hasHeight = true;
				break;
			}
		for(ItemVector::iterator it = downItems.begin(); it != downItems.end(); it++)
			if((*it) && (*it)->hasHeight())
			{
				hasHeight = true;
				break;
			}

		if(!this->hasHeight)
			for(ItemVector::iterator it = downItems.begin(); it != downItems.end(); it++)
			{
				if((*it))
				{
					System.renderItem(startWidth - camera_x*32 + pos.x*32 - offsetx,startHeight - camera_y*32 + pos.y*32 - offsety, *it, true);
					if((*it)->hasHeight() && offsetx < 24 && offsety < 24)
					{
						offsetx += 8;
						offsety += 8;
					}
				}
			}
    }
    else if(step == 2) //top items and creatures
    {
		if(this->hasHeight)
			for(ItemVector::iterator it = downItems.begin(); it != downItems.end(); it++)
			{
				if(*it)
				{
					System.renderItem(startWidth - camera_x*32 + pos.x*32 - offsetx,startHeight - camera_y*32 + pos.y*32 - offsety, *it, true);
					if((*it)->hasHeight() && offsetx < 24 && offsety < 24)
					{
						offsetx += 8;
						offsety += 8;
					}
				}
			}
		for(CreatureList::iterator it = creatures.begin(); it != creatures.end(); it++)
		{
				System.renderCreature(startWidth - camera_x*32 + pos.x*32 - offsetx,startHeight - camera_y*32 + pos.y*32 - offsety, *it, true);
		}
		for(ItemVector::iterator it = topItems.begin(); it != topItems.end(); it++)
        {
			if((*it) && (*it)->baseItem && (*it)->baseItem->alwaysOnTopOrder == 0)
			{
				System.renderItem(startWidth - camera_x*32 + pos.x*32 - offsetx,startHeight - camera_y*32 + pos.y*32 - offsety, *it, true);
				if((*it)->hasHeight() && offsetx < 24 && offsety < 24)
				{
					offsetx += 8;
					offsety += 8;
				}
			}
        }
	}
    else if(step == 3) //effects and top items
	{
		if(!effects.empty())
			for(EffectVector::size_type n = 0; n < effects.size();)
			{
				if(effects[n]->getSpriteNumber() == 0)
				{
					delete effects[n];
					effects.erase(effects.begin() + n);
				}
				else
				{
					System.renderEffect(startWidth - camera_x*32 + pos.x*32 - offsetx,startHeight - camera_y*32 + pos.y*32 - offsety, *(&effects[n]), true);
					n++;
				}
			}
		for(ItemVector::iterator it = topItems.begin(); it != topItems.end(); it++)
        {
			if((*it) && (*it)->baseItem && (*it)->baseItem->alwaysOnTopOrder == 1)
			{
				System.renderItem(startWidth - camera_x*32 + pos.x*32 - offsetx,startHeight - camera_y*32 + pos.y*32 - offsety, *it, true);
				if((*it)->hasHeight() && offsetx < 24 && offsety < 24)
				{
					offsetx += 8;
					offsety += 8;
				}
			}
        }
	}
    else if(step == 4) //creatures names and textEffects
	{
		for(CreatureList::iterator it = creatures.begin(); it != creatures.end(); it++)
			if(!creatures.empty() && *it && (*it)->active)
				System.renderCreatureName(startWidth - camera_x*32 + pos.x*32 - offsetx - 32,startHeight - camera_y*32 + pos.y*32 - offsety - 10, *it, true);
		if(!textEffects.empty())
			for(TextEffectVector::size_type n = 0; n < textEffects.size();)
			{
				if(textEffects[n]->isToDelete() == 1)
				{
					delete textEffects[n];
					textEffects.erase(textEffects.begin() + n);
				}
				else
				{
					System.renderTextEffect(startWidth - camera_x*32 + pos.x*32 - offsetx,startHeight - camera_y*32 + pos.y*32 - offsety, *(&textEffects[n]), true);
					n++;
				}
			}
	}
	else if(step == 5) //lights
	{
		for(CreatureList::iterator it = creatures.begin(); it != creatures.end(); it++)
			if(!creatures.empty() && *it && (*it)->active)
				System.renderCreatureLight(startWidth - camera_x*32 + pos.x*32, startHeight - camera_y*32 + pos.y*32,*it);
	}
}