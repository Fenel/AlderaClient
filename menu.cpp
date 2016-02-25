#ifndef _DEBUG

#include <string>
#include <iostream>
#include <deque>
#include <time.h>

#include "system.h"
#include "menu.h"
#include "gui.h"

extern System System;
extern GuiManager GuiManager;
extern std::string intToStr(int);
extern bool isLettersOnly(std::string str);
extern int mouse_x, mouse_y, mouse_b;
extern sf::Sprite menuBackgroundSprite;

GuiLabel* labelStatus;

deque<std::string> characters;
deque<bool>        selectedCharacters;

std::string newAccountName = "";
std::string newEmail = "";
std::string newPassword = "";
std::string newRepeatPassword = "";
std::string existingAccountName = "";
std::string existingPassword = "";
bool Menu::isAccountLogged = false;

Menu::Menu()
{
	labelStatus = new GuiLabel(120,360,"",560,true);
	this->addLabel(20,410,"Create new account:",317,true);
	this->addTextBoxLabel(20,440,"Account name",false,newAccountName);
	this->addTextBoxLabel(20,470,"E-mail",false,newEmail);
	this->addTextBoxLabel(20,500,"Password",true,newPassword);
	this->addTextBoxLabel(20,530,"Repeat passw.",true,newRepeatPassword);
	this->addButton(140,560,"Proceed",80,true,&Menu::createNewAccount);

	this->addLabel(463,410,"Log In",317,true);
	this->addTextBoxLabel(463,440,"Account name",false,existingAccountName);
	this->addTextBoxLabel(463,470,"Password",true,existingPassword);
	this->addButton(575,500,"Proceed",80,true,&Menu::loginAccount);

	#ifdef _DEBUG
		std::cout << "Menu setted." << std::endl;
	#endif
}

Menu::~Menu()
{
               
}

inline unsigned int Menu::getFrameRate()
{
	return 30;
}

void Menu::loop()
{  
	while(System.App->isOpen() && System.gameState == 2)
	{		
		sf::Event Event;
        while (System.App->pollEvent(Event))
        {
			if (Event.type == Event.MouseButtonPressed && sf::Mouse::isButtonPressed(sf::Mouse::Left))
			mouse_b = 1;
			if (Event.type == Event.MouseButtonPressed && sf::Mouse::isButtonPressed(sf::Mouse::Right))
			mouse_b = 2;
			if (Event.type == Event.MouseButtonPressed && sf::Mouse::isButtonPressed(sf::Mouse::Middle))
			mouse_b = 3;
			if (Event.type == Event.MouseButtonReleased )
			mouse_b = 0;
			if (Event.type == Event.MouseMoved)
			{
				mouse_x = Event.mouseMove.x;
				mouse_y = Event.mouseMove.y;
			}
			if(Event.type == sf::Event::TextEntered)
			{
				System.unicode = Event.text.unicode;
				System.keyProcessed = false;
			}
			if(Event.type == sf::Event::MouseButtonReleased)
	        {
				 System.hold = false;
	        } 
			if(Event.type == sf::Event::Closed)
			   System.App->close();
        }
		System.App->clear();

		System.App->draw(menuBackgroundSprite);

		if(labelStatus)
			labelStatus->draw();

		if(GuiManager.erase)
		{
			GuiManager.eraseAccSelection();
			System.activeTextbox = 0;
			System.globalTextboxesCount = 0;
			GuiManager.erase = false;
		}
		if(GuiManager.eraseAll)
		{
			GuiManager.clear();
			System.activeTextbox = 0;
			System.globalTextboxesCount = 0;
			GuiManager.eraseAll = false;
		}

		if(!GuiManager.elements.empty())
			for(unsigned int iter = 0; iter < GuiManager.elements.size(); iter++)
			{
				GuiManager.elements[iter]->proceed();
				GuiManager.elements[iter]->draw();
			}

		System.App->display();
		this->getFrameRate();
	}  
}

