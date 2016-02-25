#ifndef CEFFECT_H
#define CEFFECT_H

#include <time.h>
#include <string>
#include <SFML/Graphics.hpp>
#include "thing.h"

using namespace std;

class Effect : public Thing
{
public:
 Effect(unsigned char _type);
 unsigned int getSpriteNumber();
 sf::Sprite *light;
private:
 unsigned char type; 
 clock_t started;
};

class TextEffect : public Thing
{
public:
 TextEffect(unsigned char _color, string _text);
 int isToDelete();
 void update();
 int modiferX, modiferY;
 sf::Text *sfText;
private:
 unsigned char color; 
 string text;
 clock_t started;
};

#endif