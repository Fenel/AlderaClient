#include <SFML/Graphics.hpp>
#include <SFML/Network.hpp>
#include <Windows.h>
#include <iostream>
#include <fstream>
#include "game.h"
#include "system.h"
#include "BaseItems.h"
#include "chat.h"
#include "itemStorage.h"
#include "equWindow.h"
#include "hotkeys.h"
#include "map.h"
#include "creaturesManager.h"
#include "containerWindow.h"
#include "statisticsWindow.h"
#include "npcWindow.h"
#include "menu.h"

//extern sf::RenderWindow *App;
extern System System;
extern BaseItems BaseItems;
extern Chat Chat;
extern ItemStorage ItemStorage;
extern EquWindow EquWindow;
extern StatisticsWindow StatisticsWindow;
extern NPCWindow NPCWindow;
extern Hotkeys Hotkeys;
extern Map Map;
extern CreaturesManager CreaturesManager;
extern sf::Sprite storageItems;
//extern struct KeyStorage keyStorage;
extern std::string intToStr(int n);
#ifndef _DEBUG
	extern Menu Menu;
#endif

ItemType itemToDisplay;
ItemAbilities itemToDisplayAbilities;

sf::Texture guiItemInfoTexture;
sf::Sprite guiItemInfoSprite;
sf::RenderTexture guiItemInfoRenderTexture;
sf::Sprite guiItemInfoRenderTextureSprite;

using namespace std;

Game::Game()
{
	guiItemInfoTexture.loadFromFile("data/guiItemInfo.bmp");
	guiItemInfoSprite.setTexture(guiItemInfoTexture);
	guiItemInfoSprite.setPosition(0.0, 0.0);
	guiItemInfoRenderTextureSprite.setTexture(guiItemInfoRenderTexture.getTexture());
	guiItemInfoRenderTextureSprite.setPosition(0.0, 0.0);

	lastMeleeAttack = 0;
	myPlayerTarget = NULL;

	canRenderMap = false;
	mouseHoldingParentCounter = 0;
	sf::Vector2u size = System.App->getSize();
	ItemStorage.posx = size.x - 180;
	ItemStorage.posy = size.y-227;
	Map.posx = 32;
	Map.posy = 32;

	System.renderedMapTexture = new sf::RenderTexture();
	System.renderedMapTexture->create(19*32,15*32);
	renderedMap.setTexture(System.renderedMapTexture->getTexture());
	renderedMap.setPosition(0,0);

	Chat.setPos(3,32+13*32+40,size.x-190,size.y-3);

	this->updateMapRects();

	containerWindowTexture.loadFromFile("data/containerWindow.bmp");
	this->closeContainerWindow = false;

	this->loadLooktypes();

	this->itemCharges.setFont(System.font);
	this->itemCharges.setString("1");
	this->itemCharges.setCharacterSize(12);

	this->lightMapTexture = new sf::RenderTexture();
	this->lightMapTexture->create(19*32,15*32);
	this->lightMapTexture->clear(sf::Color(0, 0, 0, 200));

	this->lightMap = new sf::Sprite();
	this->lightMap->setTexture(lightMapTexture->getTexture());
	this->lightMap->setPosition(0, 0);
	this->lightMap->setScale(1.0f,1.0f);

	System.App->setFramerateLimit(30);
}

void Game::loadLooktypes()
{
	#ifdef _DEBUG
		std::cout << "::Loading outfitData.txt...";
	#endif

	vector<std::string> lines;
	vector<std::string> elements;
	vector<std::string> names;
	vector<std::string> values;
	string tmp;

	std::ifstream file ("data/outfitData.txt");
    if(file.is_open())
    {
       while(file)
       {
	      tmp.clear();
          getline(file, tmp,'\n');
          lines.push_back(tmp);
       }   
       file.close(); 
	}
	else
    {
		#ifdef _DEBUG
			std::cout << "\n::Error while opening outfitData.txt\n";
		#endif
    }

	for(unsigned int i = 0; i < lines.size(); i++) //dla wszystkich linii
	{
		string element;
		while(lines[i].find_first_of(";") != string::npos) //je¿eli linia zawiera ";"
		{
			element = lines[i].substr(0,lines[i].find_first_of(";")); //element =zbiór nazwa+wartoœæ
			lines[i].erase(0,lines[i].find_first_of(";")+2);

			elements.push_back(element);
			element.clear();
		}
		for(unsigned int j = 0; j < elements.size(); j++)
		{
			string name = elements[j];
			name.erase(name.find_first_of("="));
			names.push_back(name);

			string value = elements[j];
			value.erase(0,value.find_first_of("=")+1);
			values.push_back(value);
		}

		looktypes.push_back(Looktype());
		for(unsigned int i = 0; i < names.size(); i++)
		{
			if(names[i] == "id")
				looktypes.at(looktypes.size()-1).id = atoi(values[i].c_str());
			if(names[i] == "name")
				looktypes.at(looktypes.size()-1).name = values[i].c_str();
			if(names[i] == "type")
				looktypes.at(looktypes.size()-1).type = char(atoi(values[i].c_str()));
			if(names[i] == "north")
				looktypes.at(looktypes.size()-1).north = atoi(values[i].c_str());
			if(names[i] == "south")
				looktypes.at(looktypes.size()-1).south = atoi(values[i].c_str());
			if(names[i] == "west")
				looktypes.at(looktypes.size()-1).west = atoi(values[i].c_str());
			if(names[i] == "east")
				looktypes.at(looktypes.size()-1).east = atoi(values[i].c_str());
		}

		elements.clear();
		names.clear();
		values.clear();
	}
	#ifdef _DEBUG
		std::cout << " Done! ";
		std::cout << "Loaded " << looktypes.size() << " looktypes." << endl;
	#endif
}

inline unsigned int Game::getFrameRate()
{
	static sf::Time thetime;
	static sf::Clock elapsed;
	static int frameCounter = 0;
	static int fps = 0;
	frameCounter++;
	thetime = elapsed.getElapsedTime();
	if(thetime.asMilliseconds() > 999)
	{
	   fps = frameCounter;
	   std::string title = "Aldera: ";
	   title += intToStr(fps);
	   title += " FPS";
	   System.App->setTitle(title);
	   frameCounter = 0;
	   elapsed.restart();
	}
	return fps;
}

void Game::updateMapRects()
{
	sf::Vector2u size = System.App->getSize();
	rectBottom.setSize(sf::Vector2f(float(size.x), float(Map.posy)));
	rectBottom.setFillColor(sf::Color(80,80,80));
	rectBottom.setOutlineThickness(0);
	rectBottom.setPosition(float(0), float(0));

	rectMiddle.setSize(sf::Vector2f(float(size.x), float(size.y - Map.posy+13*32)));
	rectMiddle.setFillColor(sf::Color(80,80,80));
	rectMiddle.setOutlineThickness(0);
	rectMiddle.setPosition(float(0), float(Map.posy+13*32));

	rectLeft.setSize(sf::Vector2f(float(Map.posx), float(13*32)));
	rectLeft.setFillColor(sf::Color(80,80,80));
	rectLeft.setOutlineThickness(0);
	rectLeft.setPosition(float(0), float(Map.posy));

	rectRight.setSize(sf::Vector2f(float(size.x-Map.posx-17*32), float(13*32)));
	rectRight.setFillColor(sf::Color(80,80,80));
	rectRight.setOutlineThickness(0);
	rectRight.setPosition(float(Map.posx+17*32), float(Map.posy));
}

void Game::receiveDeEquipItem(sf::Packet received)
{
	unsigned short slot;
	unsigned short count;

	received >> slot >> count;

	if(slot == 1)
	{
		EquWindow.lRingEar->count -= count;
		if(EquWindow.lRingEar->count == 0 || count == 0)
		{
			delete EquWindow.lRingEar;
			EquWindow.lRingEar = NULL;
		}
	}
	else if(slot == 2) 
	{
		EquWindow.head->count -= count;
		if(EquWindow.head->count == 0 || count == 0)
		{
			delete EquWindow.head;
			EquWindow.head = NULL;
		}
	}
	else if(slot == 3) 
	{
		EquWindow.rRingEar->count -= count;
		if(EquWindow.rRingEar->count == 0 || count == 0)
		{
			delete EquWindow.rRingEar;
			EquWindow.rRingEar = NULL;
		}
	}
	else if(slot == 4)  
	{
		EquWindow.necklace->count -= count;
		if(EquWindow.necklace->count == 0 || count == 0)
		{
			delete EquWindow.necklace;
			EquWindow.necklace = NULL;
		}
	}
	else if(slot == 5) 
	{
		EquWindow.armor->count -= count;
		if(EquWindow.armor->count == 0 || count == 0)
		{
			delete EquWindow.armor;
			EquWindow.armor = NULL;
		}
	}
	else if(slot == 6)
	{
		EquWindow.backpack->count -= count;
		if(EquWindow.backpack->count == 0 || count == 0)
		{
			delete EquWindow.backpack;
			EquWindow.backpack = NULL;
		}
	}
	else if(slot == 7) 
	{
		EquWindow.weapon->count -= count;
		if(EquWindow.weapon->count == 0 || count == 0)
		{
			delete EquWindow.weapon;
			EquWindow.weapon = NULL;
		}
	}
	else if(slot == 8)   
	{
		EquWindow.belt->count -= count;
		if(EquWindow.belt->count == 0 || count == 0)
		{
			delete EquWindow.belt;
			EquWindow.belt = NULL;
		}
	}
	else if(slot == 9) 
	{
		EquWindow.shield->count -= count;
		if(EquWindow.shield->count == 0 || count == 0)
		{
			delete EquWindow.shield;
			EquWindow.shield = NULL;
		}
	}
	else if(slot == 10)   
	{
		EquWindow.lRing->count -= count;
		if(EquWindow.lRing->count == 0 || count == 0)
		{
			delete EquWindow.lRing;
			EquWindow.lRing = NULL;
		}
	}
	else if(slot == 11)
	{
		EquWindow.legs->count -= count;
		if(EquWindow.legs->count == 0 || count == 0)
		{
			delete EquWindow.legs;
			EquWindow.legs = NULL;
		}
	}
	else if(slot == 12)  
	{
		EquWindow.rRing->count -= count;
		if(EquWindow.rRing->count == 0 || count == 0)
		{
			delete EquWindow.rRing;
			EquWindow.rRing = NULL;
		}
	}
	else if(slot == 13) 
	{
		EquWindow.gloves->count -= count;
		if(EquWindow.gloves->count == 0 || count == 0)
		{
			delete EquWindow.gloves;
			EquWindow.gloves = NULL;
		}
	}
	else if(slot == 14)
	{
		EquWindow.boots->count -= count;
		if(EquWindow.boots->count == 0 || count == 0)
		{
			delete EquWindow.boots;
			EquWindow.boots = NULL;
		}
	}
	else if(slot == 15)  
	{
		EquWindow.arrows->count -= count;
		if(EquWindow.arrows->count == 0 || count == 0)
		{
			delete EquWindow.arrows;
			EquWindow.arrows = NULL;
		}
	}
}

