#include <SFML/Network.hpp>
#include <SFML/Graphics.hpp>
#include <iostream>
#include "system.h"
#include "game.h"
#include "chat.h"
#include "enums.h"
#include "creaturesManager.h"

extern Game Game;
extern System System;
extern CreaturesManager CreaturesManager;
extern string intToStr(int);
extern struct KeyStorage keyStorage;
extern bool SFKeyToString(unsigned int keycode, char &keyStr, bool isShift, bool numbers = false); 

ChatChannel::ChatChannel(string name, bool _default = false)
{
	this->name = name;
	this->sfName.setFont(System.font);
	this->sfName.setString(name);
	this->sfName.setCharacterSize(12);
	this->sfName.setColor(sf::Color::White);
	this->mDefault = _default;
}

void ChatChannel::addMessage(string message, sf::Color color)
{
	messages.push_back(sf::Text(message, System.font));
	messages.at(messages.size()-1).setCharacterSize(12);
	messages.at(messages.size()-1).setColor(color);
	messages.at(messages.size()-1).setPosition(5.0,System.App->getSize().y-40);
}

sf::Text* ChatChannel::getMessage(unsigned int pos)
{
	if(pos < messages.size())
		return &messages.at(pos);
	else
		return NULL;
}

void ChatChannel::render()
{
	System.App->draw(this->sfName);
}

Chat::Chat()
{
	this->channelsWindow.loadFromFile("data/channels.bmp");
	this->channelsWindowSprite.setTexture(this->channelsWindow);

	this->mainRect.setFillColor(sf::Color(92,92,92));
	this->mainRect.setOutlineThickness(2);
	this->mainRect.setOutlineColor(sf::Color::Black);
	this->textBox.setFillColor(sf::Color(112,112,112));
	this->textBox.setOutlineColor(sf::Color(55,59,62));
	this->textBox.setOutlineThickness(1);

	this->messagersShape.setFillColor(sf::Color(55,59,62));
	this->messagersShape.setOutlineColor(sf::Color(0,0,0));
	this->messagersShape.setOutlineThickness(1);

	this->caret = callback.size()-1;
	this->insert = true;
	this->iter = callback.end();

    this->blinking = clock();
	this->canSend = clock();

    this->callbackSF.setString(callback);
	this->callbackSF.setFont(System.font);
	this->callbackSF.setCharacterSize(12);
	
	serverMessage.setFont(System.font);
	serverMessage.setCharacterSize(12);
	serverMessageShape.setSize(sf::Vector2f(544.0, 16.0));
	serverMessageShape.setPosition(32.0, 432.0);
	serverMessageShape.setFillColor(sf::Color(0, 0, 0, 128));
	serverMessageReceivedTime = 0;

	currentChannel = 0;
	channels.push_back(ChatChannel("Global", true));
	activeChannel = 0;
	newConversation = false;
	channelsCaret = channelsCallback.size()-1;
	channelsIter = channelsCallback.end();
    this->sfChannelsCallback.setString(channelsCallback);
	this->sfChannelsCallback.setFont(System.font);
	this->sfChannelsCallback.setCharacterSize(12);

	#ifdef _DEBUG
		std::cout << "Chat loaded." << std::endl;
	#endif
}

void Chat::setPos(int _posx, int _posy, int _maxx, int _maxy)
{
	posx = _posx;
	posy = _posy;
	maxx = _maxx;
	maxy = _maxy;
	this->mainRect.setPosition(posx, posy);
	this->mainRect.setSize(sf::Vector2f(maxx-posx,maxy-posy));
	
	this->textBox.setPosition(posx,maxy-12);
	this->textBox.setSize(sf::Vector2f(float(maxx-posx-100),float(12)));

	this->messagersShape.setPosition(posx-1,maxy-30);
	this->messagersShape.setSize(sf::Vector2f(float(maxx-posx-258),float(15)));

	rectangle.setSize(sf::Vector2f(float(1), float(10)));
	rectangle.setFillColor(sf::Color(255,255,255));
	rectangle.setOutlineColor(sf::Color(255, 255, 255));
	rectangle.setOutlineThickness(0);

	this->callbackSF.setPosition(4, maxy-14);
	this->sfChannelsCallback.setPosition(posx + 511.0, posy-2);

	channelsWindowSprite.setPosition(maxx-99, maxy-110);
}

Chat::~Chat()
{             
}

