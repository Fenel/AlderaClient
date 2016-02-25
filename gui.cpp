#include <iostream>
#include <string>
#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include "enums.h"
#include "system.h"
#include "gui.h"
#include "menu.h"
#include "game.h"

extern System System;
extern sf::Texture guiCheckBox, guiCheckBoxChecked;
extern int mouse_x, mouse_y, mouse_b;
extern std::string intToStr(int n);

void GuiElement::draw()
{
	#ifdef _DEBUG
		std::cout << "GuiElement Virtual Draw" << std::endl;
	#endif
}

bool GuiElement::proceed()
{
     return false;
}

GuiButton::GuiButton(int x, int y, std::string _text, int _rectSize, bool _center, void (*_function)()) : functionMenu(_function)
{
    local_x = x;
    local_y = y;
	rectSize = _rectSize;
	center = _center;

	this->text.setFont(System.font);
    this->text.setString(_text);
	if(center && rectSize != 0)
	{
		this->text.setPosition(float(x+(rectSize/2)-(_text.size()*7/2)), float(y+3));
	}
	else
		this->text.setPosition(float(x + 2), float(y+3));

	this->text.setCharacterSize(11);

	if(rectSize == 0)
		rectSize = _text.size()*7 + 4;

	rectangle.setSize(sf::Vector2f(float(rectSize), float(20)));
	rectangle.setFillColor(sf::Color(32,32,32));
	rectangle.setOutlineColor(sf::Color(255, 255, 255));
	rectangle.setOutlineThickness(1);
	rectangle.setPosition(float(local_x), float(local_y));
}


bool GuiButton::proceed()
{
	if(mouse_x >= local_x && mouse_x <= local_x + rectSize && mouse_y >= local_y && mouse_y <= local_y + 20 && mouse_b == 1 && System.hold == false)
	{
		(*functionMenu)();
		System.hold = true;
		return true;
	}
	return false;
}

void GuiButton::draw()
{
	if(mouse_x >= local_x && mouse_x <= local_x + rectSize && mouse_y >= local_y && mouse_y <= local_y + 20)
	{
		rectangle.setFillColor(sf::Color(64,64,64));
	}
	else
		rectangle.setFillColor(sf::Color(32,32,32));

	System.App->draw(rectangle);
	System.App->draw(text);
}

GuiLabel::GuiLabel(int x, int y, std::string _text, int _rectSize, bool _center)
{
    local_x = x;
    local_y = y;
	rectSize = _rectSize;
	center = _center;

	this->text.setFont(System.font);
    this->text.setString(_text);
	if(center && rectSize != 0)
	{
		this->text.setPosition(float(x+(rectSize/2)-(_text.size()*7/2)), float(y));
	}
	else
		this->text.setPosition(float(x + 2), float(y));

	this->text.setCharacterSize(11);

	if(rectSize == 0)
		rectSize = _text.size()*7 + 4;

	rectangle.setSize(sf::Vector2f(float(rectSize), float(15)));
	rectangle.setFillColor(sf::Color(32,32,32));
	rectangle.setOutlineColor(sf::Color(255, 255, 255));
	rectangle.setOutlineThickness(1);
	rectangle.setPosition(float(local_x), float(local_y));
}


bool GuiLabel::proceed()
{
	return false;
}

void GuiLabel::draw()
{
	System.App->draw(rectangle);
	System.App->draw(text);
}

void GuiLabel::updateText(std::string _text)
{
    this->text.setString(_text);
	if(center && rectSize != 0)
	{
		this->text.setPosition(float(local_x+(rectSize/2)-(_text.size()*7/2)), float(local_y));
	}
	else
	{
		this->text.setPosition(float(local_x + 2), float(local_y));
		rectangle.setSize(sf::Vector2f(float(_text.size()*7 + 4), float(15)));
	}

}

GuiCheckBoxLabel::GuiCheckBoxLabel(int x, int y, std::string _text, bool &Pcallback) : callback(Pcallback)
{
    local_x = x;
    local_y = y;

	this->text.setFont(System.font);
    this->text.setString(_text);
	this->text.setPosition(float(x + 15), float(y));
	this->text.setCharacterSize(11);

	this->unchecked.setTexture(guiCheckBox);
	this->unchecked.setPosition(float(x+1), float(y+1));

	this->checked.setTexture(guiCheckBoxChecked);
	this->checked.setPosition(float(x+1), float(y+1));

	rectangle.setSize(sf::Vector2f(float(318), float(15)));
	rectangle.setFillColor(sf::Color(32,32,32));
	rectangle.setOutlineColor(sf::Color(255, 255, 255));
	rectangle.setOutlineThickness(1);
	rectangle.setPosition(float(local_x), float(local_y));
}


