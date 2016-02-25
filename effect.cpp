//#include "system.h"
#include "effect.h"
#include "system.h"

extern System System;

Effect::Effect(unsigned char _type) //default constor, type - type of effect, for example fire explosion effect
{
     type = _type;
     started = clock();

	this->light = new sf::Sprite();
	this->light->setTexture(*System.lightObjectTexture);
	this->light->setScale(3.0f, 3.0f);
	this->light->setPosition(0.0f, 0.0f);

	if(type == 1)
		this->light->setColor(sf::Color(255, 0, 0, 255));
	else
		this->light->setColor(sf::Color(0, 0, 0, 255));
}

unsigned int Effect::getSpriteNumber() //function returns current sprite number to display, if effect is over, funcion returns 0
{
	if( type == 1 ) //fire
	{
         if(started + 100 > clock())
             return 384;   
         else if(started + 200 > clock())
             return 385; 
         else if(started + 300 > clock())
             return 386;
         else if(started + 400 > clock())
             return 384; 
         else if(started + 500 > clock())
             return 385;
		 else
			 return 0;
	}
	if( type == 2 ) //blocked hit
	{
         if(started + 100 > clock())
             return 212;   
         else if( started + 200 > clock())
             return 213; 
         else if(started + 300 > clock())
             return 214;
		 else
			 return 0;
	}
	if( type == 3 ) //blood hit
	{
         if(started + 100 > clock())
             return 210;   
         else if( started + 200 > clock())
             return 211; 
		 else
			 return 0;
	}
	if( type == 4 ) //teleport
	{
         if(started + 100 > clock())
             return 215;   
         else if( started + 200 > clock())
             return 216; 
         else if(started + 300 > clock())
             return 217;
         else if( started + 400 > clock())
             return 216; 
         else if(started + 500 > clock())
             return 215;
		 else
			 return 0;
	}
	if( type == 5 ) //energy ball
	{
         if(started + 100 > clock())
             return 557;   
         else if( started + 200 > clock())
             return 558; 
         else if(started + 300 > clock())
             return 559;
         else if( started + 400 > clock())
             return 560; 
         else if(started + 500 > clock())
             return 561;
         else if(started + 600 > clock())
             return 562;
         else if( started + 700 > clock())
             return 563; 
         else if(started + 800 > clock())
             return 564;
		 else
			 return 0;
	}
	if( type == 6 ) //energy storm
	{
         if(started + 75 > clock())
             return 565;   
         else if( started + 150 > clock())
             return 566; 
         else if(started + 225 > clock())
             return 567;
         else if( started + 300 > clock())
             return 568; 
         else if(started + 375 > clock())
             return 569;
         else if(started + 450 > clock())
             return 570;
         else if( started + 525 > clock())
             return 571; 
         else if(started + 600 > clock())
             return 572;
         else if(started + 675 > clock())
             return 573;
         else if(started + 750 > clock())
             return 574;
         else if( started + 825 > clock())
             return 575; 
         else if(started + 900 > clock())
             return 576;
         else if( started + 975 > clock())
             return 577; 
         else if(started + 1050 > clock())
             return 578;
		 else
			 return 0;
	}
	if( type == 7 ) //energy bomb
	{
         if(started + 75 > clock())
             return 579;   
         else if( started + 150 > clock())
             return 580; 
         else if(started + 225 > clock())
             return 581;
         else if( started + 300 > clock())
             return 582; 
         else if(started + 375 > clock())
             return 583;
         else if(started + 450 > clock())
             return 584;
         else if( started + 525 > clock())
             return 585; 
         else if(started + 600 > clock())
             return 586;
         else if(started + 675 > clock())
             return 587;
         else if(started + 750 > clock())
             return 588;
         else if( started + 825 > clock())
             return 589; 
         else if(started + 900 > clock())
             return 590;
         else if( started + 975 > clock())
             return 591; 
         else if(started + 1050 > clock())
             return 592;
         else if( started + 1125 > clock())
             return 593; 
         else if(started + 1200 > clock())
             return 594;
		 else
			 return 0;
	}
	if( type == 8 ) //shock wave
	{
         if(started + 100 > clock())
             return 652;   
         else if( started + 200 > clock())
             return 653; 
         else if(started + 300 > clock())
             return 654;
         else if( started + 400 > clock())
             return 655; 
         else if(started + 500 > clock())
             return 656;
		 else
			 return 0;
	}
	if( type == 9 ) //energy strike
	{
         if(started + 75 > clock())
             return 814;   
         else if( started + 150 > clock())
             return 815; 
         else if(started + 225 > clock())
             return 816;
         else if(started + 300 > clock())
             return 814;   
         else if( started + 375 > clock())
             return 815; 
         else if(started + 450 > clock())
             return 816;
		 else
			 return 0;
	}
	if( type == 10 ) //great explosion
	{
         if(started + 100 > clock())
             return 817;   
         else if( started + 200 > clock())
             return 818; 
         else if(started + 300 > clock())
             return 819;
         else if(started + 400 > clock())
             return 820;
         else if(started + 500 > clock())
             return 821;
		 else
			 return 0;
	}
	if( type == 11 ) //armor blocked
	{
         if(started + 75 > clock())
             return 822;   
         else if( started + 150 > clock())
             return 823; 
         else if(started + 225 > clock())
             return 824;
		 else
			 return 0;
	}
	else
		return 0;
}

TextEffect::TextEffect(unsigned char _color, string _text)
{
	color = _color;
	started = clock();
	text = _text;
	sfText = new sf::Text(text, System.font, 11);

	if(color == 1)
		sfText->setColor(sf::Color::White);
	else if(color == 2)
		sfText->setColor(sf::Color::Red);

	modiferX = 0;
	modiferY = 0;
}

int TextEffect::isToDelete()
{
	if(started + 2000 < clock())
		return 1;
	else 
		return 0;
}

void TextEffect::update()
{
	if(color == 1)
	{
         if(started + 200 > clock())
			 modiferY-=1;
         else if( started + 400 > clock())
			 modiferY-=1;
         else if( started + 600 > clock())
			 modiferY-=1;
	}
	if(color == 2)
	{
         if(started + 200 > clock())
			 modiferY-=1;
         else if( started + 400 > clock())
			 modiferY-=1;
         else if( started + 600 > clock())
			 modiferY-=1;
	}
}