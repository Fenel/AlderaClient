#ifndef __TILE_H__
#define __TILE_H__

#include <deque>
#include <list>
#include "item.h"
#include "creature.h"
#include "enums.h"
#include "effect.h"

using namespace std;
typedef std::deque<Item*> ItemVector;
typedef std::list<Creature*> CreatureList;
typedef std::deque<Effect*> EffectVector;
typedef std::deque<TextEffect*> TextEffectVector;

class Tile : public Thing
{
public:
	Tile(Position _tilePos){pos.x = _tilePos.x; pos.y = _tilePos.y; pos.z = _tilePos.z; ground = NULL;};
	Tile(int Px, int Py, unsigned char Pz) {pos.x = Px; pos.y = Py, pos.z = Pz; ground = NULL;};
	Tile() { ground = NULL;};

    void addThing(Thing *thing);
	void createEffect(unsigned char type);
	void createTextEffect(unsigned char color, string text);
    void transformThing(unsigned char stackpos, Thing *thing);
    bool removeThing(unsigned char stackpos);
	bool removeTopItem(unsigned short count = 1);
    bool removeCreatureByID(unsigned int id);    
    Thing* getThing(unsigned char stackpos);
	Item* getTopItem();
    unsigned char getIndexOfThing(Thing* thing);
	void render(int startWidth, int startHeight, int camera_x, int camera_y, unsigned char step);

	bool isCollision();
    
    CreatureList& getCreatures();

	Position pos;
	int offsetx, offsety;
	bool hasHeight;
private:
	Item* ground;
	ItemVector topItems;
	ItemVector downItems;
	CreatureList creatures;
	EffectVector effects;
	TextEffectVector textEffects;
};


#endif
