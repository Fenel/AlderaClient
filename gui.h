#ifndef GUI_H
#define GUI_H

#include <SFML/Graphics.hpp>
#include <deque>
#include "thing.h"
#include "enums.h"

using namespace std;

class Game;
class Menu;

class GuiElement
{
public:
 virtual void draw();
 virtual bool proceed();
protected:
 int local_x, local_y;
};

class GuiManager
{
public:
 GuiManager()	{ erase = false, eraseAll = false; };
 void clear()
 {
	 elements.clear();
 }
 void eraseAccSelection()
 {
	 elements.erase(elements.begin(),elements.begin()+10);
 };
 bool erase;
 bool eraseAll;
 deque<GuiElement *> elements;
private:
};

class GuiButton : public GuiElement
{
public:
 //GuiButton::GuiButton(int x, int y, std::string _text, int _rectSize, bool _center, void (Menu::*_function)());
 GuiButton::GuiButton(int x, int y, std::string _text, int _rectSize, bool _center, void (*_function)());
 bool proceed();
 void draw();
private:
 //void (Menu::*functionMenu)();
 void (*functionMenu)();
 void (Game::*functionGame)();
 sf::Text text;
 int rectSize;
 bool center;

  sf::RectangleShape rectangle;
};

class GuiLabel : public GuiElement
{
public:
 GuiLabel::GuiLabel(int x, int y, std::string _text, int _rectSize, bool _center = false);
 bool proceed();
 void draw();
 void updateText(std::string _text);
private:
 int rectSize;
 bool center;
 sf::Text text;
 sf::RectangleShape rectangle;
};

class GuiCheckBoxLabel : public GuiElement
{
public:
 GuiCheckBoxLabel::GuiCheckBoxLabel(int x, int y, std::string _text, bool &Pcallback);
 bool proceed();
 void draw();
 sf::Sprite checked;
 sf::Sprite unchecked;
private:
 bool &callback;
 sf::Text text;

  sf::RectangleShape rectangle;
};

class GuiTextBoxLabel: public GuiElement
{
public:
 GuiTextBoxLabel(int x, int y, std::string _label, bool _password, std::string &Pcallback);
 bool proceed();
 void draw();
private:
 int id;
 bool password;
 string::iterator iter;
 unsigned int iter2;
 int caret;
 bool insert; 

 std::string &callback;
 std::string label;
 sf::Text labelSF;
 sf::Text callbackSF;
 std::string gwiazdki;

 sf::RectangleShape rectangle;
 
 clock_t blinking;
};

class GuiNumberBoxLabel: public GuiElement
{
public:
 GuiNumberBoxLabel(int x, int y, std::string label, bool _password, int32_t &number);
 bool proceed();
 void draw();
private:
 int id;
 bool password;
 string::iterator iter;
 int caret;
 bool insert; 

 int32_t &number;
 std::string callback;
 std::string label;
 sf::Text labelSF;
 sf::Text callbackSF;

 sf::RectangleShape rectangle;
 
 clock_t blinking;
};

#endif