#include "Actor.h"
#include <math.h>
#include "StudentWorld.h"
#include <iostream>
using namespace std;


///////////////
//ACTOR CLASS//
///////////////
////////////////////////////////////
//ACTOR PROTECTED HELPER FUNCTIONS//
////////////////////////////////////
double Actor::radialDistance(const Actor* first, const Actor* second) const
{
	double firstx = first->getX(), firsty = first->getY(),
		secondx = second->getX(), secondy = second->getY();

	//Use pythagorean's theorem to find the distance between 2 points. 
	double xDist = secondx - firstx;
	double yDist = secondy - firsty;
	return sqrt(xDist * xDist + yDist * yDist);
}

bool Actor::overlaps(const Actor* first, const Actor* second) const
{
	if (radialDistance(first, second) <= 2.0 * SPRITE_RADIUS)
		return true;
	return false;
}

//////////////
//DIRT CLASS//
//////////////
bool Dirt::blockObjects(const Actor* other)
{
	//If another object is close enough to dirt, it should get blocked. 
	if (radialDistance(this, other) < SPRITE_RADIUS && other->blockedByDirt())
		return true; 
	return false; 
}

//If it comes in contact with spray or flame, destroy. 

void Dirt::overlapAction(Actor* other)
{
	if (overlaps(this, other) && other->usedBySocrates())
		kill(); 
}


//////////////////
//SOCRATES CLASS//
//////////////////

void Socrates::doSomething()
{
	if (!alive())
		return; 
	int ch;
	if (myWorld()->getKey(ch))
	{
		switch (ch)
		{
			case KEY_PRESS_RIGHT: //Move counterclockwise
			{
				changePosition(0); 
				break;
			}
			case KEY_PRESS_LEFT: //Move clockwise
			{
				changePosition(1); 
				break;
			}
		}
	}
}

/////////////////////////////////////
//SOCRATES PRIVATE HELPER FUNCTIONS//
/////////////////////////////////////
void Socrates::changePosition(int dir)
{
	const double PI = 4 * atan(1);
	//0 is clockwise
	//1 is counterclockwise 

	double amt;
	if (dir == 0)
		amt = -5;
	else
		amt = 5; 

	changeCurrentAngle(amt); 
	double angleInRad = getCurrentAngle() * PI / 180;
	double moveToX = VIEW_RADIUS + VIEW_RADIUS * cos(angleInRad);
	double moveToY = VIEW_RADIUS + VIEW_RADIUS * sin(angleInRad);
	moveTo(moveToX, moveToY); 
	setDirection(getCurrentAngle() + 180); 
}

////////////////
//GOODIE CLASS//
////////////////

//Pickups will do their thing if need be. 
void Pickups::doSomething()
{
	if (!socOverlap())
		return;
	overlapAction(nullptr);
	killIfDead();
}
///////////////////////////////
//GOODIE CLASS PROTECTED FUNC//
///////////////////////////////
//Determines if Pickup overlaps with Socrates
bool Pickups::socOverlap() const
{
	if (overlaps(this, m_socrates))
		return true;
	return false; 
}

void Pickups::killIfDead()
{
	if (m_lifetime <= 0)
		kill(); 
	return; 
}

///////////////////////////////
//RESTORE HEALTH GOODIE CLASS//
///////////////////////////////

//When Socrates touches a restore health goodie, 
//Update the score
//Set goodie to dead and play sound
//Set Socrates' HP to full. 
void ResHealth::overlapAction(Actor* other)
{
	killIfDead(); 
	if (!alive())
		return; 
	myWorld()->increaseScore(250); 
	kill(); 
	myWorld()->playSound(SOUND_GOT_GOODIE); 
	int maxHP = mySoc()->maxSocHP(); 
	int curHP = mySoc()->hp(); 
	int amt = (maxHP - curHP) * -1; //Negative because we want to restore health. 
	mySoc()->takeDamage(amt); 
	return;
}

//////////////////////////////
//RESTORE FLAME GOODIE CLASS//
//////////////////////////////

//When Socrates touches a flame goodie,
//Update score and add 5 flame charges
void ResFlame::overlapAction(Actor* other)
{
	killIfDead();
	if (!alive())
		return;
	myWorld()->increaseScore(300);
	kill();
	myWorld()->playSound(SOUND_GOT_GOODIE); 
	mySoc()->updateFlame(5); 
}

///////////////////////////
//EXTRA LIFE GOODIE CLASS//
///////////////////////////
//When socrates touches an extra life goodie,
//Update score and add 1 life
void ResLife::overlapAction(Actor* other)
{
	killIfDead();
	if (!alive())
		return;
	myWorld()->increaseScore(500);
	kill();
	myWorld()->playSound(SOUND_GOT_GOODIE);
	myWorld()->incLives(); 
}

///////////////////////
//FUNGUS GOODIE CLASS//
///////////////////////
//When socrates touches a fungus
//Update score and damage him
void Fungus::overlapAction(Actor* other)
{
	killIfDead();
	if (!alive())
		return;
	myWorld()->increaseScore(-50);
	kill();
	mySoc()->takeDamage(20); 
}
