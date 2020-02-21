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

double Actor::radialDistance(const Actor *first, const double& x, const double& y) const
{
	double thisX = first->getX(), thisY = first->getY();
	double xDist = x - thisX;
	double yDist = y - thisY;
	return sqrt(xDist * xDist + yDist * yDist);
}

///////////////////////////
//ACTOR  HELPER FUNCTIONS//
///////////////////////////


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

bool Actor::overlaps(const Actor* first, const double& x, const double& y) const
{
	if (radialDistance(first, x, y) <= 2.0 * SPRITE_RADIUS)
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
	bool addSpray = true; 
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
			case KEY_PRESS_SPACE :
			{
				//Put a spray on the screen
				if (m_sprayCharges > 0)
				{
					addSpray = false; 
					double x, y; 
					getPositionInThisDirection(getDirection(), SPRITE_RADIUS * 2, x, y);
					myWorld()->addActor(0, x, y, getDirection());
					m_sprayCharges--; 
					myWorld()->playSound(SOUND_PLAYER_SPRAY); 
				}
				break; 
			}
			case KEY_PRESS_ENTER :
			{
				if (m_flameCharges > 0)
				{
					Direction angle = getDirection(); 
					//Put 16 flames around Socrates in a circle
					for (int i = 0; i < 16; i++, angle+=22)
					{
						double x, y; 
						getPositionInThisDirection(angle, SPRITE_RADIUS * 2, x, y); 
						myWorld()->addActor(1, x, y, angle); 
					}
					m_flameCharges--; 
					myWorld()->playSound(SOUND_PLAYER_FIRE); 
				}
				break; 
			}
		}
	}
	if (addSpray)
	{
		m_sprayCharges++; 
		if (m_sprayCharges > 20)
			m_sprayCharges = 20; 
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

//////////////////////
//LIVINGWITHHP CLASS//
//////////////////////
void LivingWithHP::takeDamage(const int& amt)
{
	m_health -= amt;
	if (m_health <= 0)
	{
		m_health = 0; //Set to 0 for display purposes
		kill();
	}
	if (m_health > 100)
		m_health = 100; //Socrates is the only LivingWithHp that can heal, so this is ok. 
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
	myWorld()->playSound(SOUND_PLAYER_HURT); 
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
					myWorld()->addActor(2, getX(), getY(), 0);
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
					myWorld()->addActor(3, getX(), getY(), 0); 
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
					myWorld()->addActor(4, getX(), getY(), 0);
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
		{
			other->takeDamage(m_damageAmt);
			if (other->canDamageSocrates() && other->blockedByDirt())
			{
				if (other->alive() && other->isSalmon())
					myWorld()->playSound(SOUND_SALMONELLA_HURT);
				else if (other->isSalmon() && !other->alive())
				{
					myWorld()->playSound(SOUND_SALMONELLA_DIE);
					myWorld()->increaseScore(100);
				}
				else if (other->alive() && other->chasesSocrates() && !other->isSalmon())
					myWorld()->playSound(SOUND_ECOLI_HURT);
				else if (other->chasesSocrates() && !other->alive() && !other->isSalmon())
				{
					myWorld()->playSound(SOUND_ECOLI_DIE); 
					myWorld()->increaseScore(100); 
				}
			}
		}
		else //Objects that can be damaged, but don't have HP, die immediately upon contact. 
			other->kill();
		kill(); //this weapon also dies upon damaging another. 
	}
	else
		return; 
}

void Weapons::doSomething()
{
	if (!alive()) //Can't do anything if you're dead !
		return; 
	list<Actor*>::iterator actorItr = myWorld()->myActorsItr(); 
	list<Actor*>::iterator endItr = myWorld()->myActorsEnd();
	//Damages an object if it is able to.
	while (actorItr != endItr)
	{
		if (overlaps(this, *actorItr) && (*actorItr)->canBeDamaged())
		{
			overlapAction(*actorItr);
			break; 
		}
		actorItr++; 
	}

	moveAngle(getDirection(), SPRITE_RADIUS * 2); 

	if (checkMaxDist())
		kill(); 
}

//////////////////
//BACTERIA CLASS//
//////////////////

//////////////////
//BACTERIA CLASS//
//////////////////

//All types of bacteria have a very similar overlapAction (damage Socrates)
void Bacteria::overlapAction(Actor* other)
{
	myWorld()->mySoc()->takeDamage(m_dmgAmt);
	if (myWorld()->mySoc()->alive())
		myWorld()->playSound(SOUND_PLAYER_HURT);
	else
		myWorld()->playSound(SOUND_PLAYER_DIE);
}

