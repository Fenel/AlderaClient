#include <deque>
#include <string>
#include <iostream>
#include <fstream>

#include "creature.h"
#include "map.h"
#include "creaturesManager.h"

extern Map Map;
using namespace std;

CreaturesManager::CreaturesManager()
{
	#ifdef _DEBUG
		std::cout << "CreaturesManager loaded." << std::endl;
	#endif
}


Creature* CreaturesManager::getCreature(unsigned int _id)
{
   for (deque<Creature>::iterator it = creaturesList.begin(); it < creaturesList.end(); it++)
   {
      if (it->getId() == _id && it->active)
	  {
		return &(*it);
		break;
	  }
   }
   return NULL;
}

bool CreaturesManager::createCreature(unsigned int posx, unsigned int posy, unsigned char posz, unsigned int pid, string name, unsigned char looktype, unsigned int speed)
{
	if(!this->getCreature(pid))
	{
		creaturesList.push_back(Creature());
		creaturesList[creaturesList.size()-1].setPos(posx, posy, posz);
		creaturesList[creaturesList.size()-1].setId(pid);
		creaturesList[creaturesList.size()-1].setName(name);
		creaturesList[creaturesList.size()-1].setLooktype(looktype);
		creaturesList[creaturesList.size()-1].setSpeed(speed);
		return true;
	}
	else if(this->getCreature(pid))
	{
		this->getCreature(pid)->setPos(posx, posy, posz);
		this->getCreature(pid)->setId(pid);
		this->getCreature(pid)->setName(name);
		this->getCreature(pid)->setLooktype(looktype);
		this->getCreature(pid)->setSpeed(speed);
		return true;
	}
	else
		return false;
}

bool CreaturesManager::eraseCreature(unsigned int pid)
{
   for (deque<Creature>::iterator it = creaturesList.begin(); it < creaturesList.end(); it++)
   {
      if (it->getId() == pid)
	  {
		it->active = false;
		//Creature *creature = &(*it);
		//creaturesList.erase(it);
		//delete creature;
		return true;
	  }
   }
   return false;
}