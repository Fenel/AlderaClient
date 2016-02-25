#include <iostream>
#include <SFML/Graphics.hpp>

#include "system.h"
#include "item.h"
#include "baseItems.h"
#include "creature.h"
#include "effect.h"
#include "game.h"

extern BaseItems BaseItems;
extern Game Game;
extern std::string intToStr(int);

System::System(unsigned int _width, unsigned int _height, unsigned int _res)
{
	windowWidth = _width;
	windowHeight = _height;
	res = _res;
	App = new sf::RenderWindow;
	App->create(sf::VideoMode(windowWidth, windowHeight, 32), "Aldera", sf::Style::Close);
	icon = new sf::Image();
	if(icon->loadFromFile("data/icon.png"))
		App->setIcon(32, 32, icon->getPixelsPtr());

    gameState = 0;
    
    activeTextbox = 0;
    globalTextboxesCount = 0;    
    hold = false;
	keyProcessed = true;
	unicode = 0;

    font.loadFromFile("data/courier.ttf");
    
    connected = false;
    welcome = false;
    clientVersion = "0.3.4";

	modiferX = 0;
	modiferY = 0;
	animationCounter = clock();

	ipString = "127.0.0.1";
	//ipString = "80.245.186.146";
	//ipString = "192.168.0.12";

	targetRect.setSize(sf::Vector2f(float(32), float(32)));
	targetRect.setFillColor(sf::Color(255, 0, 0, 96));
	targetRect.setOutlineColor(sf::Color(255, 32, 32));
	targetRect.setOutlineThickness(2);

	this->lightObjectTexture = new sf::Texture();
	this->lightObjectTexture->setSmooth(true);
	this->lightObjectTexture->loadFromFile("data/blend.png");

	mouseDelta = 0;
}

System::~System()
{
	delete icon;
	delete lightObjectTexture;

	App->close();
	delete App;
}

bool System::connect()
{
	this->targetSprite = new sf::Sprite();
	this->targetSprite->setTexture(*BaseItems.getTexture(39));
	this->targetSprite->setColor(sf::Color(255,255,255,192));

	if(connected == false)
	{
		if(sClient.connect(sf::IpAddress(this->ipString), 7272) != sf::Socket::Done)
		{
			#ifdef _DEBUG
				std::cout << "Error while connecting to server!" << std::endl;
			#endif
			return false;
		}
		connected = true; 
    
		return true;
	} 
	return false;
}

bool System::disconnect()
{
    if(connected == false)
	    return false;

    sClient.disconnect();
    connected = false;
	return true;
}