void Salmonella::doSomething()
{
	if (!alive())
	{
		return;
	}
	if (overlaps(this, myWorld()->mySoc()))
	{
		overlapAction(nullptr);
		//Then skip to step 5
	}
	else
	{
		checkFood();
	}
	//This is step 5
	if (getMPD() > 0)
	{
		movementPlanDistance();
	}
	else
	{
		getClosestFood();
	}
}

void Bacteria::spawnSelf()
{
	double newX = getX(), newY = getY();
	//3 is Salmonella
	//damages socrates, blocked by dirt, doesn't chase socrates
	//4 is aggressive salmonella
	//If it is neither salmonella or ecoli, it is aggressive salmonella.
	//5 is Ecoli
	//Chases socrates, blocked by dirt, damages socrates

	int type;
	if (isSalmon() && !chasesSocrates())
	{
		type = 2;
	}
	else if (!isSalmon() && chasesSocrates())
	{
		type = 4;
	}
	else
	{
		type = 3;
	}
	if (getX() < VIEW_WIDTH / 2)
		newX += SPRITE_RADIUS;
	else
		newX -= SPRITE_RADIUS;

	if (getY() < VIEW_WIDTH / 2)
		newY += SPRITE_RADIUS;
	else
		newY -= SPRITE_RADIUS;
	myWorld()->addActor(type, newX, newY, 90);
}


void Bacteria::overlapsFood()
{
	list<Actor*>::iterator beginItr = myWorld()->myActorsItr();
	list<Actor*>::iterator endItr = myWorld()->myActorsEnd();

	while (beginItr != endItr)
	{
		if ((*beginItr)->isBacteriaInteractable() && !(*beginItr)->usedBySocrates()) //Identify if food
		{
			if (overlaps(this, (*beginItr)))
			{
				m_foodEaten++;
				(*beginItr)->kill();
				break;
			}
		}
		beginItr++;
	}
}

//Return true if failed to move
bool Bacteria::movementPlanDistance()
{
	bool canMove = true;
	if (m_MPD > 0)
	{
		m_MPD--;
	}
	double newx, newy;
	double tempX = getX(), tempY = getY(); 
	if (!insideCircle(newx, newy))
	{
		moveTo(tempX, tempY); 
		canMove = false; 
	}

	if (canMove)
	{
		list<Actor*>::iterator beginItr = myWorld()->myActorsItr();
		list<Actor*>::iterator endItr = myWorld()->myActorsEnd();

		//Check if blocked by any dirt 
		while (beginItr != endItr)
		{
			if ((*beginItr)->canBeDamaged() && (*beginItr)->blockOtherObjects()) //Identify if dirt
			{
				double dirtX = (*beginItr)->getX();
				double dirtY = (*beginItr)->getY();
				if (radialDistance(*beginItr, newx, newy) <= SPRITE_WIDTH / 2) //Movement overlap
				{
					//Blocked by dirt, Don't move to this pos
					canMove = false;
				}
			}
			beginItr++;
		}
	}

	if (canMove)
	{
		return false; 
	}
	else
	{
		failedToMove();
		return true;
	}
	return false;
}

//Return true if failed to move
bool Bacteria::getClosestFood()
{
	double closestDistance = 128;
	Actor* closest;
	list<Actor*>::iterator beginItr = myWorld()->myActorsItr();
	list<Actor*>::iterator endItr = myWorld()->myActorsEnd();

	//Compare distance of all foods within 120 pixels.
	//Unfortunately.. selection sort is the easiest to implement here
	while (beginItr != endItr)
	{
		if ((*beginItr)->isBacteriaInteractable() && !(*beginItr)->usedBySocrates()
			&& radialDistance(this, *beginItr) < closestDistance) //Identify if food, and if it's within 128 pixels
		{
			closestDistance = radialDistance(this, *beginItr);
			closest = *beginItr;
		}
		beginItr++;
	}
	if (closestDistance == 128) //No food nearby
	{
		failedToMove();
		return true;
	}
	else
	{
		bool canMove = true;
		double moveToX = closest->getX();
		double moveToY = closest->getY();
		//Compute angle between the points 
		double angle = angleInDeg(moveToX, moveToY); 
		setDirection(angle);

		double newx, newy;
		if (!insideCircle(newx, newy))
			canMove = false;

		//if it gets blocked by dirt, random position.
		list<Actor*>::iterator beginItr = myWorld()->myActorsItr();
		list<Actor*>::iterator endItr = myWorld()->myActorsEnd();

		//Check if blocked by any dirt 
		while (beginItr != endItr)
		{
			if ((*beginItr)->canBeDamaged() && (*beginItr)->blockOtherObjects()) //Identify if dirt
			{
				if (radialDistance(*beginItr, newx, newy) <= SPRITE_WIDTH / 2)
				{
					//Blocked by dirt, can't move.
					canMove = false;
					break;
				}
			}
			beginItr++;
		}

		if (!canMove)
		{
			failedToMove();
			return true;
		}
		else
			moveTo(newx, newy);
	}
	return false;

}