void Menu::createNewAccount()
{
	sf::Packet toSend;
	sf::Packet toReceive;

	int id = 1337;
	int recvid = -1;
	std::string message = System.clientVersion;
	std::string recvmsg = "";

	bool proceed = true;

	if(newAccountName.size() < 6) 
	{
		labelStatus->updateText("Account name must have at least 6 characters.");
		proceed = false;
	}
	if(newAccountName.size() > 20) 
	{
		labelStatus->updateText("Account name is too long.");
		proceed = false;
	}
	if((newEmail.find("@") == string::npos) || (newEmail.size() < 5) || (newEmail.find(".") == string::npos))
	{
		labelStatus->updateText("Typed email is wrong.");
		proceed = false;
	}
	if(newPassword != newRepeatPassword)
	{
		labelStatus->updateText("Passwords is different.");
		proceed = false;
	}
	if(newPassword.size() < 6)
	{
		labelStatus->updateText("Password must have at least 6 characters.");
		proceed = false;
	}
	if(newPassword.size() > 20)
	{
		labelStatus->updateText("Password is too long.");
		proceed = false;
	}

	if(proceed)
	{
		if(System.connect())
		{
			toSend << id << message;

			System.sClient.send(toSend);

			System.sClient.receive(toReceive);
			toReceive >> recvid;

			if(recvid != 1337)
			{
				toReceive >> recvmsg;
				
				newAccountName = "";
				newEmail = "";
				newPassword = "";
				newRepeatPassword = "";
				System.activeTextbox = 0;
				labelStatus->updateText(recvmsg);
				System.disconnect();
			}
			else
			{
				bool success;
				toSend.clear();
				id = 2;
				toSend << id << newAccountName << newPassword << newEmail;
				System.sClient.send(toSend);

				System.sClient.receive(toReceive);
				toReceive >> recvid >> success;
				
				if(recvid == 2 && success)
				{
					labelStatus->updateText("Account succesfully created. Now you are able to login.");
					newAccountName = "";
					newEmail = "";
					newPassword = "";
					newRepeatPassword = "";
					System.activeTextbox = 0;
					System.disconnect();
				}
				else
				{
					labelStatus->updateText("Please type different account name.");
					newAccountName = "";
					newEmail = "";
					newPassword = "";
					newRepeatPassword = "";
					System.activeTextbox = 0;
					System.disconnect();
				}
			}
		}
		else
		{
			labelStatus->updateText("Server is offline. Please try again later!");
			newAccountName = "";
			newEmail = "";
			newPassword = "";
			newRepeatPassword = "";
			System.activeTextbox = 0;
			System.disconnect();
		}
	}
}

void Menu::loginAccount()
{
	sf::Packet toSend;
	sf::Packet toReceive;

	int id = 1337;
	int recvid = -1;
	std::string message = System.clientVersion;
	std::string recvmsg = "";

	if(System.connect())
	{
		toSend << id << message;

		System.sClient.send(toSend);

		System.sClient.receive(toReceive);
		toReceive >> recvid;

		if(recvid != 1337)
		{
			toReceive >> recvmsg;
			
			newAccountName = "";
			newEmail = "";
			newPassword = "";
			newRepeatPassword = "";
			System.activeTextbox = 0;
			labelStatus->updateText(recvmsg);
			System.disconnect();
		}
		else
		{
			bool success;
			toSend.clear();
			id = 1;
			toSend << id << existingAccountName << existingPassword;
			System.sClient.send(toSend);

			System.sClient.receive(toReceive);
			toReceive >> recvid >> success;
			
			if(recvid == 1 && success)
			{
				labelStatus->updateText("Account logged. Select or make new character to play.");
				existingAccountName = "";
				existingPassword = "";
				System.activeTextbox = 0;
				GuiManager.erase = true;
				Menu::isAccountLogged = true;
				
				int incrementer = 0;
				int charCount = 0;
				toReceive >> charCount;

				GuiManager.elements.push_back(new GuiLabel(20,410,"Create new character:",317,true));
				GuiManager.elements.push_back(new GuiTextBoxLabel(20,440,"Name",false,newAccountName));
				GuiManager.elements.push_back(new GuiButton(140,560,"Proceed",80,true,&Menu::createNewPlayer));
				GuiManager.elements.push_back(new GuiLabel(463,410,"Select one character to login.",317,true));

				while(charCount > 0)
				{
					toReceive >> message;
					characters.push_back(message);
					selectedCharacters.push_back(false);
					GuiManager.elements.push_back(new GuiCheckBoxLabel(463,440+incrementer,message,selectedCharacters.at(selectedCharacters.size()-1)));
					incrementer+=20;
					charCount--;
				}
				GuiManager.elements.push_back(new GuiButton(575,560,"Proceed",80,true,&Menu::loginPlayer));
			}
			else
			{
				toReceive >> message;
				System.disconnect();
				labelStatus->updateText(message);
				existingAccountName = "";
				existingPassword = "";
				System.activeTextbox = 0;
			}
		}
	}
	else
	{
		labelStatus->updateText("Server is offline. Please try again later!");
		existingAccountName = "";
		existingPassword = "";
		System.activeTextbox = 0;
		System.disconnect();	
	}
}