void System::renderItem(int x, int y, Item *item, bool mapTexture)
{
	if(item && BaseItems.getItemType(item->mID) && item->mID > 1 && item->mID < 1000)
	{	
		unsigned int animid = 0;
		if(BaseItems.getItemType(item->mID)->animcount == 2)
			animid = getAnimationFrame();
		if(BaseItems.getItemType(item->mID)->animcount == 1)
			animid = getAnimationFrame()%2;
		if(BaseItems.getItemType(item->mID)->numsprites > 0)
		{
			if(item->count== 1)
				animid = 0;
			if(item->count == 2)
				animid = 1;
			if(item->count == 3)
				animid = 2;
			if(item->count == 4)
				animid = 3;
			if(item->count >= 5 && item->count < 50)
				animid = 4;
			if(item->count >= 10 && item->count < 25)
				animid = 5;
			if(item->count >= 25 && item->count < 100)
				animid = 6;
			if(item->count >= 100 && item->count <= 1000)
				animid = 7;
		}

		sf::Vector2u size = BaseItems.getTexture(BaseItems.getItemType(item->mID)->spriteID + animid)->getSize();

		if(size.x == 32 && size.y == 32)
			BaseItems.getSprite(BaseItems.getItemType(item->mID)->spriteID + animid)->setPosition(float(x),float(y));
		else if(size.x == 64 && size.y == 32)
			BaseItems.getSprite(BaseItems.getItemType(item->mID)->spriteID + animid)->setPosition(float(x-32),float(y));
		else if(size.x == 32 && size.y == 64)
			BaseItems.getSprite(BaseItems.getItemType(item->mID)->spriteID + animid)->setPosition(float(x),float(y-32));
		else if(size.x == 64 && size.y == 64)
			BaseItems.getSprite(BaseItems.getItemType(item->mID)->spriteID + animid)->setPosition(float(x-32),float(y-32));

		Game.itemCharges.setString(intToStr(item->count));
		Game.itemCharges.setPosition(x+10,y+20);

		if(!mapTexture)
		{
			App->draw(*BaseItems.getSprite(BaseItems.getItemType(item->mID)->spriteID + animid));

			if(item->count > 1)
			{
				Game.itemCharges.setColor(sf::Color::Black);
				Game.itemCharges.setPosition(x+7,y+23-3);
				App->draw(Game.itemCharges);
				Game.itemCharges.setPosition(x+7,y+21-3);
				App->draw(Game.itemCharges);
				Game.itemCharges.setPosition(x+8,y+22-3);
				App->draw(Game.itemCharges);
				Game.itemCharges.setPosition(x+6,y+22-3);
				App->draw(Game.itemCharges);

				Game.itemCharges.setColor(sf::Color::White);
				Game.itemCharges.setPosition(x+7,y+22-3);
				App->draw(Game.itemCharges);
			}
		}
		else
		{
			renderedMapTexture->draw(*BaseItems.getSprite(BaseItems.getItemType(item->mID)->spriteID + animid));

			if(item->count > 1)
			{
				Game.itemCharges.setColor(sf::Color::Black);
				Game.itemCharges.setPosition(x+7,y+23-3);
				renderedMapTexture->draw(Game.itemCharges);
				Game.itemCharges.setPosition(x+7,y+21-3);
				renderedMapTexture->draw(Game.itemCharges);
				Game.itemCharges.setPosition(x+8,y+22-3);
				renderedMapTexture->draw(Game.itemCharges);
				Game.itemCharges.setPosition(x+6,y+22-3);
				renderedMapTexture->draw(Game.itemCharges);

				Game.itemCharges.setColor(sf::Color::White);
				Game.itemCharges.setPosition(x+7,y+22-3);
				renderedMapTexture->draw(Game.itemCharges);
			}
		}
		if(item->baseItem->lightLevel > 0 && Game.myPlayerZ < 7) //!!!! tymczasowo z < 7, jak dodasz noc to zmieñ !!!!
		{
			sf::Vector2f scale = item->light->getScale();
			item->light->setPosition(float(x-(int(scale.x/2)*32)), float(y-(int(scale.y/2)*32)));
			Game.lightMapTexture->draw(*item->light, sf::BlendMultiply);
		}
	}
}

