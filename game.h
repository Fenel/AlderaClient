#ifndef __GAME__H
#define __GAME__H

#include <SFML/Network.hpp>
#include <deque>
#include "tile.h"

class Thing;
class Player;
class ContainerWindow;

extern int mouse_x, mouse_y, mouse_b;

class Game
{
public:
  Game();
  ~Game(){	};
  void loadLooktypes();
  unsigned int getFrameRate();
  void updateMapRects();
  void receiveDeEquipItem(sf::Packet received); //80
  void receiveEquipItem(sf::Packet received); //81
  void receivePlayerUpdateMana(sf::Packet received); //98
  void receivePlayerStats(sf::Packet received); //99
  void receiveTile(sf::Packet received);
  void receiveItemInfo(sf::Packet received); //175
  void receiveAddItemToContainerOnMap(sf::Packet received);
  void receiveRemoveItemFromContainerOnMap(sf::Packet received);
  void receiveAddThingToTile(sf::Packet received);
  bool receiveRemoveThingFromTile(sf::Packet received);
  bool receiveRemoveTopItemFromTile(sf::Packet received);
  bool receiveTransformThingFromTile(sf::Packet received);
  void receiveAddItemToStorage(sf::Packet toReceive);
  void receiveRemoveItemFromStorage(sf::Packet toReceive);
  void receiveRemoveItemFromAnyStorage(sf::Packet toReceive);
  void receiveCreatureAppearOrMove(sf::Packet received);

  void receiveCreatureUpdateHealth(sf::Packet toReceive);
  void receiveCreatureUpdateLightLevel(sf::Packet toReceive);
  void receiveCreatureUpdateDirection(sf::Packet received);
  void receiveCreatureUpdateSpeed(sf::Packet received);

  void receiveCreatureDisappear(sf::Packet received);
  void receiveTargetConfirmation(sf::Packet toReceive);
  void receiveEffect(sf::Packet toReceive);
  void receiveTextEffect(sf::Packet toReceive);
  void receiveShootEffect(sf::Packet toReceive);
  void receiveCancelSpell(sf::Packet toReceive);
  void receiveNPCResponse(sf::Packet toReceive);
  void receiveNPCshopSells(sf::Packet toReceive);
  void receiveNPCshopBuys(sf::Packet toReceive);

  void sendThrowItem(Item *item, unsigned int tox, unsigned int toy, unsigned char toz);
  void sendEquipItem(Item *item, unsigned char index, unsigned int bpid);
  void sendUseItem(unsigned int bpid, unsigned int fromx, unsigned int fromy, unsigned char fromz, unsigned int tox, unsigned int toy, unsigned char toz);
  void sendAttackCreature(unsigned int cid, bool cancel);
  void sendMessage(unsigned char type, string message, string to = "");
  void requestItemStats(unsigned int bpid, unsigned int fromx, unsigned int fromy, unsigned char fromz, unsigned char index = 0);
  void sendAddStatPoint(unsigned char index);
  void sendNPCResponse(unsigned char choice);
  void sendNPCbuy(unsigned int choice);
  void sendNPCsell(unsigned int choice);
  Thing* getThingFromTile(unsigned char stackpos, Tile *tile);
  void loop();  
  unsigned int myPlayerX;
  unsigned int myPlayerY;
  unsigned char myPlayerZ;
  unsigned int myPlayerID;
  unsigned int myPlayerLevel;
  unsigned int myPlayerExperience;
  bool myPlayerSex;
  Vocation myPlayerVocation;
  unsigned int myPlayerSpeed;
  unsigned int myPlayerSkillPoints;
  unsigned int myPlayerStatisticsPoints;
  unsigned int myPlayerStrength;
  unsigned int myPlayerDexterity;
  unsigned int myPlayerIntelligence;
  unsigned int myPlayerMagicPower;
  unsigned int myPlayerHealth;
  unsigned int myPlayerMaxHealth;
  unsigned char myPlayerHealthPercent;
  unsigned int myPlayerMana;
  unsigned int myPlayerMaxMana;
  clock_t lastMeleeAttack;
  Creature *myPlayer;
  Creature *myPlayerTarget;
  Thing* mouseHolding;
  int mouseHoldingParentId;
  unsigned int mouseHoldingParentCounter;

  void renderShadow();

  sf::Thread* receivingThread;
  bool receivingThreadRunning;
  static void doReceivingThread(void* UserData);

  sf::Texture containerWindowTexture;
  ContainerWindow *containerWindow;
  bool closeContainerWindow;

  std::deque<Looktype>looktypes;

  sf::Text itemCharges;
  sf::RenderTexture *lightMapTexture;
  sf::Sprite *lightMap;
private:  
  bool canRenderMap;
  sf::RectangleShape rectBottom;
  sf::RectangleShape rectMiddle;
  sf::RectangleShape rectLeft;
  sf::RectangleShape rectRight;
  sf::Sprite renderedMap;
};


#endif