void Menu::createNewPlayer()
{
	sf::Packet toSend;
	sf::Packet toReceive;

	int id = 4;
	int recvid = -1;
	std::string message = "";
	std::string recvmsg = "";
	bool success;
	bool proceed = true;

	if(characters.size() > 4) 
	{
		labelStatus->updateText("You can create only 5 characters per account.");
		proceed = false;
	}
	if(newAccountName.size() < 3) 
	{
		labelStatus->updateText("Name must have at least 3 characters.");
		proceed = false;
	}
	if(newAccountName.size() > 15) 
	{
		labelStatus->updateText("Name is too long.");
		proceed = false;
	}
	if(!isLettersOnly(newAccountName))
	{
		labelStatus->updateText("Name contains non-letter characters.");
		proceed = false;
	}
	if((newAccountName.find("GM ") != string::npos) || (newAccountName.find("GOD ") != string::npos) 
	|| (newAccountName.find("Gm ") != string::npos) || (newAccountName.find("God ") != string::npos) 
	|| (newAccountName.find("GoD ") != string::npos) || (newAccountName.find("gm ") != string::npos) 
	|| (newAccountName.find("god ") != string::npos) || (newAccountName.find("Admin ") != string::npos)
	|| (newAccountName.find("admin ") != string::npos) || (newAccountName.find("ADMIN ") != string::npos)) 
	{
		labelStatus->updateText("Name contains prohibited symbols.");
		proceed = false;
	}

	if(proceed)
	{
		toSend << id << newAccountName;

		System.sClient.send(toSend);

		System.sClient.receive(toReceive);
		toReceive >> recvid >> success;

		if(recvid != 4 || !success)
		{		
			newAccountName = "";
			labelStatus->updateText("Can't create player. Try again.");
		}
		else if(recvid == 4 && success)
		{		
			labelStatus->updateText("Player succesfully created.");
			characters.push_back(newAccountName);
			selectedCharacters.push_back(false);
			GuiManager.elements.push_back(new GuiCheckBoxLabel(463,440+(selectedCharacters.size()-1)*20,newAccountName,selectedCharacters.at(selectedCharacters.size()-1)));
			newAccountName = "";
		}
	}
}

void Menu::loginPlayer()
{
	sf::Packet toSend;
	sf::Packet toReceive;

	int id = 3;
	int recvid = -1;
	std::string recvmsg = "";
	bool success;

	unsigned int selectedCharactersCount = 0;

	for(unsigned int i = 0; i < selectedCharacters.size(); i++)
	{
		if(selectedCharacters.at(i) == true)
		{
			recvmsg = characters.at(i);
			selectedCharactersCount++;
		}
	}

	if(selectedCharactersCount < 1)
		labelStatus->updateText("Please select character to login.");
	else if(selectedCharactersCount > 1)
		labelStatus->updateText("Please select ONE character to login.");
	else
	{
		toSend << id << recvmsg;

		System.sClient.send(toSend);

		System.sClient.receive(toReceive);
		toReceive >> recvid >> success;

		if(recvid == 3 && success)
		{
			delete labelStatus;
			labelStatus = NULL;
			//GuiManager.eraseAll;
			System.gameState = 1;
		}
		else
		{
			toReceive >> recvmsg;
			labelStatus->updateText(recvmsg);
		} 
	}
}

void Menu::addButton(int posx, int posy, std::string label, int size, bool center, void (*function)())
{
	GuiManager.elements.push_back(new GuiButton(posx, posy, label, size, center, function));
}

void Menu::addLabel(int posx, int posy, std::string label, int size, bool center)
{
	GuiManager.elements.push_back(new GuiLabel(posx, posy, label, size, center));
}

void Menu::addCheckBoxLabel(int posx, int posy, std::string label, bool &Pcallback)
{
	GuiManager.elements.push_back(new GuiCheckBoxLabel(posx, posy, label, Pcallback));
}

void Menu::addTextBoxLabel(int posx, int posy, std::string label, bool password, std::string &text)
{
	GuiManager.elements.push_back(new GuiTextBoxLabel(posx, posy, label, password, text));
}

void Menu::addNumberBoxLabel(int posx, int posy, std::string label, bool password, int32_t &number)
{
	GuiManager.elements.push_back(new GuiNumberBoxLabel(posx, posy, label, password, number));
}

#endif