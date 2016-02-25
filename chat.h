#ifndef CHAT_H
#define CHAT_H

#include <SFML/System.hpp>
#include <deque>

using namespace std;

class ChatChannel
{
public:
	ChatChannel(string name, bool _default);
	~ChatChannel() {};
	string getName() { return name; };
	unsigned int getMessagesNumber() { return messages.size(); };
	void clear() { messages.clear(); };
	void render();
	void setPosition(float x, float y) { sfName.setPosition(x, y); };
	void setActive(bool active) { if(active) sfName.setColor(sf::Color::Blue); else sfName.setColor(sf::Color::White); };
	bool isDefault();
	void addMessage(string message, sf::Color color);
	sf::Text* getMessage(unsigned int pos);
private:
	bool mDefault;
	deque<sf::Text> messages;
	string name;
	sf::Text sfName;
};

class Chat
{
public:
  Chat();
  ~Chat();
  void setPos(int _posx, int _posy, int _maxx, int _maxy);
  void receiveMessage(sf::Packet &message);
  void addMessage(unsigned char type, string message, string sender = "");
  void render();
  bool mouse(unsigned char type, int mouse_x, int mouse_y);
  bool proceedSendMessage();
  void sendMessage();
  void updateToSend(sf::Uint32 unicode);
  void updateNewConversation(sf::Uint32 unicode);
  void setLastSended();
  void nextActiveTab();

  int posx, posy, maxx, maxy;
  bool newConversation;
private:
  deque<ChatChannel> channels;
  int currentChannel;
  sf::RectangleShape mainRect;
  sf::RectangleShape textBox;
  sf::RectangleShape messagersShape;
  sf::Sprite channelsWindowSprite;
  sf::Texture channelsWindow;
  int channelsCaret;
  string::iterator channelsIter;
  string channelsCallback;
  sf::Text sfChannelsCallback;

  string::iterator iter;
  int caret;
  bool insert; 
  std::string callback;
  sf::Text callbackSF;
  sf::RectangleShape rectangle;
  clock_t blinking;
  clock_t canSend;

  string lastSended;
  unsigned int activeChannel;

  sf::Text serverMessage;
  sf::RectangleShape serverMessageShape;
  clock_t serverMessageReceivedTime;
};

#endif