void Game::receiveEquipItem(sf::Packet received)
{
	unsigned short slot;
	unsigned int id;
	unsigned short count;

	received >> slot >> id >> count;

	Item *item = NULL;
	if(BaseItems.getItemType(id)->isContainer())
		item = new Container(id);
	else
		item = new Item(id,count);

	item->parent = myPlayer;

	if(slot == 1)
		EquWindow.lRingEar = item;
	else if(slot == 2) 
		EquWindow.head = item;
	else if(slot == 3)  
		EquWindow.rRingEar = item;
	else if(slot == 4)    
		EquWindow.necklace = item;
	else if(slot == 5) 
		EquWindow.armor = item;
	else if(slot == 6)
		EquWindow.backpack = item;
	else if(slot == 7) 
		EquWindow.weapon = item;
	else if(slot == 8)   
		EquWindow.belt = item;
	else if(slot == 9) 
		EquWindow.shield = item;
	else if(slot == 10)    
		EquWindow.lRing = item;
	else if(slot == 11)
		EquWindow.legs = item;
	else if(slot == 12)   
		EquWindow.rRing = item;
	else if(slot == 13)   
		EquWindow.gloves = item;
	else if(slot == 14)
		EquWindow.boots = item;
	else if(slot == 15)  
		EquWindow.arrows = item;
}

void Game::receivePlayerUpdateMana(sf::Packet received)
{
	received >> this->myPlayerMana;
	EquWindow.update();
}

void Game::receivePlayerStats(sf::Packet received)
{
	int voc;
	received >> this->myPlayerLevel >> this->myPlayerExperience >> this->myPlayerSex >> voc>> this->myPlayerSpeed;
	received >> this->myPlayerSkillPoints >> this->myPlayerStatisticsPoints >> this->myPlayerStrength >> this->myPlayerDexterity >> this->myPlayerIntelligence;
	received >> this->myPlayerMagicPower >> this->myPlayerHealth >> this->myPlayerMaxHealth >> this->myPlayerMana >> this->myPlayerMaxMana;

	this->myPlayerVocation = Vocation(voc);

	if(this->myPlayer)
	{
		this->myPlayer->setSpeed(this->myPlayerSpeed);
		this->myPlayer->setHealth(this->myPlayerHealth);
		this->myPlayer->setMaxHealth(this->myPlayerMaxHealth);
	}
	this->myPlayerHealthPercent = int(float(myPlayerHealth)/float(myPlayerMaxHealth)*100.0f);

	StatisticsWindow.update();
	EquWindow.update();
}

void Game::receiveTile(sf::Packet received)
{    
	unsigned int posx, posy;
	unsigned char posz;
	unsigned int itemCount;

	received >> posx >> posy >> posz >> itemCount; //itemCount deprecated

	//std::cout << posx << "  " << posy << "  " << int(posz) << std::endl;

	Tile* tilet = new Tile(posx, posy, posz);

	while(received)
	{
		unsigned int itemID = 0;
		unsigned short count = 1;
		received >> itemID >> count;
		if(itemID > 0 && itemID < BaseItems.getItemsCount())
		{
			if(BaseItems.getItemType(itemID)->isContainer())
				tilet->addThing(new Container(itemID));
			else
				tilet->addThing(new Item(itemID,count)); 
		}
		//else
		{
			//#ifdef _DEBUG
			//cout << "Error, itemID too big or too low, while processing receiveTile() function!" << endl;
			//#endif
		}
	}
	Map.setTile(posx,posy,posz, tilet);
} 

void Game::receiveRemoveItemFromContainerOnMap(sf::Packet received)
{
	unsigned int posx, posy;
	unsigned char posz, index, index2;

	received >> posx >> posy >> posz >> index >> index2;

	Tile *tile = Map.getTile(posx, posy, posz);
	if(tile)
	{
		Container *container = dynamic_cast<Container*>(tile->getThing(index));
		if(container)
		{
			container->eraseItem(index2);

			if(containerWindow && containerWindow->container && container == containerWindow->container && containerWindow->isReadyToClose())
				this->closeContainerWindow = true;
		}
	}
}

void Game::receiveItemInfo(sf::Packet received)
{
	received >> itemToDisplay.name >> itemToDisplay.pluralName >> itemToDisplay.armor >> itemToDisplay.attack >> itemToDisplay.defence;
	received >> itemToDisplay.description;
	received >> itemToDisplay.minReqDex >> itemToDisplay.minReqInt >> itemToDisplay.minReqMPW >> itemToDisplay.minReqStr;
	bool hasAbilities = false;
	received >> hasAbilities;
	if(hasAbilities)
	{
		int tempElementType;
		received >> itemToDisplayAbilities.addHealth >> itemToDisplayAbilities.addMana >> itemToDisplayAbilities.addSkillDex >> itemToDisplayAbilities.addSkillInt;
		received >> itemToDisplayAbilities.addSkillMPW >> itemToDisplayAbilities.addSkillStr >> itemToDisplayAbilities.addSpeed >> itemToDisplayAbilities.elementDamage;
		received >> tempElementType >> itemToDisplayAbilities.upgradeLevel >> itemToDisplayAbilities.text;
		received >> itemToDisplayAbilities.resistance[0] >> itemToDisplayAbilities.resistance[1] >> itemToDisplayAbilities.resistance[2];
		received >> itemToDisplayAbilities.resistance[3] >> itemToDisplayAbilities.resistance[4] >> itemToDisplayAbilities.resistance[5];
		received >> itemToDisplayAbilities.resistance[6] >> itemToDisplayAbilities.resistance[7] >> itemToDisplayAbilities.resistance[8];
		received >> itemToDisplayAbilities.resistance[9];
		itemToDisplayAbilities.elementType = CombatType(tempElementType);
	}
}

void Game::receiveAddItemToContainerOnMap(sf::Packet received)
{
	unsigned int posx, posy, itemID;
	unsigned short count;
	unsigned char posz, index;

	received >> posx >> posy >> posz >> index >> itemID >> count;
	Item *item = new Item(itemID, count);

	Tile *tile = Map.getTile(posx, posy, posz);
	if(tile)
	{
		Container *container = dynamic_cast<Container*>(tile->getThing(index));
		if(container)
		{
			container->addItem(item);
			#ifdef _DEBUG
				std::cout << "Added item to container, itemID: " << itemID << ", count: " << int(count) << std::endl;
			#endif
		}
	}
}

void Game::receiveAddThingToTile(sf::Packet received)
{
	unsigned int posx, posy, itemID;
	unsigned short count;
	unsigned char posz;

	received >> posx >> posy >> posz >> itemID >> count;

	Tile *tile = Map.getTile(posx, posy, posz);
	if(!tile)
	{
		tile = new Tile(posx, posy, posz);
		Map.setTile(posx, posy, posz, tile);
	}
	if(BaseItems.getItemType(itemID)->isContainer())
		tile->addThing(new Container(itemID));
	else
		tile->addThing(new Item(itemID, count));
}

bool Game::receiveRemoveThingFromTile(sf::Packet received)
{
	unsigned int posx, posy;
	unsigned char posz, stackpos;

	received >> posx >> posy >> posz >> stackpos;

	Tile *tile = Map.getTile(posx, posy, posz);
	if(tile)
		tile->removeThing(stackpos);
	else
		return false;

	return true;
}

bool Game::receiveRemoveTopItemFromTile(sf::Packet received)
{
	unsigned int posx, posy;
	unsigned short count;
	unsigned char posz;

	received >> posx >> posy >> posz >> count;

	Tile *tile = Map.getTile(posx, posy, posz);
	if(tile)
		tile->removeTopItem(count);
	else
		return false;

	return true;
}