bool GuiCheckBoxLabel::proceed()
{
      if(mouse_x >= local_x+1 && mouse_x <= local_x + 13 && mouse_y >= local_y+1 && mouse_y <= local_y + 13 && mouse_b == 1 && System.hold == false)
      {
		  callback = !callback;
		  System.hold = true;
          return true;
      }
	  return false;
}

void GuiCheckBoxLabel::draw()
{
	System.App->draw(rectangle);      

	if(callback == false)
	{
		System.App->draw(unchecked);
	}
	else
	{
		System.App->draw(checked);
	}
	System.App->draw(text);
}

GuiTextBoxLabel::GuiTextBoxLabel(int x, int y, std::string _label, bool _password, std::string &Pcallback) : callback(Pcallback)
{
	this->password = _password;
	this->caret = callback.size()-1;
	this->insert = true;
	this->iter = callback.end();

    System.globalTextboxesCount++;
    id = System.globalTextboxesCount;
    blinking = clock();
	this->label = _label;
	this->local_x = x;
	this->local_y = y;

	this->labelSF.setFont(System.font);
    this->labelSF.setString(_label);
	this->labelSF.setPosition(float(x+2), float(y));
	this->labelSF.setCharacterSize(12);

	this->callbackSF.setFont(System.font);
	this->callbackSF.setPosition(float(x+100), float(y));
	this->callbackSF.setCharacterSize(12);
	this->callbackSF.setString(callback);

	rectangle.setSize(sf::Vector2f(float(100), float(15)));
	rectangle.setOutlineColor(sf::Color(255, 255, 255));
	rectangle.setOutlineThickness(1);
	rectangle.setPosition(float(x), float(y));
}

