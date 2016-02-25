#ifndef _DEBUG

#ifndef __MENU_H
#define __MENU_H

#include <vector>
#include <string>
#include "enums.h"
#include "gui.h"

using namespace std;

class Menu
{
public:
  Menu();
  ~Menu();
  void loop();
  unsigned int getFrameRate();
  static void createNewAccount();
  static void loginAccount();
  static void createNewPlayer();
  static void loginPlayer();

  void addButton(int posx, int posy, std::string label, int size, bool center, void (*function)());
  void addLabel(int posx, int posy, std::string label, int size, bool center = false);
  void addCheckBoxLabel(int posx, int posy, std::string label, bool &Pcallback);
  void addTextBoxLabel(int posx, int posy, std::string label, bool password, std::string &text);
  void addNumberBoxLabel(int posx, int posy, std::string label, bool password, int32_t &number);

  static vector<GuiElement*> elements;
 
  //static std::string newAccountName, newEmail, newPassword, newRepeatPassword;
  //static std::string existingAccountName, existingPassword;

  static bool Menu::isAccountLogged;

  private:
    sf::Event Event;
};

#endif

#endif