bool Game::receiveTransformThingFromTile(sf::Packet received)
{
	unsigned int posx, posy;
	unsigned char posz, stackpos;
	unsigned int id;
	unsigned short count;

	received >> posx >> posy >> posz >> stackpos >> id >> count;

	Tile *tile = Map.getTile(posx, posy, posz);
	if(tile)
	{
		tile->transformThing(stackpos, new Item(id, count));
		tile->getThing(stackpos)->parent = tile;
	}
	else
	{
		return false;
	}
	
	return true;
}

void Game::receiveAddItemToStorage(sf::Packet toReceive)
{
	unsigned int bpid, itemid;
	unsigned short count;

	toReceive >> bpid >> itemid >> count;

	if(BaseItems.getItemType(itemid)->stackable)
	{
		Item* itemOld = ItemStorage.getItemId(itemid);
		if(itemOld && itemOld->count < 1000 && itemOld->count + count <= 1000)
		{
			itemOld->count += count;
		}
		else if(itemOld && itemOld->count < 1000 && itemOld->count + count > 1000)
		{
			unsigned short newItemCount = itemOld->count + count - 1000;
			itemOld->count = 1000;
			ItemStorage.addItem(new Item(itemid, newItemCount));
		}
		else
		{
			ItemStorage.addItem(new Item(itemid, count));
		}
	}
	else
		ItemStorage.addItem(new Item(itemid, count));
}

void Game::receiveRemoveItemFromStorage(sf::Packet toReceive)
{
	unsigned int bpid;
	unsigned char index;

	toReceive >> bpid >> index;

	if(bpid == 20)
	  ItemStorage.eraseItem(index);
}

void Game::receiveRemoveItemFromAnyStorage(sf::Packet toReceive)
{
	unsigned int itemid;
	unsigned short count;

	toReceive >> itemid >> count;

	ItemStorage.eraseItemId(itemid, count);
}

void Game::receiveCreatureAppearOrMove(sf::Packet received)
{
	unsigned int posx, posy, pid, speed;
	unsigned char posz, healthPercent, looktype, direction, lightLevel;
	bool move;
	string name;
	int diff_x = 0, diff_y = 0;

	received >> posx >> posy >> posz >> pid >> name >> healthPercent >> looktype >> direction >> speed >> lightLevel >> move;

	Creature *creature = CreaturesManager.getCreature(pid);
	if(creature == this->myPlayer)
		closeContainerWindow = true;

	if(creature && move && creature->active)
	{
		creature->active = true;
		creature->updateHealthBar(healthPercent);
		creature->setLightLevel(lightLevel);

		if(direction == 4)
		  diff_x = -1;
		else if(direction == 6)
		  diff_x = 1;
		else if(direction == 8)
		  diff_y = -1;
		else if(direction == 2)
		  diff_y = 1;
		// DIAGONAL MOVEMENT
		else if(direction == 7)
		{
		  diff_x = -1;
		  diff_y = -1;
		}
		else if(direction == 9)
		{
		  diff_x = 1;
		  diff_y = -1;
		}
		else if(direction == 1)
		{
		  diff_x = -1;
		  diff_y = 1;
		}
		else if(direction == 3)
		{
		  diff_x = 1;
		  diff_y = 1;
		}
		creature->setDirection(direction);

		Tile *tile = Map.getTile(posx - diff_x, posy - diff_y, posz);
		if(!tile || !tile->removeCreatureByID(pid))
		{
			Map.removeCreatureFromTiles(pid);
		}

		tile = Map.getTile(posx, posy, posz);
		if(tile)
		{
			tile->addThing(creature);
			creature->setPos(posx, posy, posz);
			creature->setStartedWalking();
		}

		if(pid == this->myPlayerID)
		{
			this->myPlayerX = posx;
			this->myPlayerY = posy;
			this->myPlayerZ = posz;

			unsigned char zz1, zz2;
			if(myPlayerZ >= 7)
			{
				zz1 = 7;
				zz2 = 14;
			}
			else
			{
				zz1 = myPlayerZ - 2;
				zz2 = 6;
			}

			if(direction == 4)
				for(zz1; zz1 <= zz2; zz1++)
				for(unsigned int y = myPlayerY - 7; y <= myPlayerY + 8; y++)
				{
					Tile *tile = Map.getTile(myPlayerX + 11, y, zz1);
					if(tile)
					{
						std::list<Creature*> creaturesList = tile->getCreatures();
						for(std::list<Creature*>::iterator it = creaturesList.begin(); it != creaturesList.end(); it++)
						{
							unsigned int tempid = (*it)->getId();
							tile->removeCreatureByID(tempid);
							CreaturesManager.eraseCreature(tempid);
						}
					}
				}
			if(direction == 6)
				for(zz1; zz1 <= zz2; zz1++)
				for(unsigned int y = myPlayerY - 7; y <= myPlayerY + 8; y++)
				{
					Tile *tile = Map.getTile(myPlayerX - 10, y, zz1);
					if(tile)
					{
						std::list<Creature*> creaturesList = tile->getCreatures();
						for(std::list<Creature*>::iterator it = creaturesList.begin(); it != creaturesList.end(); it++)
						{
							unsigned int tempid = (*it)->getId();
							tile->removeCreatureByID(tempid);
							CreaturesManager.eraseCreature(tempid);
						}
					}
				}
			if(direction == 8)
				for(zz1; zz1 <= zz2; zz1++)                  
				for(unsigned int x = myPlayerX - 9; x <= myPlayerX + 10; x++)  
				{
					Tile *tile = Map.getTile(x, myPlayerY + 9, zz1);
					if(tile)
					{
						std::list<Creature*> creaturesList = tile->getCreatures();
						for(std::list<Creature*>::iterator it = creaturesList.begin(); it != creaturesList.end(); it++)
						{
							unsigned int tempid = (*it)->getId();
							tile->removeCreatureByID(tempid);
							CreaturesManager.eraseCreature(tempid);
						}
					}
				}
			if(direction == 2)
				for(zz1; zz1 <= zz2; zz1++)                    
				for(unsigned int x = myPlayerX - 9; x <= myPlayerX + 10; x++)
				{
					Tile *tile = Map.getTile(x, myPlayerY - 8, zz1);
					if(tile)
					{
						std::list<Creature*> creaturesList = tile->getCreatures();
						for(std::list<Creature*>::iterator it = creaturesList.begin(); it != creaturesList.end(); it++)
						{
							unsigned int tempid = (*it)->getId();
							tile->removeCreatureByID(tempid);
							CreaturesManager.eraseCreature(tempid);
						}
					}
				}
			if(direction == 7)
			{
				for(unsigned char zz1_t = zz1; zz1_t <= zz2; zz1_t++)                  
				for(unsigned int x = myPlayerX - 9; x <= myPlayerX + 10; x++)  
				{
					Tile *tile = Map.getTile(x, myPlayerY + 9, zz1_t);
					if(tile)
					{
						std::list<Creature*> creaturesList = tile->getCreatures();
						for(std::list<Creature*>::iterator it = creaturesList.begin(); it != creaturesList.end(); it++)
						{
							unsigned int tempid = (*it)->getId();
							tile->removeCreatureByID(tempid);
							CreaturesManager.eraseCreature(tempid);
						}
					}
				}
				for(zz1; zz1 <= zz2; zz1++)
				for(unsigned int y = myPlayerY - 7; y <= myPlayerY + 8; y++)
				{
					Tile *tile = Map.getTile(myPlayerX + 11, y, zz1);
					if(tile)
					{
						std::list<Creature*> creaturesList = tile->getCreatures();
						for(std::list<Creature*>::iterator it = creaturesList.begin(); it != creaturesList.end(); it++)
						{
							unsigned int tempid = (*it)->getId();
							tile->removeCreatureByID(tempid);
							CreaturesManager.eraseCreature(tempid);
						}
					}
				}
			}
			if(direction == 9)
			{
				for(unsigned char zz1_t = zz1; zz1_t <= zz2; zz1_t++) 
				for(unsigned int y = myPlayerY - 7; y <= myPlayerY + 8; y++)
				{
					Tile *tile = Map.getTile(myPlayerX - 10, y, zz1_t);
					if(tile)
					{
						std::list<Creature*> creaturesList = tile->getCreatures();
						for(std::list<Creature*>::iterator it = creaturesList.begin(); it != creaturesList.end(); it++)
						{
							unsigned int tempid = (*it)->getId();
							tile->removeCreatureByID(tempid);
							CreaturesManager.eraseCreature(tempid);
						}
					}
				}
				for(zz1; zz1 <= zz2; zz1++)                  
				for(unsigned int x = myPlayerX - 9; x <= myPlayerX + 10; x++)  
				{
					Tile *tile = Map.getTile(x, myPlayerY + 9, zz1);
					if(tile)
					{
						std::list<Creature*> creaturesList = tile->getCreatures();
						for(std::list<Creature*>::iterator it = creaturesList.begin(); it != creaturesList.end(); it++)
						{
							unsigned int tempid = (*it)->getId();
							tile->removeCreatureByID(tempid);
							CreaturesManager.eraseCreature(tempid);
						}
					}
				}
			}
			if(direction == 1)
			{
				for(unsigned char zz1_t = zz1; zz1_t <= zz2; zz1_t++) 
				for(unsigned int y = myPlayerY - 7; y <= myPlayerY + 8; y++)
				{
					Tile *tile = Map.getTile(myPlayerX + 11, y, zz1_t);
					if(tile)
					{
						std::list<Creature*> creaturesList = tile->getCreatures();
						for(std::list<Creature*>::iterator it = creaturesList.begin(); it != creaturesList.end(); it++)
						{
							unsigned int tempid = (*it)->getId();
							tile->removeCreatureByID(tempid);
							CreaturesManager.eraseCreature(tempid);
						}
					}
				}
				for(zz1; zz1 <= zz2; zz1++)                    
				for(unsigned int x = myPlayerX - 9; x <= myPlayerX + 10; x++)
				{
					Tile *tile = Map.getTile(x, myPlayerY - 8, zz1);
					if(tile)
					{
						std::list<Creature*> creaturesList = tile->getCreatures();
						for(std::list<Creature*>::iterator it = creaturesList.begin(); it != creaturesList.end(); it++)
						{
							unsigned int tempid = (*it)->getId();
							tile->removeCreatureByID(tempid);
							CreaturesManager.eraseCreature(tempid);
						}
					}
				}
			}
			if(direction == 3)
			{
				for(unsigned char zz1_t = zz1; zz1_t <= zz2; zz1_t++) 
				for(unsigned int y = myPlayerY - 7; y <= myPlayerY + 8; y++)
				{
					Tile *tile = Map.getTile(myPlayerX - 10, y, zz1_t);
					if(tile)
					{
						std::list<Creature*> creaturesList = tile->getCreatures();
						for(std::list<Creature*>::iterator it = creaturesList.begin(); it != creaturesList.end(); it++)
						{
							unsigned int tempid = (*it)->getId();
							tile->removeCreatureByID(tempid);
							CreaturesManager.eraseCreature(tempid);
						}
					}
				}
				for(zz1; zz1 <= zz2; zz1++)                    
				for(unsigned int x = myPlayerX - 9; x <= myPlayerX + 10; x++)
				{
					Tile *tile = Map.getTile(x, myPlayerY - 8, zz1);
					if(tile)
					{
						std::list<Creature*> creaturesList = tile->getCreatures();
						for(std::list<Creature*>::iterator it = creaturesList.begin(); it != creaturesList.end(); it++)
						{
							unsigned int tempid = (*it)->getId();
							tile->removeCreatureByID(tempid);
							CreaturesManager.eraseCreature(tempid);
						}
					}
				}
			}
		}
	}
	else if(creature)
	{
		creature->active = true;
		creature->updateHealthBar(healthPercent);
		creature->setLightLevel(lightLevel);
		
		Map.removeCreatureFromTiles(pid);

		Tile* tile = Map.getTile(posx, posy, posz);
		if(tile)
		{
			tile->addThing(creature);

			if(direction != 0)
				creature->setDirection(direction);

			creature->setPos(posx, posy, posz);
		}

		if(pid == this->myPlayerID)
		{
			this->myPlayerX = posx;
			this->myPlayerY = posy;
			this->myPlayerZ = posz;
		}

	}
	else
	{
		CreaturesManager.createCreature(posx, posy, posz, pid, name, looktype, speed);
		creature = CreaturesManager.getCreature(pid);
		
		if(creature)
		{
			creature->setDirection(direction);
			creature->active = true;
			creature->updateHealthBar(healthPercent);
			creature->setLightLevel(lightLevel);
		
			Tile *tile = Map.getTile(posx, posy, posz);
			if(tile)
				tile->addThing(creature);
			else
			{
				tile = new Tile(posx, posy, posz);
				tile->addThing(creature);
				Map.setTile(posx, posy, posz, tile);
			}
			if(pid == this->myPlayerID)
			{
				this->myPlayerX = posx;
				this->myPlayerY = posy;
				this->myPlayerZ = posz;
				this->myPlayer = creature;
			}
		}
	}
}