void Bacteria::failedToMove()
{
	int randAngle = randInt(0, 359);
	setDirection(randAngle);
	m_MPD = 10;
}

bool Bacteria::insideCircle(double& newX, double& newY)
{
	double tempX = getX(), tempY = getY();
	getPositionInThisDirection(getDirection(), m_movePixels, newX, newY);

	//This actually moves it.
	moveTo(newX, newY);

	//Use this to check if the REAL bacteria would go somewhere it shouldn't. 

	//Check if blocked by radius 
	if (radialDistance(this, VIEW_WIDTH / 2, VIEW_HEIGHT / 2) >= VIEW_RADIUS)
	{
		moveTo(tempX, tempY); 
		return false;
	}
	return true;
}

//Return true only if got stuck on dirt
bool Bacteria::findSocrates(int dist)
{
	bool canMove = true;
	int distance = dist; 
	if (radialDistance(myWorld()->mySoc(), getX(), getY()) <= dist)
	{
		//Get angle to Socrates
		double socX = myWorld()->mySoc()->getX(); 
		double socY = myWorld()->mySoc()->getY(); 
		double angle = angleInDeg(socX, socY); 
		list<Actor*>::iterator beginItr = myWorld()->myActorsItr();
		list<Actor*>::iterator endItr = myWorld()->myActorsEnd();
		Direction temp = getDirection(); 
		setDirection(angle); 
		double newx, newy;
		double tempX = getX(), tempY = getY();
		if (!insideCircle(newx, newy))
		{
			setDirection(temp); 
			moveTo(tempX, tempY);
			canMove = false;
		}

		//If it can move.. first check if it would be blocked by dirt
		if (canMove)
		{

			while (beginItr != endItr)
			{
				if ((*beginItr)->canBeDamaged() && (*beginItr)->blockOtherObjects()) //Identify if dirt
				{
					if (radialDistance(*beginItr, getX(), getY()) <= SPRITE_WIDTH / 2)
					{
						moveTo(tempX, tempY); 
						return true;
					}
				}
				beginItr++;
			}
		}
		return false;
	}
	return false; 
}


////////////////////
//AGGRO SALMONELLA//
////////////////////
void AggroSalmonella::doSomething()
{
	if (!alive())
		return;
	bool stop = false;
	if (findSocrates(72)) //Stuck on dirt
	{
		stop = true;
	}
	if (overlaps(this, myWorld()->mySoc()))
	{
		overlapAction(nullptr);
	}
	else
	{
		checkFood();
	}
	if (stop)
	{
		return;
	}
	if (getMPD() > 0)
	{
		movementPlanDistance();
	}
	else
	{
		getClosestFood();
	}

}

void Bacteria::checkFood()
{
	if (foodEaten() == 3)
	{
		spawnSelf();
		myWorld()->playSound(SOUND_BACTERIUM_BORN); 
		resetFood();
	}
	else
	{
		overlapsFood();
	}

}

void EColi::doSomething()
{
	if (!alive())
		return;
	if (overlaps(this, myWorld()->mySoc()))
	{
		overlapAction(nullptr);
	}
	else
	{
		checkFood();
	}
	int i = 0;
	int tempX = getX(), tempY = getY(); 
	while (!findSocrates(256) && i < 10)
	{
		moveTo(tempX, tempY); 
		setDirection(getDirection() + 10);
		i++;
	}

}
//Compute angle between this and the given coords
double Bacteria::angleInDeg(const double& otherx, const double& othery)
{
	const double PI = 4 * atan(1);
	double yDist = othery - VIEW_RADIUS;
	double xDist = otherx - VIEW_RADIUS;
	//Compute angle between the points 
	double angle = atan2(yDist, xDist);
	angle = angle * 180 / PI;
	if (angle < 0)
		angle += 360;
	return angle;
}