void System::renderCreature(int x, int y, Creature *creature, bool mapTexture)
{
	int finalX = 0, finalY = 0, finalLooktype = 0;

	if(creature->getStartedWalking() + creature->getSpeed() <= clock()) /*&& Game.myPlayer*/
	{
		modiferX = 0;
		modiferY = 0;
	}
	if(creature->getStartedWalking() + creature->getSpeed() >= clock())
	{
         int licznik_x = x;
         int licznik_y = y;

         if(creature->getDirection() == 6 && ( (creature->getStartedWalking() + creature->getSpeed()/7 <= clock() && creature->getStartedWalking() + creature->getSpeed()/3.5 >= clock()) 
                                                || (creature->getStartedWalking() + creature->getSpeed()/1.76 <= clock() && creature->getStartedWalking() + creature->getSpeed()/1.4 >= clock())
                                                || (creature->getStartedWalking() + creature->getSpeed()/1.16 <= clock() && creature->getStartedWalking() + creature->getSpeed() >= clock() )))
         {
			 finalX = licznik_x + ((clock() - creature->getStartedWalking())/(creature->getSpeed()/32))-32 - modiferX;
			 finalY = licznik_y - modiferY;
			 finalLooktype = Game.looktypes[creature->getLooktype()].east+1;
         }
         else if(creature->getDirection() == 6)
         {
			 finalX = licznik_x + ((clock() - creature->getStartedWalking())/(creature->getSpeed()/32))-32 - modiferX;
			 finalY = licznik_y - modiferY;
			 finalLooktype = Game.looktypes[creature->getLooktype()].east+2;
         }
         if(creature->getDirection() == 4 && ( (creature->getStartedWalking() + creature->getSpeed()/7 <= clock() && creature->getStartedWalking() + creature->getSpeed()/3.5 >= clock()) 
                                                      || (creature->getStartedWalking() + creature->getSpeed()/1.76 <= clock() && creature->getStartedWalking() + creature->getSpeed()/1.4 >= clock())
                                                      || (creature->getStartedWalking() + creature->getSpeed()/1.16 <= clock() && creature->getStartedWalking() + creature->getSpeed() >= clock() )))
         {
			 finalX = licznik_x- ((clock() - creature->getStartedWalking())/(creature->getSpeed()/32))+32 - modiferX;
			 finalY = licznik_y - modiferY;
			 finalLooktype = Game.looktypes[creature->getLooktype()].west+1;
         }
         else if(creature->getDirection() == 4)
         {
			 finalX = licznik_x- ((clock() - creature->getStartedWalking())/(creature->getSpeed()/32))+32 - modiferX;
			 finalY = licznik_y - modiferY;
			 finalLooktype = Game.looktypes[creature->getLooktype()].west+2;
         }
         if(creature->getDirection() == 8 && ( (creature->getStartedWalking() + creature->getSpeed()/7 <= clock() && creature->getStartedWalking() + creature->getSpeed()/3.5 >= clock()) 
                                                     || (creature->getStartedWalking() + creature->getSpeed()/1.76 <= clock() && creature->getStartedWalking() + creature->getSpeed()/1.4 >= clock())
                                                     || (creature->getStartedWalking() + creature->getSpeed()/1.16 <= clock() && creature->getStartedWalking() + creature->getSpeed() >= clock() )))
         {
			 finalX = licznik_x - modiferX;
			 finalY = licznik_y - ((clock() - creature->getStartedWalking())/(creature->getSpeed()/32))+32 - modiferY;
			 finalLooktype = Game.looktypes[creature->getLooktype()].north+1;
         }
         else if(creature->getDirection() == 8)
         {
			 finalX = licznik_x - modiferX;
			 finalY = licznik_y - ((clock() - creature->getStartedWalking())/(creature->getSpeed()/32))+32 - modiferY;
			 finalLooktype = Game.looktypes[creature->getLooktype()].north+2;
         }
         if(creature->getDirection() == 2 && ( (creature->getStartedWalking() + creature->getSpeed()/7 <= clock() && creature->getStartedWalking() + creature->getSpeed()/3.5 >= clock()) 
                                                       || (creature->getStartedWalking() + creature->getSpeed()/1.76 <= clock() && creature->getStartedWalking() + creature->getSpeed()/1.4 >= clock())
                                                       || (creature->getStartedWalking() + creature->getSpeed()/1.16 <= clock() && creature->getStartedWalking() + creature->getSpeed() >= clock() )))
         {
			 finalX = licznik_x - modiferX;
			 finalY = licznik_y + ((clock() - creature->getStartedWalking())/(creature->getSpeed()/32))-32 - modiferY;
			 finalLooktype = Game.looktypes[creature->getLooktype()].south+1;
         }
         else if(creature->getDirection() == 2) 
         {
			 finalX = licznik_x - modiferX;
			 finalY = licznik_y + ((clock() - creature->getStartedWalking())/(creature->getSpeed()/32))-32 - modiferY;
			 finalLooktype = Game.looktypes[creature->getLooktype()].south+2;
         }
		 // DIAGONAL MOVEMENTS
         if(creature->getDirection() == 7 && ( (creature->getStartedWalking() + creature->getSpeed()/7 <= clock() && creature->getStartedWalking() + creature->getSpeed()/3.5 >= clock()) 
                                                     || (creature->getStartedWalking() + creature->getSpeed()/1.76 <= clock() && creature->getStartedWalking() + creature->getSpeed()/1.4 >= clock())
                                                     || (creature->getStartedWalking() + creature->getSpeed()/1.16 <= clock() && creature->getStartedWalking() + creature->getSpeed() >= clock() )))
         {
			 finalX = licznik_x - ((clock() - creature->getStartedWalking())/(creature->getSpeed()/32))+32 - modiferX;
			 finalY = licznik_y - ((clock() - creature->getStartedWalking())/(creature->getSpeed()/32))+32 - modiferY;
			 finalLooktype = Game.looktypes[creature->getLooktype()].north+1;
         }
         else if(creature->getDirection() == 7)
         {
			 finalX = licznik_x - ((clock() - creature->getStartedWalking())/(creature->getSpeed()/32))+32 - modiferX;
			 finalY = licznik_y - ((clock() - creature->getStartedWalking())/(creature->getSpeed()/32))+32 - modiferY;
			 finalLooktype = Game.looktypes[creature->getLooktype()].north+2;
         }
         if(creature->getDirection() == 9 && ( (creature->getStartedWalking() + creature->getSpeed()/7 <= clock() && creature->getStartedWalking() + creature->getSpeed()/3.5 >= clock()) 
                                                     || (creature->getStartedWalking() + creature->getSpeed()/1.76 <= clock() && creature->getStartedWalking() + creature->getSpeed()/1.4 >= clock())
                                                     || (creature->getStartedWalking() + creature->getSpeed()/1.16 <= clock() && creature->getStartedWalking() + creature->getSpeed() >= clock() )))
         {
			 finalX = licznik_x + ((clock() - creature->getStartedWalking())/(creature->getSpeed()/32))-32 - modiferX;
			 finalY = licznik_y - ((clock() - creature->getStartedWalking())/(creature->getSpeed()/32))+32 - modiferY;
			 finalLooktype = Game.looktypes[creature->getLooktype()].north+1;
         }
         else if(creature->getDirection() == 9)
         {
			 finalX = licznik_x + ((clock() - creature->getStartedWalking())/(creature->getSpeed()/32))-32 - modiferX;
			 finalY = licznik_y - ((clock() - creature->getStartedWalking())/(creature->getSpeed()/32))+32 - modiferY;
			 finalLooktype = Game.looktypes[creature->getLooktype()].north+2;
         }
         if(creature->getDirection() == 1 && ( (creature->getStartedWalking() + creature->getSpeed()/7 <= clock() && creature->getStartedWalking() + creature->getSpeed()/3.5 >= clock()) 
                                                     || (creature->getStartedWalking() + creature->getSpeed()/1.76 <= clock() && creature->getStartedWalking() + creature->getSpeed()/1.4 >= clock())
                                                     || (creature->getStartedWalking() + creature->getSpeed()/1.16 <= clock() && creature->getStartedWalking() + creature->getSpeed() >= clock() )))
         {
			 finalX = licznik_x - ((clock() - creature->getStartedWalking())/(creature->getSpeed()/32))+32 - modiferX;
			 finalY = licznik_y + ((clock() - creature->getStartedWalking())/(creature->getSpeed()/32))-32 - modiferY;
			 finalLooktype = Game.looktypes[creature->getLooktype()].south+1;
         }
         else if(creature->getDirection() == 1)
         {
			 finalX = licznik_x - ((clock() - creature->getStartedWalking())/(creature->getSpeed()/32))+32 - modiferX;
			 finalY = licznik_y + ((clock() - creature->getStartedWalking())/(creature->getSpeed()/32))-32 - modiferY;
			 finalLooktype = Game.looktypes[creature->getLooktype()].south+2;
         }
         if(creature->getDirection() == 3 && ( (creature->getStartedWalking() + creature->getSpeed()/7 <= clock() && creature->getStartedWalking() + creature->getSpeed()/3.5 >= clock()) 
                                                     || (creature->getStartedWalking() + creature->getSpeed()/1.76 <= clock() && creature->getStartedWalking() + creature->getSpeed()/1.4 >= clock())
                                                     || (creature->getStartedWalking() + creature->getSpeed()/1.16 <= clock() && creature->getStartedWalking() + creature->getSpeed() >= clock() )))
         {
			 finalX = licznik_x + ((clock() - creature->getStartedWalking())/(creature->getSpeed()/32))-32 - modiferX;
			 finalY = licznik_y + ((clock() - creature->getStartedWalking())/(creature->getSpeed()/32))-32 - modiferY;
			 finalLooktype = Game.looktypes[creature->getLooktype()].south+1;
         }
         else if(creature->getDirection() == 3)
         {
			 finalX = licznik_x + ((clock() - creature->getStartedWalking())/(creature->getSpeed()/32))-32 - modiferX;
			 finalY = licznik_y + ((clock() - creature->getStartedWalking())/(creature->getSpeed()/32))-32 - modiferY;
			 finalLooktype = Game.looktypes[creature->getLooktype()].south+2;
         }

	}
	else
	{
		if(creature->getDirection() == 7)
			creature->setDirection(8);
		else if(creature->getDirection() == 9)
			creature->setDirection(8);
		else if(creature->getDirection() == 1)
			creature->setDirection(2);
		else if(creature->getDirection() == 3)
			creature->setDirection(2);

	    int dir_modifer = 0;

		if(creature->getDirection() == 8)
			dir_modifer = 0;
	    if(creature->getDirection() == 2)
			dir_modifer = 3;
	    if(creature->getDirection() == 4)
			dir_modifer = 6;
	    if(creature->getDirection() == 6)
			dir_modifer = 9;

		finalX = x - modiferX;
		finalY = y - modiferY;
		finalLooktype = int(finalLooktype = Game.looktypes[creature->getLooktype()].north + dir_modifer);
	}
	if(Game.myPlayerTarget && Game.myPlayerTarget->getId() == creature->getId())
	{
		targetSprite->setPosition(float(finalX), float(finalY));
		renderedMapTexture->draw(*targetSprite);
		//targetRect.setPosition(float(finalX), float(finalY));
		//renderedMapTexture->draw(targetRect);
	}

	sf::Vector2u size = BaseItems.getTexture(finalLooktype)->getSize();

	if(size.x == 32 && size.y == 32)
		BaseItems.getSprite(finalLooktype)->setPosition(float(finalX),float(finalY));
	else if(size.x == 64 && size.y == 32)
		BaseItems.getSprite(finalLooktype)->setPosition(float(finalX-32),float(finalY));
	else if(size.x == 32 && size.y == 64)
		BaseItems.getSprite(finalLooktype)->setPosition(float(finalX),float(finalY-32));
	else if(size.x == 64 && size.y == 64)
		BaseItems.getSprite(finalLooktype)->setPosition(float(finalX-32),float(finalY-32));

	if(!mapTexture)
		App->draw(*BaseItems.getSprite(finalLooktype));
	else
		renderedMapTexture->draw(*BaseItems.getSprite(finalLooktype));
}

