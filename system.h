#ifndef CSYSTEM_H
#define CSYSTEM_H

#include <SFML/Network.hpp>
#include <SFML/Graphics.hpp>
#include <time.h>
#include <deque>
#include "effect.h"
#include "enums.h"

class Item;
class Creature;
//class Effect;

class System
{
public:  
  bool connect();
  bool disconnect();

  System(unsigned int _width, unsigned int _height, unsigned int _res);
  ~System();
  void renderItem(int x, int y, Item *item, bool mapTexture = false);
  void renderCreature(int x, int y, Creature *creature, bool mapTexture = false);
  void renderCreatureName(int x, int y, Creature *creature, bool mapTexture = false);
  void renderEffect(int x, int y, Effect *effect, bool mapTexture = false);
  void renderTextEffect(int x, int y, TextEffect *textEffect, bool mapTexture);
  void renderShootEffects(unsigned char posz);
  void renderCreatureLight(int x, int y, Creature *creature);
  unsigned int gameState;
  unsigned int windowWidth, windowHeight, res;
  unsigned int activeTextbox;
  unsigned int globalTextboxesCount;
  bool keyProcessed;
  unsigned int unicode;

  bool hold;
  bool welcome;
  bool connected;
  int modiferX, modiferY;
  clock_t animationCounter;
  unsigned char getAnimationFrame();

  int mouseDelta;

  sf::Font font;
  sf::TcpSocket sClient;
  std::string clientVersion;
  std::string ipString;

  sf::RectangleShape targetRect;
  sf::Sprite *targetSprite;

  sf::RenderWindow *App;
  sf::RenderTexture *renderedMapTexture;

  sf::Texture *lightObjectTexture;

  std::deque<ShootEffect*> shootEffects;

  sf::Image *icon;
private:
};



#endif
