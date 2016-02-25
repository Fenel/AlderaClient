#include "thing.h"
#include "creature.h"
#include "system.h"

extern System System;

Creature::Creature()
{
	id = currentId;
	currentId++;
	direction = 2;
	startedWalking = 0;
	healthPercent = 100;
	active = true;

	this->light = new sf::Sprite();
	this->light->setTexture(*System.lightObjectTexture);
	this->light->setColor(sf::Color(0, 0, 0, 255));
	this->light->setScale(7.0f, 7.0f);
	this->light->setPosition(0.0f, 0.0f);
}

unsigned int Creature::currentId = 1;

/*
Creature::~Creature()
{
      id = 0;
}
*/

void Creature::setName(string _name)
{ 
	 name = _name; 
	 formattedName.setFont(System.font);
	 formattedName.setString(name);
	 formattedName.setCharacterSize(11);
	 //formattedName.setColor(sf::Color::White);
	 formattedName.setColor(sf::Color(0,200,0));
	 formattedName.setPosition(0.0f, 0.0f);

	 healthBarBackground.setFillColor(sf::Color(0,0,0));
	 healthBarBackground.setSize(sf::Vector2f(30.0f, 5.0f));
	 healthBarBackground.setPosition(0.0f, 0.0f);
	 
	 healthBarHealth.setFillColor(sf::Color(0,200,0));
	 healthBarHealth.setSize(sf::Vector2f(28.0f, 3.0f));
	 healthBarHealth.setPosition(1.0f, 1.0f);
	 //sf::FloatRect rect = formattedName.getLocalBounds();!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
	 //formattedName.setOrigin(int(rect.getSize().x/2), 0);!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
}

void Creature::setPos(unsigned int _posX, unsigned int _posY, unsigned char _posZ)
{
	posX = _posX;
	posY = _posY;
	posZ = _posZ;
}

void Creature::setStartedWalking()
{
    startedWalking = clock();
}

bool Creature::isWalking()
{
	if(startedWalking + speed <= clock())
		return false;
	else
		return true;
}
 
void Creature::addHealth(int _health)
{
	if(health + _health > 0)
		health += _health;
	if(health + _health <= 0)
		health = 0;
	if(health + _health > maxHealth)
		health = maxHealth;
}


void Creature::addPosX(int _posX)
{
	posX += _posX;
}

void Creature::addPosY(int _posY)
{
	posY += _posY;
}

void Creature::addPosZ(int _posZ)
{
	posZ += _posZ;
}

void Creature::updateHealthBar(unsigned char _healthPercent)
{
	this->healthPercent = _healthPercent;
	float percent = (28.0f*float(this->healthPercent))/100.0f;
	healthBarHealth.setSize(sf::Vector2f(percent, 3.0f));
}