void System::renderCreatureName(int x, int y, Creature *creature, bool mapTexture)
{
	int movex = 0;
	int movey = 0;

	if(creature->isWalking())
	{
		if(creature->getDirection() == 2)
			movey = int(float(clock() - creature->getStartedWalking())/float(creature->getSpeed())*32) - 32;
		if(creature->getDirection() == 8)
			movey = -(int(float(clock() - creature->getStartedWalking())/float(creature->getSpeed())*32)) + 32;
		if(creature->getDirection() == 6)
			movex = int(float(clock() - creature->getStartedWalking())/float(creature->getSpeed())*32) - 32;
		if(creature->getDirection() == 4)
			movex = -(int(float(clock() - creature->getStartedWalking())/float(creature->getSpeed())*32)) + 32;
		// DIAGONAL MOVEMENT
		if(creature->getDirection() == 7)
		{
			movex = -(int(float(clock() - creature->getStartedWalking())/float(creature->getSpeed())*32)) + 32;
			movey = -(int(float(clock() - creature->getStartedWalking())/float(creature->getSpeed())*32)) + 32;
		}
		if(creature->getDirection() == 9)
		{
			movex = int(float(clock() - creature->getStartedWalking())/float(creature->getSpeed())*32) - 32;
			movey = -(int(float(clock() - creature->getStartedWalking())/float(creature->getSpeed())*32)) + 32;
		}
		if(creature->getDirection() == 1)
		{
			movex = -(int(float(clock() - creature->getStartedWalking())/float(creature->getSpeed())*32)) + 32;
			movey = int(float(clock() - creature->getStartedWalking())/float(creature->getSpeed())*32) - 32;
		}
		if(creature->getDirection() == 3)
		{
			movex = int(float(clock() - creature->getStartedWalking())/float(creature->getSpeed())*32) - 32;
			movey = int(float(clock() - creature->getStartedWalking())/float(creature->getSpeed())*32) - 32;
		}
	}
	else
	{
		movex = 0;
		movey = 0;
	}
	

	creature->formattedName.setColor(sf::Color::Black);
	creature->formattedName.setPosition(float(movex+x+7),float(movey+y-14));
	renderedMapTexture->draw(creature->formattedName);
	creature->formattedName.setPosition(float(movex+x+9),float(movey+y-14));
	renderedMapTexture->draw(creature->formattedName);
	creature->formattedName.setPosition(float(movex+x+8),float(movey+y-15));
	renderedMapTexture->draw(creature->formattedName);
	creature->formattedName.setPosition(float(movex+x+8),float(movey+y-13));
	renderedMapTexture->draw(creature->formattedName);

	//creature->formattedName.setColor(sf::Color::White);
	creature->formattedName.setColor(sf::Color(0,200,0));
	creature->formattedName.setPosition(float(movex+x+8),float(movey+y-14));
	renderedMapTexture->draw(creature->formattedName);

	creature->healthBarBackground.setPosition(float(movex+x+11), float(movey+y-1));
	creature->healthBarHealth.setPosition(float(movex+x+12), float(movey+y));

	renderedMapTexture->draw(creature->healthBarBackground);
	renderedMapTexture->draw(creature->healthBarHealth);
}