void Chat::addMessage(unsigned char type, string message, string sender)
{
	sf::Color msgColor;
	switch(type)
	{
		case 0: //SYSTEM - WHITE
			msgColor = sf::Color::White;
			break;
		case 1: //SYSTEM - RED
			msgColor = sf::Color(230,170,30);
			break;
		case 2: //PUBLIC - WHITE
			msgColor = sf::Color::White;
			break;
		case 3: //PUBLIC - RED
			msgColor = sf::Color(230,170,30);
			break;	
		case 4: //PRIVATE - BROWN
			msgColor = sf::Color(6,253,62);
			break;
		case 5: //PRIVATE_ME - BROWN
			msgColor = sf::Color(100,134,255);
			break;
		default:
			msgColor = sf::Color::White;
			break;
	}

	if(type == 0)
	{
		serverMessage.setString(message);
		serverMessage.setPosition(int(300 - (float(message.size())*3.5)), 432.0);
		serverMessageReceivedTime = clock();
	}
	else if(type >= 1 && type <= 3) //wiadomoœæ przeznaczona na publiczny kana³
	{
		channels[0].addMessage(message, msgColor);
		activeChannel = 0;
	}
	else //wiadomoœæ od innego gracza
	{
		int xxx = 0;
		for(deque<ChatChannel>::iterator it = channels.begin(); it != channels.end(); it++)
		{
			if(it->getName() == sender)
			{
				it->addMessage(message, msgColor);
				activeChannel = xxx;
				return;
			}
			xxx++;
		}
		channels.push_back(ChatChannel(sender));
		channels.at(channels.size()-1).addMessage(message, msgColor);
		activeChannel = channels.size()-1;
	}
}

void Chat::receiveMessage(sf::Packet &message)
{      
	unsigned char type;
	unsigned int temp;
	string senderName;
	string msg;
	string msgTemp = "";
	string fromMe = "";
	message >> type;

	if(type >= 2)
	{
		message >> msgTemp;
		msg = msgTemp;
		senderName = msgTemp;
		msg +="[";
		message >> temp;
		msg += intToStr(temp);
		msg += "]: ";
		message >> msgTemp;
		msg += msgTemp;
		if(type == 5)
			message >> fromMe;
	}
	else
	{
		message >> msgTemp;
		message >> msgTemp;
		message >> msgTemp;
		msg = msgTemp;
	}

	if(type != 5)
		this->addMessage(type, msg, senderName);
	else
		this->addMessage(type, msg, fromMe);
}

void Chat::render()
{
	System.App->draw(this->mainRect);
	System.App->draw(this->textBox);
	System.App->draw(this->channelsWindowSprite);

	unsigned int iterator = channels[activeChannel].getMessagesNumber();
    for(unsigned int i = 0; i < (maxy-posy-15)/15; i++)
    {
		if(iterator > 0)
			iterator--;
		else
			break;

		if(channels[activeChannel].getMessagesNumber()-1 >= iterator)
		{
			sf::Text *message = channels[activeChannel].getMessage(iterator);
			message->setPosition(5.0,float(maxy-36-(i*15)));
			System.App->draw(*message);
		}
    }

	if(callbackSF.getString() != callback)
	{
		callbackSF.setString(callback);
		iter = callback.end();
		caret = callback.size()-1;
	}
	if(sfChannelsCallback.getString() != channelsCallback)
	{
		sfChannelsCallback.setString(channelsCallback);
		channelsIter = channelsCallback.end();
		channelsCaret = channelsCallback.size()-1;
	}
	System.App->draw(callbackSF);
	System.App->draw(sfChannelsCallback);

    if(blinking + 500 <= clock())
	{
		if(this->newConversation == false)
			rectangle.setPosition(float(12+caret*7), float(maxy-11));
		else
			rectangle.setPosition(float(posx + 519.0 + channelsCaret*7), float(posy+1));
		System.App->draw(rectangle);
	}

	if(serverMessageReceivedTime + 3000 > clock())
	{
		System.App->draw(this->serverMessageShape);
		System.App->draw(this->serverMessage);
	}

	for(unsigned int i = 0; i < channels.size(); i++)
	{
		if(i+currentChannel >= channels.size() || i > 7)
			break;
		if(activeChannel == i+currentChannel)
			channels[i+currentChannel].setActive(true);
		else
			channels[i+currentChannel].setActive(false);

		channels[i+currentChannel].setPosition(maxx-95.0, posy + 14.0 + i*13.0);
		channels[i+currentChannel].render();
	}
}

