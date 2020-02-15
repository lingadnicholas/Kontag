#ifndef ACTOR_H_
#define ACTOR_H_

#include "GraphObject.h"
class StudentWorld; 

class Actor : public GraphObject
{
public:
	Actor(StudentWorld* swptr, int imageID, double startX, double startY, Direction dir = 0, int depth = 0, double size = 1.0)
		: GraphObject(imageID, startX, startY, dir, depth, size), m_alive(true), m_studentworld(swptr) 
	{
	}

	//Pure virtual functions	
	virtual void doSomething() = 0; 
	virtual void blockObjects() = 0;
	virtual void overlapAction() = 0; 


	virtual bool canBeDamaged() const
	{
		return true; 
	}

	//Returns true if an actor is alive
	virtual bool alive() const
	{
		return m_alive; 
	}

	

	virtual ~Actor()
	{

	}

private: 
	bool m_alive;
	StudentWorld* m_studentworld;
};

class Dirt: public Actor
{
public: 
	Dirt(StudentWorld* swptr, double startX, double startY)
		: Actor(swptr, IID_DIRT, startX, startY, 90, 1)
	{

	}
	virtual void doSomething()
	{
		return; //Dirt can't do anything! 
	}
	virtual void blockObjects()
	{

	}
	virtual void overlapAction()
	{

	}
	//Blocks bacteria, spray, flames; dies from contact with spray or flames
	//See pg 28 of spec
private: 
};

class LivingWithHP : public Actor
{
public: 
	LivingWithHP(StudentWorld* swptr, int hp, int imageID, double startX, double startY, Direction dir = 0, int depth = 0, double size = 1.0)
		:Actor(swptr, imageID, startX, startY, dir, depth, size), m_health(hp)
	{

	}
	virtual void takeDamage(const int& amt)
	{
		m_health -= amt;
	}
private: 
	int m_health; 

};

//INCOMPLETE... NEED ACCESS TO STUDENTWORLD!
class Socrates : public LivingWithHP
{
public:
	Socrates(StudentWorld* swptr)
		:LivingWithHP(swptr, 100, IID_PLAYER, 0, VIEW_HEIGHT/2, 0, 0)
	{
	}
	virtual void doSomething();
	virtual void blockObjects()
	{

	}
	virtual void overlapAction()
	{

	}
private: 
	int m_sprayCharges = 20; 
	int m_flameCharges = 5; 
};

// Students:  Add code to this file, Actor.cpp, StudentWorld.h, and StudentWorld.cpp

#endif // ACTOR_H_