void Game::receiveCreatureUpdateHealth(sf::Packet received)
{
	unsigned int cid;
	unsigned char healthPercent;
	received >> cid >> healthPercent;

	if(CreaturesManager.getCreature(cid))
		CreaturesManager.getCreature(cid)->updateHealthBar(healthPercent);

	if(cid == this->myPlayerID)
	{
		this->myPlayerHealthPercent = healthPercent;
		EquWindow.update();
	}
}

void Game::receiveCreatureUpdateLightLevel(sf::Packet received)
{
	unsigned int cid;
	unsigned char lightLevel;
	received >> cid >> lightLevel;

	if(CreaturesManager.getCreature(cid))
		CreaturesManager.getCreature(cid)->setLightLevel(lightLevel);
}

void Game::receiveCreatureUpdateDirection(sf::Packet received)
{
	unsigned int cid;
	unsigned char direction;
	received >> cid >> direction;

	if(CreaturesManager.getCreature(cid))
		CreaturesManager.getCreature(cid)->setDirection(direction);
}

void Game::receiveCreatureUpdateSpeed(sf::Packet received)
{
	unsigned int cid;
	unsigned int speed;
	received >> cid >> speed;

	if(CreaturesManager.getCreature(cid))
		CreaturesManager.getCreature(cid)->setSpeed(speed);

	if(cid == this->myPlayerID)
		this->myPlayerSpeed = speed;
}

void Game::receiveCreatureDisappear(sf::Packet received)
{
	unsigned int cid;
	received >> cid;

	Creature *creature = CreaturesManager.getCreature(cid);
	if(creature)
	{
		if(myPlayerTarget == creature)
			myPlayerTarget = NULL;

		Tile *tile = Map.getTile(creature->getPosX(), creature->getPosY(), creature->getPosZ());
		if(tile)
		{
			tile->removeCreatureByID(cid);
			creature->active = false;
			CreaturesManager.eraseCreature(cid);
		}
	
	}
	if( cid == this->myPlayerID)
		System.App->close();
}

void Game::receiveTargetConfirmation(sf::Packet toReceive)
{
	unsigned int cid;
	bool cancel = false;
	toReceive >> cid >> cancel;

	if(cancel)
	{
		this->myPlayerTarget = NULL;
	}
	else
	{
		Creature *creature = CreaturesManager.getCreature(cid);
		if(creature && creature->active)
		{
			this->myPlayerTarget = creature;
		}
	}
}

 void Game::receiveEffect(sf::Packet toReceive)
 {
	int posx, posy;
	unsigned char posz, type;
	toReceive >> posx >> posy >> posz >> type;

	Tile *tile = Map.getTile(posx, posy, posz);
	if(tile)
	{
		tile->createEffect(type);
	}
 }
 
 void Game::receiveTextEffect(sf::Packet toReceive)
 {
	int posx, posy;
	unsigned char posz, color;
	string text;
	toReceive >> posx >> posy >> posz >> color >> text;

	Tile *tile = Map.getTile(posx, posy, posz);
	if(tile)
	{
		tile->createTextEffect(color, text);
	}
 }

void Game::receiveShootEffect(sf::Packet toReceive)
{
	int fromx, fromy, tox, toy;
	unsigned char fromz, toz, type;
	toReceive >> fromx >> fromy >> fromz >> tox >> toy >> toz >> type;
	
	ShootEffect *effect = new ShootEffect;
	effect->hasLight = false;

	if(type == 1)
		effect->sprite.setTexture(*BaseItems.getTexture(766)); //arrow
	else if(type == 2)
		effect->sprite.setTexture(*BaseItems.getTexture(783)); //bolt
	else if(type == 3)
	{
		effect->sprite.setTexture(*BaseItems.getTexture(792)); //fireball
		effect->light.setTexture(*System.lightObjectTexture);
		effect->light.setScale(3.0f, 3.0f);
		effect->hasLight = true;
	}
	else
		effect->sprite.setTexture(*BaseItems.getTexture(755)); //emergency use only

	effect->sprite.setOrigin(16.0, 16.0);
	effect->type = type;
	effect->posz = fromz;
	effect->fromx = fromx;
	effect->fromy = fromy;
	effect->tox = tox;
	effect->toy = toy;

	if(fromx > tox)
	{
		if(fromy > toy)
		{
			effect->sprite.setRotation(270+45);
		}
		if(fromy < toy)
		{
			effect->sprite.setRotation(270-45);
		}
		else
		{
			effect->sprite.setRotation(270);
		}
	}
	else if(fromx < tox)
	{
		if(fromy > toy)
		{
			effect->sprite.setRotation(90-45);
		}
		if(fromy < toy)
		{
			effect->sprite.setRotation(90+45);
		}
		else
		{
			effect->sprite.setRotation(90);
		}
	}
	else
	{
		if(fromy < toy)
		{
			effect->sprite.setRotation(180);
		}
	}

	unsigned char diffx = 0;
	unsigned char diffy = 0;

	if(fromx > tox)
		diffx = fromx - tox;
	else
		diffx = tox - fromx;

	if(fromy > toy)
		diffy = fromy - toy;
	else
		diffy = toy - fromy;

	unsigned char rangeToCompare = diffx*diffx + diffy*diffy;
	unsigned char distance = static_cast<unsigned char>(sqrt(float(rangeToCompare)));

	effect->started = clock();
	effect->range = distance*50;
	effect->expires = effect->started + effect->range;

	System.shootEffects.push_back(effect);
}

void Game::receiveCancelSpell(sf::Packet toReceive)
{
	unsigned char spellID;
	toReceive >> spellID;
	Hotkeys.cancelCooldown(spellID);
}

