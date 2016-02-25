#include "item.h"
#include "baseItems.h"
#include "system.h"

extern BaseItems BaseItems;
extern System System;

Item::Item(unsigned int id, unsigned short _count)
{
    mID = id;
    baseItem = BaseItems.getItemType(id);
    count = _count;

	this->light = new sf::Sprite();
	this->light->setTexture(*System.lightObjectTexture);
	this->light->setColor(sf::Color(0, 0, 0, 255));
	this->light->setScale(float(BaseItems.getItemType(id)->lightLevel), float(BaseItems.getItemType(id)->lightLevel));
	this->light->setPosition(0.0f, 0.0f);
}

Item::~Item()
{

}

void Item::initialize(unsigned int id, unsigned short _count)
{
    mID = id;
    baseItem = BaseItems.getItemType(id);
    count = _count;
}

void Item::setItemCountOrSubType(unsigned short n)
{
    {
        count = n;
        if(count == 1)
            subType = 0;
        else if (count == 2)
            subType = 1;
        else if (count == 3)
            subType = 2;
        else if (count == 4)
            subType = 3;
        else if (count > 4 && count < 10)
            subType = 4;
        else if (count > 10 && count < 25)
            subType = 5;
        else if (count > 24 && count < 50)
            subType = 6;
        else
            subType = 7;
    }
}

bool Item::isGroundTile()
{
    return (baseItem && baseItem->group == ITEM_GROUP_GROUND);
}

bool Item::isSplash()
{
    return (baseItem && baseItem->group == ITEM_GROUP_SPLASH);
}

bool Item::hasHeight()
{
    return (baseItem && baseItem->hasHeight);
}

bool Item::isStackable()
{
    return (baseItem && baseItem->stackable);
}

bool Item::isRune()
{
    return (baseItem && baseItem->group == ITEM_GROUP_RUNE);
}

bool Item::isFluidContainer()
{
    return (baseItem && baseItem->group == ITEM_GROUP_FLUID);
}

bool Item::isAlwaysOnTop()
{
    return (baseItem && baseItem->alwaysOnTop);
}

bool Item::isCollision()
{
     return (baseItem && baseItem->blockSolid);
}