#include <SFML/Graphics.hpp>

#include "NPCWindow.h"
#include "system.h"
#include "game.h"
#include "creaturesManager.h"
#include "baseItems.h"

extern System System;
extern Game Game;
extern CreaturesManager CreaturesManager;
extern BaseItems BaseItems;
extern std::string intToStr(int n);

NPCWindow::NPCWindow(int _posx, int _posy)
{
	this->posx = _posx;
	this->posy = _posy;

	this->npcWindowTexture.loadFromFile("data/npcWindow.bmp");
	this->npcWindowSprite.setTexture(this->npcWindowTexture);
	this->npcShopWindowTexture.loadFromFile("data/npcShopWindow.bmp");
	this->npcShopWindowSprite.setTexture(this->npcShopWindowTexture);

	this->setPos(posx, posy);

	show = false;
	transparent = false;
	shop = false;
	currentSell = 0; //do obs³ugi przewijania produktów w sklepie
	currentBuy = 0;

	npcName.setFont(System.font);
	npcName.setCharacterSize(12);
	npcName.setColor(sf::Color(0,200,0));
	//npcName.setStyle(sf::Text::Bold);
	npcName.setPosition(posx+4, posy+83);

	playerName.setFont(System.font);
	playerName.setCharacterSize(12);
	playerName.setColor(sf::Color(0,200,0));
	//playerName.setStyle(sf::Text::Bold);
	playerName.setPosition(posx+4, posy+172);
}

void NPCWindow::render()
{
	if(show)
	{
		if(shop == false)
			System.App->draw(this->npcWindowSprite);
		else
			System.App->draw(this->npcShopWindowSprite);

		System.App->draw(this->playerName);
		System.App->draw(this->npcName);
		System.App->draw(this->npcSprite);
		System.App->draw(this->playerSprite);

		if(shop == false)
		{
			for(unsigned int i = 0; i < said.size(); i++)
				System.App->draw(*this->said.at(i));
			for(unsigned int i = 0; i < choices.size(); i++)
				System.App->draw(*this->choices.at(i));
		}
		else
		{
			for(unsigned int i = 0; i < 12; i++)
			{
				if(i + currentBuy >= buys.size())
					break;

				this->buys.at(i+currentBuy).sfName.setPosition(posx + 76.0, float(posy + 28 + i*13));
				System.App->draw(this->buys.at(i+currentBuy).sfName);
				this->buys.at(i+currentBuy).sfPrice.setPosition(posx + 226.0, float(posy + 28 + i*13));
				System.App->draw(this->buys.at(i+currentBuy).sfPrice);			
			}
			for(unsigned int i = 0; i < 12; i++)
			{
				if(i + currentSell >= sells.size())
					break;

				this->sells.at(i+currentSell).sfName.setPosition(posx + 311.0, float(posy + 28 + i*13));
				System.App->draw(this->sells.at(i+currentSell).sfName);
				this->sells.at(i+currentSell).sfPrice.setPosition(posx + 460.0, float(posy + 28 + i*13));
				System.App->draw(this->sells.at(i+currentSell).sfPrice);			
			}	
		}
	}
}