void Game::receiveNPCResponse(sf::Packet toReceive)
{
	string name;
	unsigned int dialogs, choices;
	string msg;
	string temp;
	unsigned char looktype;
	toReceive >> name >> looktype;

	if(looktype != 255)
	{
		NPCWindow.newConversation(name, looktype);

		toReceive >> dialogs;
		for(unsigned int i = 0; i < dialogs; i++)
		{
			msg.clear();
			toReceive >> msg;
			NPCWindow.addSaid(msg);
		}

		toReceive >> choices;
		for(unsigned int i = 0; i < choices; i++)
		{
			msg.clear();
			toReceive >> msg;
			NPCWindow.addChoice(msg);
		}
	}
	else
		NPCWindow.show = false;
}

void Game::receiveNPCshopSells(sf::Packet toReceive)
{
	NPCWindow.clearSells();
	unsigned int itemId;
	unsigned int price;
	while(!toReceive.endOfPacket())
	{
		toReceive >> itemId;
		toReceive >> price;
		NPCWindow.addSells(itemId, price, BaseItems.getItemType(itemId)->name);
	}
}
void Game::receiveNPCshopBuys(sf::Packet toReceive)
{
	NPCWindow.clearBuys();
	unsigned int itemId;
	unsigned int price;
	while(!toReceive.endOfPacket())
	{
		toReceive >> itemId;
		toReceive >> price;
		NPCWindow.addBuys(itemId, price, BaseItems.getItemType(itemId)->name);
	}
}

/*
void Game::receiveCreatureAppearFirstTime(sf::Packet received)
{
	unsigned int posx, posy, pid, speed;
	unsigned char posz, looktype, direction;
	string name;

	received >> posx >> posy >> posz >> pid >> name >> looktype >> direction >> speed;
	CreaturesManager.createCreature(posx, posy, posz, pid, name, looktype, speed);
	
	Tile *tile = Map.getTile(posx, posy, posz);
	if(tile)
		tile->addThing(CreaturesManager.getCreature(pid));
	else
	{
		tile = new Tile(posx, posy, posz);
		tile->addThing(CreaturesManager.getCreature(pid));
		Map.setTile(posx, posy, posz, tile);
	}
	if(pid == this->myPlayerID)
	{
		this->myPlayerX = posx;
		this->myPlayerY = posy;
		this->myPlayerZ = posz;
	}
}

void Game::receiveMoveCreature(sf::Packet received)
{
	unsigned int posx, posy, pid;
	unsigned char posz, direction;
	bool move;
	int diff_x = 0, diff_y = 0;

	received >> posx >> posy >> posz >> pid >> direction >> move;

	if(move)
	{
		if(direction == 4)
		  diff_x = -1;
		else if(direction == 6)
		  diff_x = 1;
		else if(direction == 8)
		  diff_y = -1;
		else if(direction == 2)
		  diff_y = 1;

		Creature *creature = CreaturesManager.getCreature(pid);
		creature->setDirection(direction);

		Tile *tile = Map.getTile(posx - diff_x, posy - diff_y, posz);
		if(tile)
			tile->removeCreatureByID(pid);

		tile = Map.getTile(posx, posy, posz);
		if(tile)
		{
			tile->addThing(creature);
			creature->setPos(posx, posy, posz);
			creature->setStartedWalking();
		}

		if(pid == this->myPlayerID)
		{
			this->myPlayerX = posx;
			this->myPlayerY = posy;
			this->myPlayerZ = posz;
		}
	}
}
*/


void Game::sendThrowItem(Item *item, unsigned int tox, unsigned int toy, unsigned char toz)
{
	bool proceed = true;
	int id = 170;
	sf::Packet toSend;
	toSend << id << item->mID << item->count << this->mouseHoldingParentId; //mouseHoldingParentId == bpid
	if(this->mouseHoldingParentId == 0) //je¿eli przesuwamy item z pola na mapie do innego pola, tutaj dodatkowe pola ze Ÿród³em itemu
	{
		Tile* tile = dynamic_cast<Tile*>(item->parent);
		if(tile)
		{
			toSend << tile->pos.x << tile->pos.y << tile->pos.z;
		}
		else
			proceed = false;
	}
	if(proceed)
	{
		toSend << tox << toy << toz;
		System.sClient.send(toSend);
	}
}

void Game::sendEquipItem(Item *item, unsigned char index, unsigned int bpid)
{
	int id = 171;
	sf::Packet toSend;

	Container *container = dynamic_cast<Container*>(item->parent);
	if(container && dynamic_cast<Tile*>(container->parent))
	{
		Tile *tile2 = dynamic_cast<Tile*>(container->parent);

		if(tile2)
		{
			toSend << id << true << tile2->getIndexOfThing(container) << mouseHoldingParentId << bpid << container->getItemIndex(item) 
				   << tile2->pos.x << tile2->pos.y << tile2->pos.z;
			System.sClient.send(toSend);
		}
	}
	else
	{
		unsigned char TO_REMOVE = 0;
		toSend << id << false << index << mouseHoldingParentId << bpid << TO_REMOVE;
		if(this->mouseHoldingParentId == 0) //podnosimy item z mapy
		{
			Tile* tile = dynamic_cast<Tile*>(item->parent);
			toSend << tile->pos.x << tile->pos.y << tile->pos.z;
		}
		System.sClient.send(toSend);  
	}
}

void Game::sendUseItem(unsigned int bpid, unsigned int fromx, unsigned int fromy, unsigned char fromz, unsigned int tox, unsigned int toy, unsigned char toz)
{
	int id = 180;
	sf::Packet toSend;
	toSend << id << bpid << fromx << fromy << fromz << tox << toy << toz;
	System.sClient.send(toSend);
}

void Game::sendAttackCreature(unsigned int cid, bool cancel = false)
{
	int id = 220;
	sf::Packet toSend;
	toSend << id << cid << cancel;
	System.sClient.send(toSend);
}

void Game::sendMessage(unsigned char type, string message, string to)
{
	int id = 200;
	sf::Packet toSend;
	toSend << id << type << message;
	if(to != "")
		toSend << to;
	System.sClient.send(toSend);
}

void Game::requestItemStats(unsigned int bpid, unsigned int fromx, unsigned int fromy, unsigned char fromz, unsigned char index)
{
	//bpid 0 = mapa, bpid 20 = plecak, bpid 1-15 equ
	int id = 175;
	sf::Packet toSend;
	toSend << id << bpid << fromx << fromy << fromz << index;
	System.sClient.send(toSend);
}

void Game::sendAddStatPoint(unsigned char index)
{
	int id = 99;
	sf::Packet toSend;
	toSend << id << index;
	System.sClient.send(toSend);
}

void Game::sendNPCResponse(unsigned char choice)
{
	int id = 600;
	sf::Packet toSend;
	toSend << id << choice;
	System.sClient.send(toSend);
}

void Game::sendNPCbuy(unsigned int choice)
{
	int id = 601;
	sf::Packet toSend;
	toSend << id << choice;
	System.sClient.send(toSend);
}

void Game::sendNPCsell(unsigned int choice)
{
	int id = 602;
	sf::Packet toSend;
	toSend << id << choice;
	System.sClient.send(toSend);
}

Thing* Game::getThingFromTile(unsigned char stackpos, Tile *tile)
{
	return tile->getThing(stackpos);
}

