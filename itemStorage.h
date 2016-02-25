#ifndef __ITEMSTORAGE_H__
#define __ITEMSTORAGE_H__

#include "container.h"

class ItemStorage : public Container
{
public:
	ItemStorage(unsigned int _type);
	virtual ~ItemStorage();
	void render();
	void mouse(unsigned char type, int mouse_x, int mouse_y);//lokalne x i y myszki
	int posx, posy;
private:

protected:
	unsigned int totalWeight;

	sf::Texture storageItemsT;
	sf::Sprite storageItems;
};

#endif