bool GuiTextBoxLabel::proceed()
{
	if( mouse_x >= local_x+100 && mouse_x <= local_x + 320 && mouse_y >= local_y && mouse_y <= local_y + 15 && mouse_b == 1)
    {
        System.activeTextbox = id;
        return true;
    }
	if(System.activeTextbox == id && blinking + 1000 <= clock())
	{
		blinking = clock();
	}
    if(System.activeTextbox == id && System.keyProcessed == false)
	{ 
		System.keyProcessed = true;
		bool isSpecial = true;

		switch(System.unicode)
		{
			case 8: //BACKSPACE
				if(iter != callback.begin())
				{
					caret--;
					iter--;
					iter = callback.erase(iter);
				}
				break;
			case 11: //TAB
				caret = 0;
				System.activeTextbox++;
				break; 
			default:
				isSpecial = false;
				break;
		} 

		if(isSpecial == false && System.unicode > 31 && callback.size() < 39)
		{
			char c = static_cast<char>(System.unicode);

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
		if(this->password)
		{
			gwiazdki = "";
			for(int i = 0; i < callback.size(); i++)
			{
				gwiazdki += "*";
			}
			callbackSF.setString(gwiazdki);
		}
		else
		{
			callbackSF.setString(callback);
		}

		return true;
	}
	return false;
}

void GuiTextBoxLabel::draw()
{  
	if(!password && callbackSF.getString() != callback)
	{
		callbackSF.setString(callback);
		iter = callback.end();
		caret = callback.size()-1;
	}
	if(password && callback.size() != gwiazdki.size())
	{
		gwiazdki = "";
		for(int i = 0; i < callback.size(); i++)
		{
			gwiazdki += "*";
		}
		callbackSF.setString(gwiazdki);
		iter = callback.end();
		caret = callback.size()-1;
	}
	
	rectangle.setSize(sf::Vector2f(float(100), float(15)));
	rectangle.setFillColor(sf::Color(32,32,32));
	rectangle.setOutlineColor(sf::Color(255, 255, 255));
	rectangle.setOutlineThickness(1);
	rectangle.setPosition(float(local_x), float(local_y));
	System.App->draw(rectangle);

	rectangle.setSize(sf::Vector2f(float(220), float(15)));
	rectangle.setFillColor(sf::Color(96,96,96));
	rectangle.setPosition(float(local_x+98), float(local_y));
	System.App->draw(rectangle);

	System.App->draw(labelSF);
	System.App->draw(callbackSF);

    if(blinking + 500 <= clock() && System.activeTextbox == id)
	{
		rectangle.setSize(sf::Vector2f(float(1), float(13)));
		rectangle.setFillColor(sf::Color(255,255,255));
		rectangle.setOutlineColor(sf::Color(255, 255, 255));
		rectangle.setOutlineThickness(0);
		rectangle.setPosition(float(local_x+108+caret*7), float(local_y+1));
		System.App->draw(rectangle);
	}
}

GuiNumberBoxLabel::GuiNumberBoxLabel(int x, int y, std::string _label, bool _password, int32_t &Pnumber) : number(Pnumber)
{
	this->password = _password;
	this->caret = callback.size()-1;
	this->insert = true;
	this->iter = callback.end();

    System.globalTextboxesCount++;
    id = System.globalTextboxesCount;
    blinking = clock();
	this->label = _label;
	this->local_x = x;
	this->local_y = y;

	this->callback = intToStr(number);

	this->labelSF.setFont(System.font);
    this->labelSF.setString(_label);
	this->labelSF.setPosition(float(x+2), float(y));
	this->labelSF.setCharacterSize(12);

	this->callbackSF.setFont(System.font);
    this->callbackSF.setString(callback);
	this->callbackSF.setPosition(float(x+100), float(y));
	this->callbackSF.setCharacterSize(12);

	rectangle.setSize(sf::Vector2f(float(100), float(15)));
	rectangle.setOutlineColor(sf::Color(255, 255, 255));
	rectangle.setOutlineThickness(1);
	rectangle.setPosition(float(x), float(y));
}

bool GuiNumberBoxLabel::proceed()
{
	if( mouse_x >= local_x+100 && mouse_x <= local_x + 320 && mouse_y >= local_y && mouse_y <= local_y + 15 && mouse_b == 1)
    {
        System.activeTextbox = id;
        return true;
    }
	if(System.activeTextbox == id && blinking + 1000 <= clock())
	{
		blinking = clock();
	}
    if(System.activeTextbox == id && System.keyProcessed == false)
	{ 
		System.keyProcessed = true;
		bool isSpecial = true;

		switch(System.unicode)
		{
			case 8: //BACKSPACE
				if(iter != callback.begin())
				{
					caret--;
					iter--;
					iter = callback.erase(iter);
				}
				break;
			case 11: //TAB
				caret = 0;
				System.activeTextbox++;
				break; 
			default:
				isSpecial = false;
				break;
		} 

		if(isSpecial == false && System.unicode > 31 && callback.size() < 39)
		{
			char c = static_cast<char>(System.unicode);

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
		number = atoi(callback.c_str());

		if(this->password)
		{
			std::string gwiazdki = "";
			for(int i = 0; i < callback.size(); i++)
			{
				gwiazdki += "*";
			}
			callbackSF.setString(gwiazdki);
		}
		else
			callbackSF.setString(intToStr(number));

		return true;
	}
	return false;
}

void GuiNumberBoxLabel::draw()
{       
	if(callbackSF.getString() != callback)
	{
		number = atoi(callback.c_str());
		callbackSF.setString(callback);
		iter = callback.end();
		caret = callback.size()-1;
	}

	rectangle.setSize(sf::Vector2f(float(100), float(15)));
	rectangle.setFillColor(sf::Color(32,32,32));
	rectangle.setOutlineColor(sf::Color(255, 255, 255));
	rectangle.setOutlineThickness(1);
	rectangle.setPosition(float(local_x), float(local_y));
	System.App->draw(rectangle);

	rectangle.setSize(sf::Vector2f(float(220), float(15)));
	rectangle.setFillColor(sf::Color(96,96,96));
	rectangle.setPosition(float(local_x+98), float(local_y));
	System.App->draw(rectangle);

	System.App->draw(labelSF);
	System.App->draw(callbackSF);

    if(blinking + 500 <= clock() && System.activeTextbox == id)
	{
		rectangle.setSize(sf::Vector2f(float(1), float(13)));
		rectangle.setFillColor(sf::Color(255,255,255));
		rectangle.setOutlineColor(sf::Color(255, 255, 255));
		rectangle.setOutlineThickness(0);
		rectangle.setPosition(float(local_x+108+caret*7), float(local_y+1));
		System.App->draw(rectangle);
	}
}