void Game::loop()
{ 
	#ifndef _DEBUG
		Menu.~Menu();
	#endif

	sf::Packet toSend;
	int id = -1;

	System.sClient.setBlocking(false);

	//this->receivingThread = new sf::Thread(&Game::doReceivingThread, this);
	//receivingThread->launch();
	//receivingThreadRunning = true;

	bool mouseMapOrEqu = false;

	sf::Packet toReceive;
	int recvid = -1;

	while(System.App->isOpen())
	{		
		sf::Event Event;
        while (System.App->pollEvent(Event))
        {
			if(Event.type == sf::Event::KeyPressed && Event.key.code == sf::Keyboard::L && sf::Keyboard::isKeyPressed(sf::Keyboard::LControl))
			{
				static bool setted = false;
				if(!setted)
				{
					System.App->setFramerateLimit(30);
					setted = true;
				}
				else
				{
					System.App->setFramerateLimit(0);
					setted = false;
				}
			}
			if(Event.type == Event.MouseButtonPressed && sf::Mouse::isButtonPressed(sf::Mouse::Left))
				mouse_b = 1;
			if(Event.type == Event.MouseButtonPressed && sf::Mouse::isButtonPressed(sf::Mouse::Right))
				mouse_b = 2;
			if(Event.type == Event.MouseButtonPressed && sf::Mouse::isButtonPressed(sf::Mouse::Middle))
				mouse_b = 3;
			if(Event.type == Event.MouseButtonReleased)
				mouse_b = 0;
			if(Event.type == Event.MouseMoved)
			{
				mouse_x = Event.mouseButton.button;//Event.mouseButton.x;
				mouse_y = Event.mouseButton.x;//Event.mouseButton.y;

				NPCWindow.mouse(0, mouse_x-NPCWindow.posx, mouse_y-NPCWindow.posy); //sprawdzam czy myszka jest nad napisami
			}
			if(Event.type == Event.MouseWheelMoved)
			{
				System.mouseDelta = Event.mouseWheel.delta;

				NPCWindow.mouse(137, mouse_x-NPCWindow.posx, mouse_y-NPCWindow.posy);
				Chat.mouse(137, mouse_x-Chat.posx, mouse_y-Chat.posy);
			}
			if(Event.type == sf::Event::TextEntered)
			{
				if(Chat.newConversation == false)
					Chat.updateToSend(Event.text.unicode);
				else
					Chat.updateNewConversation(Event.text.unicode);
			}
			if((Event.type == sf::Event::KeyPressed) && Event.key.control && (Event.key.code == sf::Keyboard::Num7 || Event.key.code == sf::Keyboard::Numpad7))
	        {
	           id = 120;
	           int direction = 7;
			   toSend.clear();
			   toSend << id << direction;
			   System.sClient.send(toSend);    
			   NPCWindow.show = false;
	        }
			if((Event.type == sf::Event::KeyPressed) && Event.key.control && (Event.key.code == sf::Keyboard::Num9 || Event.key.code == sf::Keyboard::Numpad9))
	        {
	           id = 120;
	           int direction = 9;
			   toSend.clear();
			   toSend << id << direction;
			   System.sClient.send(toSend);    
			   NPCWindow.show = false;
	        }
			if((Event.type == sf::Event::KeyPressed) && Event.key.control && (Event.key.code == sf::Keyboard::Num1 || Event.key.code == sf::Keyboard::Numpad1))
	        {
	           id = 120;
	           int direction = 1;
			   toSend.clear();
			   toSend << id << direction;
			   System.sClient.send(toSend);   
			   NPCWindow.show = false;
	        }
			if((Event.type == sf::Event::KeyPressed) && Event.key.control && (Event.key.code == sf::Keyboard::Num3 || Event.key.code == sf::Keyboard::Numpad3))
	        {
	           id = 120;
	           int direction = 3;
			   toSend.clear();
			   toSend << id << direction;
			   System.sClient.send(toSend);  
			   NPCWindow.show = false;
	        }
			if((Event.type == sf::Event::KeyPressed) && (Event.key.code == sf::Keyboard::Right))
	        {
				if(Event.key.control)
					id = 142;
				else
					id = 120;  

				int direction = 6;
				toSend.clear();
				toSend << id << direction;
				System.sClient.send(toSend);  
				NPCWindow.show = false;
	        }
	        if((Event.type == sf::Event::KeyPressed) && (Event.key.code == sf::Keyboard::Left))
	        {
				if(Event.key.control)
					id = 142;
				else
					id = 120;   

				int direction = 4;
				toSend.clear();
				toSend << id << direction;
				System.sClient.send(toSend); 
				NPCWindow.show = false;
	        }
	        if((Event.type == sf::Event::KeyPressed) && (Event.key.code == sf::Keyboard::Up))
	        {
				if(Event.key.shift)
					Chat.setLastSended();
				else
				{
					if(Event.key.control)
						id = 142;
					else
						id = 120;   

					int direction = 8;
					toSend.clear();
					toSend << id << direction;
					System.sClient.send(toSend); 
					NPCWindow.show = false;
				}
	        }
	        if((Event.type == sf::Event::KeyPressed) && (Event.key.code == sf::Keyboard::Down))
	        {
				if(Event.key.control)
					id = 142;
				else
					id = 120;   

				int direction = 2;
				toSend.clear();
				toSend << id << direction;
				System.sClient.send(toSend); 
				NPCWindow.show = false;
	        } 
	        if((Event.type == sf::Event::KeyPressed) && (Event.key.code == sf::Keyboard::S) && (Event.key.control))
	        {
				StatisticsWindow.show = !StatisticsWindow.show;
	        }
			if((Event.type == sf::Event::KeyPressed) && (Event.key.code == sf::Keyboard::Tab))
	        {
				Chat.nextActiveTab();
	        } 
			if((Event.type == sf::Event::KeyPressed) && (Event.key.code == sf::Keyboard::F1))
	        {
				id = 250;
				toSend.clear();
				toSend << id << static_cast<unsigned char>(1);
				System.sClient.send(toSend);
				Hotkeys.setLastUse(1);
	        } 
			if((Event.type == sf::Event::KeyPressed) && (Event.key.code == sf::Keyboard::F2))
	        {
				id = 250;
				toSend.clear();
				toSend << id << static_cast<unsigned char>(2);
				System.sClient.send(toSend); 
				Hotkeys.setLastUse(2);
	        } 
			if((Event.type == sf::Event::KeyPressed) && (Event.key.code == sf::Keyboard::F3))
	        {
				id = 250;
				toSend.clear();
				toSend << id << static_cast<unsigned char>(3);
				System.sClient.send(toSend);
				Hotkeys.setLastUse(3);
	        } 
			if((Event.type == sf::Event::KeyPressed) && (Event.key.code == sf::Keyboard::F4))
	        {
				id = 250;
				toSend.clear();
				toSend << id << static_cast<unsigned char>(4);
				System.sClient.send(toSend); 
				Hotkeys.setLastUse(4);
	        }
			if((Event.type == sf::Event::KeyPressed) && (Event.key.code == sf::Keyboard::F5))
	        {
				id = 250;
				toSend.clear();
				toSend << id << static_cast<unsigned char>(5);
				System.sClient.send(toSend); 
				Hotkeys.setLastUse(5);
	        } 
			if((Event.type == sf::Event::KeyPressed) && (Event.key.code == sf::Keyboard::F6))
	        {
				id = 250;
				toSend.clear();
				toSend << id << static_cast<unsigned char>(6);
				System.sClient.send(toSend); 
				Hotkeys.setLastUse(6);
	        } 
			if((Event.type == sf::Event::KeyPressed) && (Event.key.code == sf::Keyboard::F7))
	        {
				id = 250;
				toSend.clear();
				toSend << id << static_cast<unsigned char>(7);
				System.sClient.send(toSend);
				Hotkeys.setLastUse(7);
	        } 
			if((Event.type == sf::Event::KeyPressed) && (Event.key.code == sf::Keyboard::F8))
	        {
				id = 250;
				toSend.clear();
				toSend << id << static_cast<unsigned char>(8);
				System.sClient.send(toSend); 
				Hotkeys.setLastUse(8);
	        }
			if((Event.type == sf::Event::KeyPressed) && (Event.key.code == sf::Keyboard::F9))
	        {
				id = 250;
				toSend.clear();
				toSend << id << static_cast<unsigned char>(9);
				System.sClient.send(toSend); 
				Hotkeys.setLastUse(9);
	        }
			if((Event.type == sf::Event::KeyPressed) && (Event.key.code == sf::Keyboard::F10))
	        {
				id = 250;
				toSend.clear();
				toSend << id << static_cast<unsigned char>(10);
				System.sClient.send(toSend); 
				Hotkeys.setLastUse(10);
	        }
			if((Event.type == sf::Event::KeyPressed) && (Event.key.code == sf::Keyboard::F11))
	        {
				id = 250;
				toSend.clear();
				toSend << id << static_cast<unsigned char>(11);
				System.sClient.send(toSend); 
				Hotkeys.setLastUse(11);
	        }
			if((Event.type == sf::Event::KeyPressed) && (Event.key.code == sf::Keyboard::F12))
	        {
				id = 250;
				toSend.clear();
				toSend << id << static_cast<unsigned char>(12);
				System.sClient.send(toSend); 
				Hotkeys.setLastUse(12);
	        }
			if((Event.type == sf::Event::MouseButtonPressed) //wciœniêcie przycisku myszki na equ
				&& (Event.mouseButton.x > (800-180)) && (Event.mouseButton.x < 800) && (Event.mouseButton.y > 0)&& (Event.mouseButton.y < 227))
	        {
				if(sf::Mouse::isButtonPressed(sf::Mouse::Left) && !sf::Mouse::isButtonPressed(sf::Mouse::Right))
					EquWindow.mouse(1, Event.mouseButton.x-EquWindow.posx, Event.mouseButton.y-EquWindow.posy);
				if(sf::Mouse::isButtonPressed(sf::Mouse::Right) && !sf::Mouse::isButtonPressed(sf::Mouse::Left))
					EquWindow.mouse(3, Event.mouseButton.x-EquWindow.posx, Event.mouseButton.y-EquWindow.posy);
			} 
			if((Event.type == sf::Event::MouseButtonReleased) //puszczenie przycisku myszki na equ
				&& (Event.mouseButton.x > (800-180)) && (Event.mouseButton.x < 800) && (Event.mouseButton.y > 0)&& (Event.mouseButton.y < 227))
	        {
				if(Event.mouseButton.button == sf::Mouse::Left && !sf::Mouse::isButtonPressed(sf::Mouse::Right))
					EquWindow.mouse(4, Event.mouseButton.x-EquWindow.posx, Event.mouseButton.y-EquWindow.posy);
				if(Event.mouseButton.button == sf::Mouse::Right && !sf::Mouse::isButtonPressed(sf::Mouse::Left))
					EquWindow.mouse(6, Event.mouseButton.x-EquWindow.posx, Event.mouseButton.y-EquWindow.posy);

				this->mouseHolding = NULL;
	        } 
			if((Event.type == sf::Event::MouseButtonPressed) //wciœniêcie przycisku myszki na plecaku
				&& (Event.mouseButton.x > ItemStorage.posx) && (Event.mouseButton.x < ItemStorage.posx+180) && (Event.mouseButton.y > ItemStorage.posy)&& (Event.mouseButton.y < ItemStorage.posy+227))
	        {
				mouseMapOrEqu = true;
				#ifdef _DEBUG
					std::cout << "MB Pressed, ItemStorage." << std::endl;
				#endif

			    if(sf::Mouse::isButtonPressed(sf::Mouse::Left) && !sf::Mouse::isButtonPressed(sf::Mouse::Right) )
			       ItemStorage.mouse(1, Event.mouseButton.x-ItemStorage.posx, Event.mouseButton.y-ItemStorage.posy);
			    if(sf::Mouse::isButtonPressed(sf::Mouse::Right) && !sf::Mouse::isButtonPressed(sf::Mouse::Left))
			       ItemStorage.mouse(3, Event.mouseButton.x-ItemStorage.posx, Event.mouseButton.y-ItemStorage.posy);
	        } 
			if((Event.type == sf::Event::MouseButtonReleased) //puszczenie przycisku myszki na plecaku
				&& (Event.mouseButton.x > ItemStorage.posx) && (Event.mouseButton.x < ItemStorage.posx+180) && (Event.mouseButton.y > ItemStorage.posy)&& (Event.mouseButton.y < ItemStorage.posy+227))
	        {
				mouseMapOrEqu = true;
				#ifdef _DEBUG
					std::cout << "MB Released, ItemStorage." << std::endl;
				#endif

			    if(Event.mouseButton.button == sf::Mouse::Left && !sf::Mouse::isButtonPressed(sf::Mouse::Right))
			       ItemStorage.mouse(4, Event.mouseButton.x-ItemStorage.posx, Event.mouseButton.y-ItemStorage.posy);
			    if(Event.mouseButton.button == sf::Mouse::Right && !sf::Mouse::isButtonPressed(sf::Mouse::Left))
			       ItemStorage.mouse(6, Event.mouseButton.x-ItemStorage.posx, Event.mouseButton.y-ItemStorage.posy);
	        } 
			if((Event.type == sf::Event::MouseButtonReleased) //puszczenie przycisku myszki na chat'cie
				&& (Event.mouseButton.x > Chat.posx) && (Event.mouseButton.x < Chat.maxx) && (Event.mouseButton.y > Chat.posy)&& (Event.mouseButton.y < Chat.maxy))
	        {
				mouseMapOrEqu = true;
				#ifdef _DEBUG
					std::cout << "MB Released, Chat." << std::endl;
				#endif

			    if(Event.mouseButton.button == sf::Mouse::Left && !sf::Mouse::isButtonPressed(sf::Mouse::Right))
			       Chat.mouse(4, Event.mouseButton.x-Chat.posx, Event.mouseButton.y-Chat.posy);
			    if(Event.mouseButton.button == sf::Mouse::Right && !sf::Mouse::isButtonPressed(sf::Mouse::Left))
			       Chat.mouse(6, Event.mouseButton.x-Chat.posx, Event.mouseButton.y-Chat.posy);
	        } 
			if((Event.type == sf::Event::MouseButtonPressed) //wciœniêcie przycisku myszki na mapie
				&& (Event.mouseButton.x > Map.posx) && (Event.mouseButton.x < Map.posx+544) && (Event.mouseButton.y > Map.posy)&& (Event.mouseButton.y < Map.posy+416))
	        {
				mouseMapOrEqu = false;
				#ifdef _DEBUG
					std::cout << "MB Pressed, Map." << std::endl;
				#endif

				if(sf::Mouse::isButtonPressed(sf::Mouse::Left) && !sf::Mouse::isButtonPressed(sf::Mouse::Right) && StatisticsWindow.show
				&& StatisticsWindow.mouse(1,Event.mouseButton.x-StatisticsWindow.posx,Event.mouseButton.y-StatisticsWindow.posy))
					mouseMapOrEqu = true;

				if(Event.mouseButton.button == sf::Mouse::Left && !sf::Mouse::isButtonPressed(sf::Mouse::Right) && NPCWindow.show
				&& NPCWindow.mouse(1,Event.mouseButton.x-NPCWindow.posx,Event.mouseButton.y-NPCWindow.posy))
					mouseMapOrEqu = true;

				bool onContainer = false;
				if(sf::Mouse::isButtonPressed(sf::Mouse::Left) && !sf::Mouse::isButtonPressed(sf::Mouse::Right) && this->containerWindow 
				&& containerWindow->mouse(1,Event.mouseButton.x-containerWindow->posx,Event.mouseButton.y-containerWindow->posy))
					onContainer = true;
				if(sf::Mouse::isButtonPressed(sf::Mouse::Right) && !sf::Mouse::isButtonPressed(sf::Mouse::Left) && this->containerWindow 
				&& containerWindow->mouse(3,Event.mouseButton.x-containerWindow->posx,Event.mouseButton.y-containerWindow->posy))
					onContainer = true;

				if(!onContainer && !mouseMapOrEqu && sf::Mouse::isButtonPressed(sf::Mouse::Left) && !sf::Mouse::isButtonPressed(sf::Mouse::Right))
			       Map.mouse(1,Event.mouseButton.x-Map.posx,Event.mouseButton.y-Map.posy);

				if(!onContainer && !mouseMapOrEqu && sf::Mouse::isButtonPressed(sf::Mouse::Right) && !sf::Mouse::isButtonPressed(sf::Mouse::Left))
			       Map.mouse(3,Event.mouseButton.x-Map.posx,Event.mouseButton.y-Map.posy);

				mouseMapOrEqu = true;
	        } 
			if((Event.type == sf::Event::MouseButtonReleased) //puszczenie przycisku myszki na mapie
				&& (Event.mouseButton.x > Map.posx) && (Event.mouseButton.x < Map.posx+544) && (Event.mouseButton.y > Map.posy)&& (Event.mouseButton.y < Map.posy+416))
	        {
				mouseMapOrEqu = false;
				#ifdef _DEBUG
					std::cout << "MB Released, Map." << std::endl;
				#endif

				if(Event.mouseButton.button == sf::Mouse::Left && !sf::Mouse::isButtonPressed(sf::Mouse::Right) && StatisticsWindow.show
				&& StatisticsWindow.mouse(4,Event.mouseButton.x-StatisticsWindow.posx,Event.mouseButton.y-StatisticsWindow.posy))
					mouseMapOrEqu = true;

				if(Event.mouseButton.button == sf::Mouse::Left && !sf::Mouse::isButtonPressed(sf::Mouse::Right) && NPCWindow.show
				&& NPCWindow.mouse(4,Event.mouseButton.x-NPCWindow.posx,Event.mouseButton.y-NPCWindow.posy))
					mouseMapOrEqu = true;

				bool onContainer = false;
				if(Event.mouseButton.button == sf::Mouse::Left && !sf::Mouse::isButtonPressed(sf::Mouse::Right) && this->containerWindow 
				&& containerWindow->mouse(4,Event.mouseButton.x-containerWindow->posx,Event.mouseButton.y-containerWindow->posy))
					onContainer = true;
				if(Event.mouseButton.button == sf::Mouse::Right && !sf::Mouse::isButtonPressed(sf::Mouse::Left) && this->containerWindow 
				&& containerWindow->mouse(6,Event.mouseButton.x-containerWindow->posx,Event.mouseButton.y-containerWindow->posy))
					onContainer = true;

				if(!onContainer && !mouseMapOrEqu  && Event.mouseButton.button == sf::Mouse::Left && !sf::Mouse::isButtonPressed(sf::Mouse::Right))
			       Map.mouse(4,Event.mouseButton.x-Map.posx,Event.mouseButton.y-Map.posy);

				if(!onContainer && !mouseMapOrEqu  && Event.mouseButton.button == sf::Mouse::Right && !sf::Mouse::isButtonPressed(sf::Mouse::Left))
			       Map.mouse(6,Event.mouseButton.x-Map.posx,Event.mouseButton.y-Map.posy);

				mouseMapOrEqu = true;
				this->mouseHolding = NULL;
	        } 
			if(mouseMapOrEqu == false && (Event.type == sf::Event::MouseButtonPressed || Event.type == sf::Event::MouseButtonReleased)) //puszczenie przycisku myszki poza map¹ i ekwipunkiem
	        {
				#ifdef _DEBUG
					std::cout << "MB on nothing. Cleared references." << std::endl;
				#endif
				this->mouseHoldingParentId = -1;
				this->mouseHolding = NULL;
	        } 
			/*
			if(Event.type == sf::Event::Resized)
			{
				sf::Vector2u size = System.App->getSize();
				if(size.y != 600)
				{
					size.y = 600;
					System.App->setSize(size);
				}
				if(size.x != 800)
				{
					size.x = 800;
					System.App->setSize(size);
				}
				System.App->create(sf::VideoMode(size.x, size.y,32),"Fusion Client");
				//ItemStorage.posx = size.x  - 181 - 1;
				//ItemStorage.posy = size.y - 227;
				//EquWindow.setPos(size.x - 181 - 1, 32);
				//Chat.setPos(3,32+13*32+36,size.x-190,size.y-3);
				//this->updateMapRects();
			}
			*/
			if(Event.type == sf::Event::Closed)
			   System.App->close();
        }
		if( sf::Mouse::isButtonPressed(sf::Mouse::Left) && sf::Mouse::isButtonPressed(sf::Mouse::Right) )
		{
			sf::Vector2i localPosition = sf::Mouse::getPosition(*System.App);
			mouse_x = localPosition.x;
			mouse_y = localPosition.y;
			if( (mouse_x > (800-136)) && (mouse_x < 800) && (mouse_y > 0) && (mouse_y < 227) ) //equ
			{
				EquWindow.mouse(0, mouse_x - EquWindow.posx, mouse_y - EquWindow.posy);
			}
			if( ( mouse_x > ItemStorage.posx) && (mouse_x < ItemStorage.posx+180) && (mouse_y > ItemStorage.posy)&& (mouse_y < ItemStorage.posy+227) ) //plecak
			{
				ItemStorage.mouse(0, mouse_x - ItemStorage.posx, mouse_y - ItemStorage.posy);
			}
			if( (mouse_x > Map.posx) && (mouse_x < Map.posx+544) && (mouse_y > Map.posy)&& (mouse_y < Map.posy+416) ) //mapa
			{
				Map.mouse(0, mouse_x - Map.posx, mouse_y - Map.posy);
			}
			if( (mouse_x > Hotkeys.posx) && (mouse_x < Hotkeys.posx+437) && (mouse_y > Hotkeys.posy)&& (mouse_y < Hotkeys.posy+40) ) //hotkeye
			{
				Hotkeys.mouse(0, mouse_x - Hotkeys.posx, mouse_y - Hotkeys.posy);
			}
		}

		if( myPlayerTarget && lastMeleeAttack + 1500 < clock() )
		{
			this->sendAttackCreature(myPlayerTarget->getId(), false);
			lastMeleeAttack = clock();
		}

		while(System.sClient.receive(toReceive) == sf::Socket::Done)
		{
			toReceive >> recvid;
			if(recvid == 0)	
			{
			   receivingThreadRunning = false;
			   System.App->close();
			}
			if(recvid == 80)
				receiveDeEquipItem(toReceive);
			if(recvid == 81)
				receiveEquipItem(toReceive);
			if(recvid == 98)
				receivePlayerUpdateMana(toReceive);
			if(recvid == 99)
				receivePlayerStats(toReceive);
			if(recvid == 100)	
			   toReceive >> myPlayerID >> myPlayerX >> myPlayerY >> myPlayerZ;
			if(recvid == 101)	
				receiveTile(toReceive);
			if(recvid == 102)	
				receiveAddThingToTile(toReceive);
			if(recvid == 103)	
				receiveRemoveThingFromTile(toReceive);
			if(recvid == 105)	
				receiveTransformThingFromTile(toReceive);
			if(recvid == 106)
				receiveAddItemToStorage(toReceive);
			if(recvid == 107)
				receiveRemoveItemFromStorage(toReceive);
			if(recvid == 108)
				receiveRemoveItemFromAnyStorage(toReceive);
			if(recvid == 109)
				receiveRemoveTopItemFromTile(toReceive);
			if(recvid == 111)	
				receiveCreatureAppearOrMove(toReceive);
			if(recvid == 113)	
				receiveCreatureDisappear(toReceive);
			if(recvid == 140)	
				receiveCreatureUpdateHealth(toReceive);
			if(recvid == 141)	
				receiveCreatureUpdateLightLevel(toReceive);
			if(recvid == 142)
				receiveCreatureUpdateDirection(toReceive);
			if(recvid == 143)
				receiveCreatureUpdateSpeed(toReceive);
			if(recvid == 175)	
				receiveItemInfo(toReceive);
			if(recvid == 190)	
				receiveAddItemToContainerOnMap(toReceive);
			if(recvid == 191)	
				receiveRemoveItemFromContainerOnMap(toReceive);
			if(recvid == 200)
				Chat.receiveMessage(toReceive);
			if(recvid == 220)
				receiveTargetConfirmation(toReceive);
			if(recvid == 230)
				receiveEffect(toReceive);
			if(recvid == 231)
				receiveTextEffect(toReceive);
			if(recvid == 232)
				receiveShootEffect(toReceive);
			if(recvid == 250)
				receiveCancelSpell(toReceive);
			if(recvid == 600)
				receiveNPCResponse(toReceive);
			if(recvid == 601)
				receiveNPCshopSells(toReceive);
			if(recvid == 602)
				receiveNPCshopBuys(toReceive);
			recvid = -1;

			toReceive.clear();
		}
	

		System.App->clear();
		System.renderedMapTexture->clear();
		Map.render();
		if(this->myPlayer && this->myPlayer->isWalking())
		{
			float move = (float(clock()) - float(this->myPlayer->getStartedWalking()))/float(this->myPlayer->getSpeed())*32.0f;

			if(myPlayer->getDirection() == 2)
				renderedMap.setPosition(0.0f, -move+32.0f);
			if(myPlayer->getDirection() == 8)
				renderedMap.setPosition(0.0f, move-32.0f);
			if(myPlayer->getDirection() == 6)
			    renderedMap.setPosition(-move+32.0f, 0.0f);
			if(myPlayer->getDirection() == 4)
			    renderedMap.setPosition(move-32.0f, 0.0f);
			// DIAGONAL MOVEMENT
			if(myPlayer->getDirection() == 7)
			    renderedMap.setPosition(move-32.0f, move-32.0f);
			if(myPlayer->getDirection() == 9)
			    renderedMap.setPosition(-move+32.0f, move-32.0f);
			if(myPlayer->getDirection() == 1)
			    renderedMap.setPosition(move-32.0f, -move+32.0f);
			if(myPlayer->getDirection() == 3)
			    renderedMap.setPosition(-move+32.0f, -move+32.0f);
		}
	    else
			renderedMap.setPosition(0.0f,0.0f);

		System.renderedMapTexture->display();
		System.App->draw(renderedMap);
		System.App->draw(rectBottom);
		System.App->draw(rectMiddle);
		System.App->draw(rectLeft);
		System.App->draw(rectRight);

		if(this->containerWindow)
			containerWindow->render();
		if(EquWindow.show)
			EquWindow.render();

		ItemStorage.render();
		Chat.render();
		Hotkeys.render();
		NPCWindow.render();
		StatisticsWindow.render();

		System.App->display();

		if(this->closeContainerWindow)
		{
			delete containerWindow;
			this->containerWindow = NULL;
			this->closeContainerWindow = false;
		}

		Chat.proceedSendMessage();

		this->getFrameRate();
	}
	toSend.clear();
	id = 0;
	toSend << id;
	System.sClient.send(toSend);
}

