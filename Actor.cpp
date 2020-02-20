#include "Actor.h"
#include <math.h>
#include "StudentWorld.h"
#include <iostream>
using namespace std;


///////////////
//ACTOR CLASS//
///////////////
double Actor::radialDistance(const Actor* first, const Actor* second) const
{
	double firstx = first->getX(), firsty = first->getY(),
		secondx = second->getX(), secondy = second->getY();

	//Use pythagorean's theorem to find the distance between 2 points. 
	double xDist = secondx - firstx;
	double yDist = secondy - firsty;
	return sqrt(xDist * xDist + yDist * yDist);
}

double Actor::radialDistance(const double& x, const double& y) const
{
	double thisX = getX(), thisY = getY(); 
	double xDist = x - thisX; 
	double yDist = y - thisY; 
	return sqrt(xDist * xDist + yDist * yDist); 
}

////////////////////////////////////
//ACTOR PROTECTED HELPER FUNCTIONS//
////////////////////////////////////


bool Actor::overlaps(const Actor* first, const Actor* second) const
{
	if (radialDistance(first, second) <= 2.0 * SPRITE_RADIUS)
		return true;
	return false;
}

bool Actor::overlaps(const double& x, const double& y) const
{
	if (radialDistance(x, y) <= 2.0 * SPRITE_RADIUS)
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
//PICKUPS CLASS//
////////////////

//Pickups will do their thing if need be. 
void Pickups::doSomething()
{
	//Kills goodie/fungus if dead
	m_lifetime--; 
	if (m_lifetime <= 0)
		kill(); 
	if (!alive())
		return; 
	if (!socOverlap())
		return;
	overlapAction(nullptr);
}
///////////////////////////////
//PICKUPS CLASS PROTECTED FUNC//
///////////////////////////////
//Determines if Pickup overlaps with Socrates
bool Pickups::socOverlap() const
{
	if (overlaps(this, m_socrates))
		return true;
	return false; 
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
	myWorld()->increaseScore(-50);
	kill();
	mySoc()->takeDamage(20); 
}

/////////////
//PIT CLASS//
/////////////
void Pit::doSomething()
{
	if (!anyRegSalmon() && !anyAggSalmon() && !anyEColi())
	{
		kill(); 
		return; 
	}
	//TODO: ADD AN IF STATEMENT TO STUDENTWORLD. IF IT'S KILLING A PIT, DECREMENT # OF PITS LEFT! 

	int RNG = randInt(0, 49); //1 in 50 chance
	const int regSalmon = 1; 
	const int aggSalmon = 2; 
	const int eColi = 3; 
	if (RNG == 0)
	{
		int whichOne; 
		//Randomly picks a bacteria to spawn, if it is available to spawn. 
		for (;;)
		{
			bool shouldBreak = false; 
			whichOne = randInt(1, 3); 
			switch (whichOne)
			{
			case regSalmon: 
			{
				if (anyRegSalmon())
				{
					shouldBreak = true; 
					//TODO: Then spawn salmon...
					m_numRegSalmon--; 
					myWorld()->playSound(SOUND_BACTERIUM_BORN); 
				}
			}
				break; 
			case aggSalmon:
			{
				if (anyAggSalmon())
				{
					shouldBreak = true;
					//TODO: Then spawn agg salmon..
					m_numAggSalmon--; 
					myWorld()->playSound(SOUND_BACTERIUM_BORN);
				}
			}
				break;
			case eColi:
			{
				if (anyEColi())
				{
					shouldBreak = true;
					//TODO: Then spawn ecoli salmon..
					m_numEColi--; 
					myWorld()->playSound(SOUND_BACTERIUM_BORN);
				}
			}
				break; 
			}
			if (shouldBreak)
				break; 
		}
	}

}

/////////////////
//WEAPONS CLASS//
/////////////////
void Weapons::overlapAction(Actor* other)
{
	if (other->canBeDamaged())
	{
		if (other->hasHP())
			other->takeDamage(m_damageAmt);
		else //Objects that can be damaged, but don't have HP, die immediately upon contact. 
			other->kill(); 
		kill(); //this weapon also dies upon damaging another. 
	}
}

void Weapons::doSomething()
{
	myWorld()->//maybe use the socOverlap concept from your pickups class. but make it a damageableObject overlap. 
}