void System::renderEffect(int x, int y, Effect *effect, bool mapTexture)
{
	int finalX = x, finalY = y;

	sf::Vector2u size = BaseItems.getTexture(effect->getSpriteNumber())->getSize();

	if(size.x == 32 && size.y == 32)
		BaseItems.getSprite(effect->getSpriteNumber())->setPosition(float(finalX),float(finalY));
	else if(size.x == 64 && size.y == 32)
		BaseItems.getSprite(effect->getSpriteNumber())->setPosition(float(finalX-32),float(finalY));
	else if(size.x == 32 && size.y == 64)
		BaseItems.getSprite(effect->getSpriteNumber())->setPosition(float(finalX),float(finalY-32));
	else if(size.x == 64 && size.y == 64)
		BaseItems.getSprite(effect->getSpriteNumber())->setPosition(float(finalX-32),float(finalY-32));
	else if(size.x == 96 && size.y == 96) //tylko dla efektu zdefiniowany taki rozmiar na ten moment, centruje go na kratce œrodkowej (inaczej ni¿ rysuj¹c itemy)
		BaseItems.getSprite(effect->getSpriteNumber())->setPosition(float(finalX-32),float(finalY-32));

	if(!mapTexture)
		App->draw(*BaseItems.getSprite(effect->getSpriteNumber()));
	else
		renderedMapTexture->draw(*BaseItems.getSprite(effect->getSpriteNumber()));

	sf::Vector2f scale = effect->light->getScale();
	if(size.x > 64 && size.y > 64)
		effect->light->setPosition(float(x+(int(scale.x/2)*32))-48, float(y+(int(scale.y/2)*32))-48);
	else
		effect->light->setPosition(float(x-(int(scale.x/2)*32)), float(y-(int(scale.y/2)*32)));

	Game.lightMapTexture->draw(*effect->light, sf::BlendMultiply);
}

