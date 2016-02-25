#ifndef __CREATURESMANAGER_H__
#define __CREATURESMANAGER_H__

#include <string>
#include <deque>

#include "creature.h"

using namespace std;

class CreaturesManager
{
public:
	CreaturesManager();
	~CreaturesManager()
    {
		creaturesList.empty();
    }
	Creature* getCreature(unsigned int _id);

	bool createCreature(unsigned int posx, unsigned int posy, unsigned char posz, unsigned int pid, string name, unsigned char looktype, unsigned int speed);
	bool eraseCreature(unsigned int pid);
private:
	deque<Creature> creaturesList;
};




#endif