bool Chat::mouse(unsigned char type, int mouse_x, int mouse_y) //lokalne x i y myszki.
{
	if(type == 137) //ruch rolk¹ myszki
	{
		if(mouse_x >= 0 && mouse_x <= 510 && mouse_y >= 0 && mouse_y <= maxy-posy)
		{
			//////
			return true;
		}
		if(mouse_x >= 510 && mouse_x <= 610 && mouse_y >= 0 && mouse_y <= maxy-posy)
		{
			currentChannel -= System.mouseDelta;
			if(currentChannel > int(channels.size()) - 7)
				currentChannel = channels.size() - 7;
			if(currentChannel < 0)
				currentChannel = 0;
			return true;
		}
	}
	if(type == 4)
	{
		if(mouse_x >= 598 && mouse_x <= 610 && mouse_y >= 1 && mouse_y <= 12)
		{ 
			Game.sendMessage(4, "Started Conversation.", channelsCallback);
			channelsCallback.clear();
			sfChannelsCallback.setString(channelsCallback);
			channelsIter = channelsCallback.end();
			channelsCallback.size()-1;
			channelsCaret = -1;
			newConversation = false;
			return true;
		}
		if(mouse_x >= 510 && mouse_x <= 598 && mouse_y >= 1 && mouse_y <= 12)
		{ 
			this->newConversation = true;
			return true;
		}
		if(mouse_x >= 0 && mouse_x <= 510 && mouse_y >= 0 && mouse_y <= maxy-posy)
		{
			this->newConversation = false;
			return true;
		}
		if(mouse_x >= 510 && mouse_x <= 598)
		{
			for(int i = 0; i < 7; i++)
			{
				if(mouse_y >= (18+i*13) && mouse_y <= (31+i*13))
				{
					if(i + currentChannel < channels.size())
						activeChannel = i+currentChannel;
					return true;
				}
			}
		}
		if(mouse_x >= 296 && mouse_x <= 306)
		{
			if(mouse_y >= 14 && mouse_y <= 25)
			{
				if(currentChannel > 0)
					currentChannel--;
				return true;
			}
			if(mouse_y >= 178 && mouse_y <= 190)
			{
				if(currentChannel < channels.size() - 7)
					currentChannel++;
				return true;
			}
		}
	}
	return false;
}

bool Chat::proceedSendMessage()
{
	if(blinking + 1000 <= clock())
		blinking = clock();

	return false;
}

void Chat::sendMessage()
{
	if(canSend + 1000 <= clock())
	{
		if(activeChannel == 0)
			Game.sendMessage(2,callback);
		else if(activeChannel < channels.size())
			Game.sendMessage(4, callback, channels.at(activeChannel).getName());

		lastSended = callback;
		canSend = clock();

		callback.clear();
	}
}

void Chat::updateToSend(sf::Uint32 unicode)
{
	bool isSpecial = true;

	switch(unicode)
	{
		/*
		case 127: //sf::Keyboard::Delete:
			if(iter != callback.end()) iter = callback.erase(iter);
		break;

		case sf::Keyboard::Right:
			if(iter != callback.end() && caret < 30)   caret++, iter++;
		break;

		case sf::Keyboard::Left:
			if(iter != callback.begin()) caret--, iter--;
		break;

		case sf::Keyboard::Insert:
			insert = !insert;
		break; 
		*/
		case 8: //sf::Keyboard::BackSpace: BACKSPACE
			if(iter != callback.begin())
			{
				caret--;
				iter--;
				iter = callback.erase(iter);
			}
		break;

		case 13: //sf::Keyboard::Return: ENTER
			this->sendMessage();
		break;   

		default:
			isSpecial = false;
		break;
	} 

	if(isSpecial == false && unicode > 31 && callback.size() < 50)
	{
		char c = static_cast<char>(unicode);

		if(insert || iter == callback.end())
		{
			iter = callback.insert(iter, c);
		}
		else
		{
			callback.replace(caret, 1, 1, c);
		}
		caret++;
		iter++;
	}
}

void Chat::updateNewConversation(sf::Uint32 unicode)
{
	bool isSpecial = true;

	switch(unicode)
	{
		case 8: //sf::Keyboard::BackSpace: BACKSPACE
			if(channelsIter != channelsCallback.begin())
			{
				channelsCaret--;
				channelsIter--;
				channelsIter = channelsCallback.erase(channelsIter);
			}
		break;

		case 13: //sf::Keyboard::Return: ENTER
			Game.sendMessage(4, "Started Conversation.", channelsCallback);
			channelsCallback.clear();
			sfChannelsCallback.setString(channelsCallback);
			channelsIter = channelsCallback.end();
			channelsCallback.size()-1;
			channelsCaret = -1;
			newConversation = false;
		break;   

		default:
			isSpecial = false;
		break;
	} 

	if(isSpecial == false && unicode > 31 && channelsCallback.size() < 12)
	{
		char c = static_cast<char>(unicode);

		if(insert || channelsIter == channelsCallback.end())
		{
			channelsIter = channelsCallback.insert(channelsIter, c);
		}
		else
		{
			channelsCallback.replace(channelsCaret, 1, 1, c);
		}
		channelsCaret++;
		channelsIter++;
	}
}

void Chat::setLastSended()
{
	callback = lastSended;
	caret = callback.size();
	iter = callback.end();
}

void Chat::nextActiveTab()
{
	if(activeChannel < channels.size()-1)
		activeChannel++;
	else
		activeChannel = 0;
}