void System::renderTextEffect(int x, int y, TextEffect *textEffect, bool mapTexture)
{
	textEffect->update();

	sf::Color color = textEffect->sfText->getColor();

	textEffect->sfText->setColor(sf::Color::Black);
	textEffect->sfText->setPosition(float(x + textEffect->modiferX+1), float(y + textEffect->modiferY));
	renderedMapTexture->draw(*(textEffect->sfText));
	textEffect->sfText->setPosition(float(x + textEffect->modiferX-1), float(y + textEffect->modiferY));
	renderedMapTexture->draw(*(textEffect->sfText));
	textEffect->sfText->setPosition(float(x + textEffect->modiferX), float(y + textEffect->modiferY+1));
	renderedMapTexture->draw(*(textEffect->sfText));
	textEffect->sfText->setPosition(float(x + textEffect->modiferX), float(y + textEffect->modiferY-1));
	renderedMapTexture->draw(*(textEffect->sfText));

	textEffect->sfText->setColor(color);
	textEffect->sfText->setPosition(float(x + textEffect->modiferX), float(y + textEffect->modiferY));
	renderedMapTexture->draw(*(textEffect->sfText));
}

void System::renderShootEffects(unsigned char posz)
{
	for(unsigned int i = 0; i < shootEffects.size(); i++)
	{
		if(shootEffects[i]->posz == posz)
		{
			if(shootEffects[i]->expires < clock())
			{
				//shootEffects.erase(i);
				continue;
			}
			float movex = float(int(shootEffects[i]->tox) - int(shootEffects[i]->fromx))*32.0f*( float(shootEffects[i]->expires - clock())/float(shootEffects[i]->range) );
			float movey = float(int(shootEffects[i]->toy) - int(shootEffects[i]->fromy))*32.0f*( float(shootEffects[i]->expires - clock())/float(shootEffects[i]->range) );
			float posx;
			float posy;
			if(shootEffects[i]->fromx < shootEffects[i]->tox && shootEffects[i]->fromy < shootEffects[i]->toy)
			{
				posx = -int(Game.myPlayerX-9)*32 + int(shootEffects[i]->tox)*32 - movex - 16.0f;
				posy = -int(Game.myPlayerY-7)*32 + int(shootEffects[i]->toy)*32 - movey - 16.0f;
			}
			if(shootEffects[i]->fromx <= shootEffects[i]->tox && shootEffects[i]->fromy >= shootEffects[i]->toy)
			{
				posx = -int(Game.myPlayerX-9)*32 + int(shootEffects[i]->tox)*32 - movex + 16.0f;
				posy = -int(Game.myPlayerY-7)*32 + int(shootEffects[i]->toy)*32 - movey + 16.0f;
			}
			if(shootEffects[i]->fromx >= shootEffects[i]->tox && shootEffects[i]->fromy < shootEffects[i]->toy)
			{
				posx = -int(Game.myPlayerX-9)*32 + int(shootEffects[i]->tox)*32 - movex + 16.0f;
				posy = -int(Game.myPlayerY-7)*32 + int(shootEffects[i]->toy)*32 - movey + 16.0f;
			}
			if(shootEffects[i]->fromx > shootEffects[i]->tox && shootEffects[i]->fromy >= shootEffects[i]->toy)
			{
				posx = -int(Game.myPlayerX-9)*32 + int(shootEffects[i]->tox)*32 - movex + 16.0f;
				posy = -int(Game.myPlayerY-7)*32 + int(shootEffects[i]->toy)*32 - movey + 16.0f;
			}
			shootEffects[i]->sprite.setPosition(posx, posy);
			
			renderedMapTexture->draw(*(&shootEffects[i]->sprite));
			/*if(posz < 7 && shootEffects[i]->hasLight == true)
			{
				shootEffects[i]->light.setPosition(posx, posy);
				Game.lightMapTexture->draw(shootEffects[i]->light, sf::BlendMultiply);
			}*/
		}
	}	
}