bool NPCWindow::mouse(unsigned char type, int mouse_x, int mouse_y) //lokalne x i y myszki.
{
	if(type == 137) //ruch rolk¹ myszki
	{
		if(mouse_x >= 73 && mouse_x <= 307 && mouse_y >= 15 && mouse_y <= 190)
		{
			currentBuy -= System.mouseDelta;
			if(currentBuy > int(buys.size()) - 12)
				currentBuy = buys.size() - 12;
			if(currentBuy < 0)
				currentBuy = 0;

			return true;
		}
		if(mouse_x >= 308 && mouse_x <= 542 && mouse_y >= 15 && mouse_y <= 190)
		{
			currentSell -= System.mouseDelta;
			if(currentSell > int(sells.size()) - 12)
				currentSell = sells.size() - 12;
			if(currentSell < 0)
				currentSell = 0;
			return true;
		}
	}
	if(type == 4) //obs³uga transparentnoœci i wy³¹czania okna
	{
		if(mouse_x >= 531 && mouse_x <= 543 && mouse_y >= 1 && mouse_y <= 12)
		{ 
			this->show = false;
			this->shop = false;
			buys.clear();
			sells.clear();
			return true;
		}
		if(mouse_x >= 507 && mouse_x <= 519 && mouse_y >= 1 && mouse_y <= 12 && this->shop == false)
		{ 
			this->shop = true;
			return true;
		}
		if(mouse_x >= 519 && mouse_x <= 531 && mouse_y >= 1 && mouse_y <= 12)
		{ 
			this->transparent = !this->transparent;
			sf::Color *color;
			if(transparent)
				color = new sf::Color(255, 255, 255, 216);
			else
				color = new sf::Color(255, 255, 255, 255);

			this->npcWindowSprite.setColor(*color);
			this->npcShopWindowSprite.setColor(*color);
			this->playerName.setColor(*color);
			this->npcName.setColor(*color);
			this->npcSprite.setColor(*color);
			this->playerSprite.setColor(*color);

			for(unsigned int i = 0; i < said.size(); i++)
				this->said.at(i)->setColor(*color);
			for(unsigned int i = 0; i < choices.size(); i++)
				this->choices.at(i)->setColor(*color);

			return true;
		}
	}
	if(shop == true)
	{
		if(type == 4)
		{
			if(mouse_x >= 264 && mouse_x <= 292)
			{
				for(int i = 0; i < 12; i++)
				{
					if(mouse_y >= (31+i*13) && mouse_y <= (41+i*13))
					{
						if(i + currentBuy < buys.size())
							Game.sendNPCbuy(buys.at(i+currentBuy).id);
						return true;
					}
				}
			}
			if(mouse_x >= 296 && mouse_x <= 306)
			{
				if(mouse_y >= 14 && mouse_y <= 25)
				{
					if(currentBuy > 0)
						currentBuy--;
					return true;
				}
				if(mouse_y >= 178 && mouse_y <= 190)
				{
					if(currentBuy < buys.size() - 12)
						currentBuy++;
					return true;
				}
			}
			if(mouse_x >= 499 && mouse_x <= 527)
			{
				for(int i = 0; i < 11; i++)
				{
					if(mouse_y >= (31+i*13) && mouse_y <= (41+i*13))
					{
						if(i + currentSell < sells.size())
							Game.sendNPCsell(sells.at(i+currentSell).id);
						return true;
					}
				}
			}
			if(mouse_x >= 531 && mouse_x <= 541)
			{
				if(mouse_y >= 14 && mouse_y <= 25)
				{
					if(currentSell > 0)
						currentSell--;
					return true;
				}
				if(mouse_y >= 178 && mouse_y <= 190)
				{
					if(currentSell < sells.size() - 12)
						currentSell++;
					return true;
				}
			}
		}
		return false;
	}
	if(type == 0 && this->show) //zatrzymanie myszki nad napisem
	{
		for(unsigned int i = 0; i < choices.size(); i++)
			this->choices.at(i)->setStyle(sf::Text::Style::Regular);

		if(mouse_x >= 73 && mouse_x <= 529)
		{ 
			if(mouse_y >= 107 && mouse_y < 122 && choices.size() >= 1)
			{
				this->choices.at(0)->setStyle(sf::Text::Style::Bold);
				return true;
			}
			if(mouse_y >= 122 && mouse_y < 137 && choices.size() >= 2)
			{
				this->choices.at(1)->setStyle(sf::Text::Style::Bold);
				return true;
			}
			if(mouse_y >= 137 && mouse_y < 152 && choices.size() >= 3)
			{
				this->choices.at(2)->setStyle(sf::Text::Style::Bold);
				return true;
			}
			if(mouse_y >= 152 && mouse_y < 167 && choices.size() >= 4)
			{
				this->choices.at(3)->setStyle(sf::Text::Style::Bold);
				return true;
			}
			if(mouse_y >= 167 && mouse_y < 182 && choices.size() >= 5)
			{
				this->choices.at(4)->setStyle(sf::Text::Style::Bold);
				return true;
			}
		}
	}
	if(type == 4) //puszczenie przycisku myszki na obszarze okna
	{
		if(mouse_x >= 73 && mouse_x <= 529)
		{ 
			if(mouse_y >= 107 && mouse_y < 122 && choices.size() >= 1)
			{
				Game.sendNPCResponse(1);
				return true;
			}
			if(mouse_y >= 122 && mouse_y < 137 && choices.size() >= 2)
			{
				Game.sendNPCResponse(2);
				return true;
			}
			if(mouse_y >= 137 && mouse_y < 152 && choices.size() >= 3)
			{
				Game.sendNPCResponse(3);
				return true;
			}
			if(mouse_y >= 152 && mouse_y < 167 && choices.size() >= 4)
			{
				Game.sendNPCResponse(4);
				return true;
			}
			if(mouse_y >= 167 && mouse_y < 182 && choices.size() >= 5)
			{
				Game.sendNPCResponse(5);
				return true;
			}
		}
	}
	return false;
}