void Game::renderShadow()
{
	this->lightMapTexture->display();
	System.renderedMapTexture->draw(*this->lightMap, sf::BlendAlpha);
}

void Game::doReceivingThread(void* UserData)
{
  ////////////////////////////////////////////////////////////////////////////////////////////////////
 // I'm don't using this receiving thread anymore cos this is making game buggy and random crashes///
////////////////////////////////////////////////////////////////////////////////////////////////////

/*	Game *game = static_cast<Game*>(UserData);
	
	sf::Packet toReceive;
	int recvid = -1;

	while(game->receivingThreadRunning)
	{
		if(System.sClient.receive(toReceive) == sf::Socket::Done)
		{
			toReceive >> recvid;
			if(recvid == 0)	
			{
			   game->receivingThreadRunning = false;
			   System.App->close();
			}
			if(recvid == 80)
			{
				game->receiveDeEquipItem(toReceive);
			}
			if(recvid == 81)
			{
				game->receiveEquipItem(toReceive);
			}
			if(recvid == 99)
			{
				game->receivePlayerStats(toReceive);
			}
			if(recvid == 100)	
			{
			   toReceive >> game->myPlayerID >> game->myPlayerX >> game->myPlayerY >> game->myPlayerZ;
			}
			if(recvid == 101)	
			{
				game->receiveTile(toReceive);
			}
			if(recvid == 102)	
			{
				game->receiveAddThingToTile(toReceive);
				//cout << "Received addThingToTile!" << endl;
			}
			if(recvid == 103)	
			{
				game->receiveRemoveThingFromTile(toReceive);
				//cout << "Received removeThingFromTile!" << endl;
			}
			if(recvid == 105)	
			{
				game->receiveTransformThingFromTile(toReceive);
				//cout << "Received transformThingFromTile!" << endl;
			}
			if(recvid == 106)
			{
				game->receiveAddItemToStorage(toReceive);
			}
			if(recvid == 107)
			{
				game->receiveRemoveItemFromStorage(toReceive);
			}
			if(recvid == 108)
			{
				game->receiveRemoveItemFromAnyStorage(toReceive);
			}
			if(recvid == 109)
			{
				game->receiveRemoveTopItemFromTile(toReceive);
			}
			if(recvid == 111)	
			{
				game->receiveCreatureAppearOrMove(toReceive);
				//cout << "Received creatureAppearOrMove!" << endl;
			}
			if(recvid == 112)	
			{
				game->receiveCreatureUpdateHealth(toReceive);
			}
			if(recvid == 113)	
			{
				game->receiveCreatureDisappear(toReceive);
			}
			if(recvid == 111)	
			{
				game->receiveCreatureUpdateLightLevel(toReceive);
			}
			if(recvid == 190)	
			{
				game->receiveAddItemToContainerOnMap(toReceive);
			}
			if(recvid == 191)	
			{
				game->receiveRemoveItemFromContainerOnMap(toReceive);
			}
			if(recvid == 200)
			{
				//cout << "Received Chat Message!" << endl;
				//Chat.addMessage(toReceive);
			}
			if(recvid == 220)
			{
				game->receiveTargetConfirmation(toReceive);
			}
			if(recvid == 230)
			{
				game->receiveEffect(toReceive);
			}
			recvid = -1;
			//Sleep(10);
		}
	}
*/
}