void System::renderCreatureLight(int x, int y, Creature *creature)
{
	int movex = 0;
	int movey = 0;

	if(creature->isWalking())
	{
		if(creature->getDirection() == 2)
			movey = int(float(clock() - creature->getStartedWalking())/float(creature->getSpeed())*32) - 32;
		if(creature->getDirection() == 8)
			movey = -(int(float(clock() - creature->getStartedWalking())/float(creature->getSpeed())*32)) + 32;
		if(creature->getDirection() == 6)
			movex = int(float(clock() - creature->getStartedWalking())/float(creature->getSpeed())*32) - 32;
		if(creature->getDirection() == 4)
			movex = -(int(float(clock() - creature->getStartedWalking())/float(creature->getSpeed())*32)) + 32;
		// DIAGONAL MOVEMENT
		if(creature->getDirection() == 7)
		{
			movex = -(int(float(clock() - creature->getStartedWalking())/float(creature->getSpeed())*32)) + 32;
			movey = -(int(float(clock() - creature->getStartedWalking())/float(creature->getSpeed())*32)) + 32;
		}
		if(creature->getDirection() == 9)
		{
			movex = int(float(clock() - creature->getStartedWalking())/float(creature->getSpeed())*32) - 32;
			movey = -(int(float(clock() - creature->getStartedWalking())/float(creature->getSpeed())*32)) + 32;
		}
		if(creature->getDirection() == 1)
		{
			movex = -(int(float(clock() - creature->getStartedWalking())/float(creature->getSpeed())*32)) + 32;
			movey = int(float(clock() - creature->getStartedWalking())/float(creature->getSpeed())*32) - 32;
		}
		if(creature->getDirection() == 3)
		{
			movex = int(float(clock() - creature->getStartedWalking())/float(creature->getSpeed())*32) - 32;
			movey = int(float(clock() - creature->getStartedWalking())/float(creature->getSpeed())*32) - 32;
		}
	}
	else
	{
		movex = 0;
		movey = 0;
	}
	sf::Vector2f scale = creature->light->getScale();
	creature->light->setPosition(float(x+movex-(int(scale.x/2)*32)), float(y+movey-(int(scale.y/2)*32)));
	Game.lightMapTexture->draw(*creature->light, sf::BlendMultiply);
}

unsigned char System::getAnimationFrame()
{
	if(this->animationCounter + 500 >= clock())
		return 0;
	if(this->animationCounter + 1000 >= clock())
	{
		return 1;
	}
	if(this->animationCounter + 1500 >= clock())
	{
		return 2;
	}
	if(this->animationCounter + 1500 < clock())
	{
		this->animationCounter = clock();
		return 0;
	}
	return 0;
}

 

 

 

 

 

 

 

 

 