void NPCWindow::setPos(int x, int y) 
{ 
	this->posx = x;
	this->posy = y;
	this->npcWindowSprite.setPosition(posx, posy);
	this->npcShopWindowSprite.setPosition(posx, posy);
};

void NPCWindow::update()
{
	npcName.setPosition(posx+4, posy+83);

	playerName.setPosition(posx+4, posy+172);

}

void NPCWindow::newConversation(string npcName, unsigned char npcLooktype)
{
	if(CreaturesManager.getCreature(Game.myPlayerID))
		playerName.setString(CreaturesManager.getCreature(Game.myPlayerID)->getName());

	this->npcName.setString(npcName);
	if(Game.looktypes.size() >= npcLooktype)
		npcSprite.setTexture(*BaseItems.getTexture(Game.looktypes.at(npcLooktype).south));
	else
		npcSprite.setTexture(*BaseItems.getTexture(694));

	sf::Vector2u size = npcSprite.getTexture()->getSize();	
	if(size.x == 32 && size.y == 32)
		npcSprite.setPosition(posx+36, posy+48);
	else if(size.x == 64 && size.y == 64)
		npcSprite.setPosition(posx+4, posy+16);

	playerSprite.setTexture(*BaseItems.getTexture(Game.looktypes.at(CreaturesManager.getCreature(Game.myPlayerID)->getLooktype()).south));
	
	size = playerSprite.getTexture()->getSize();	
	if(size.x == 32 && size.y == 32)
		playerSprite.setPosition(float(posx+36),float(posy+137));
	else if(size.x == 64 && size.y == 64)
		playerSprite.setPosition(float(posx+4),float(posy+105));

	said.clear();
	choices.clear();
	this->show = true;
}

void NPCWindow::addSaid(string text)
{
	sf::Text *saiding = new sf::Text();
	saiding->setFont(System.font);
	saiding->setCharacterSize(12);

	if(transparent)
		saiding->setColor(sf::Color(255,255,255,216));
	else
		saiding->setColor(sf::Color::White);
	//saiding->setStyle(sf::Text::Bold);
	saiding->setPosition(posx+74+2, posy+17+15*(this->said.size()));
	saiding->setString(text);
	said.push_back(saiding);
}

void NPCWindow::addChoice(string text)
{
	string newText = intToStr(choices.size()+1);
	newText += ". ";
	newText += text;
	sf::Text *choice = new sf::Text();
	choice->setFont(System.font);
	choice->setCharacterSize(12);
	if(transparent)
		choice->setColor(sf::Color(255,255,255,216));
	else
		choice->setColor(sf::Color::White);
	//choice->setStyle(sf::Text::Bold);
	choice->setPosition(posx+74+2, posy+107+15*(this->choices.size()));
	choice->setString(newText);
	choices.push_back(choice);
}

void NPCWindow::addSells(unsigned int itemId, unsigned int price, string name)
{
	sells.push_back(shopOffer());
	sells.at(sells.size()-1).id = itemId;
	sells.at(sells.size()-1).price = price;
	sells.at(sells.size()-1).name = name;

	sells.at(sells.size()-1).sfName.setFont(System.font);
	sells.at(sells.size()-1).sfName.setColor(sf::Color::White);
	sells.at(sells.size()-1).sfName.setCharacterSize(12);
	sells.at(sells.size()-1).sfName.setString(name);

	sells.at(sells.size()-1).sfPrice.setFont(System.font);
	sells.at(sells.size()-1).sfPrice.setColor(sf::Color::White);
	sells.at(sells.size()-1).sfPrice.setCharacterSize(12);
	sells.at(sells.size()-1).sfPrice.setString(intToStr(price));
}

void NPCWindow::addBuys(unsigned int itemId, unsigned int price, string name)
{
	buys.push_back(shopOffer());
	buys.at(buys.size()-1).id = itemId;
	buys.at(buys.size()-1).price = price;
	buys.at(buys.size()-1).name = name;

	buys.at(buys.size()-1).sfName.setFont(System.font);
	buys.at(buys.size()-1).sfName.setColor(sf::Color::White);
	buys.at(buys.size()-1).sfName.setCharacterSize(12);
	buys.at(buys.size()-1).sfName.setString(name);

	buys.at(buys.size()-1).sfPrice.setFont(System.font);
	buys.at(buys.size()-1).sfPrice.setColor(sf::Color::White);
	buys.at(buys.size()-1).sfPrice.setCharacterSize(12);
	buys.at(buys.size()-1).sfPrice.setString(intToStr(price));
}