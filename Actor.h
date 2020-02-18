#ifndef ACTOR_H_
#define ACTOR_H_

#include "GraphObject.h"
#include <cmath>
class StudentWorld; 
class Socrates; 
//Reminder to me:
/*
For a class to NOT be pure virtual.. you must define
doSomething()
overlapAction()
*/
///////////////
//ACTOR CLASS//
///////////////
class Actor : public GraphObject
{
public:
	Actor(StudentWorld* swptr, int imageID, double startX, double startY, Direction dir = 0, int depth = 0, double size = 1.0)
		: GraphObject(imageID, startX, startY, dir, depth, size), m_alive(true), m_studentworld(swptr) 
	{
	}

	//Pure virtual functions	
	virtual void doSomething() = 0; 

	//Virtual functions:

	//Determines if an object can take damage. 
	virtual bool canBeDamaged() const
	{
		return true; 
	}

	//Determines if an object is a weapon. 
	virtual bool usedBySocrates() const
	{
		return false; 
	}

	//Determines if an object is blocked by dirt
	virtual bool blockedByDirt() const
	{
		return false; 
	}

	virtual ~Actor()
	{

	}

	//Other public member functions:
	//Sets alive status to false 
	void kill()
	{
		m_alive = false;
	}

	//Returns true if an actor is alive
	bool alive() const
	{
		return m_alive;
	}


	//Returns a pointer to the StudentWorld it is associated with. 
	StudentWorld* myWorld() const
	{
		return m_studentworld; 
	}

protected:
	//PROTECTED HELPER FUNCTIONS
	double radialDistance(const Actor* first, const Actor* second) const;
	bool overlaps(const Actor* first, const Actor* second) const;



	//Protected, because only used by objects derived from this class. 
	virtual void overlapAction(Actor* other) = 0;

private: 
	bool m_alive;
	StudentWorld* m_studentworld;
};

	//////////////
	//DIRT CLASS//
	//////////////
	class Dirt: public Actor
	{
	public: 
		Dirt(StudentWorld* swptr, double startX, double startY)
			: Actor(swptr, IID_DIRT, startX, startY, 0, 1) //Specs say 90 degrees.. but this is what looks right 
		{

		}

		//Dirt does not do anything per tick.
		virtual void doSomething()
		{
			return; //Dirt can't do anything! 
		}

		//Dirt cannot take damage, but they can still be killed. 
		virtual bool canBeDamaged() const
		{
			return false;
		}

		//Dirt is set to dead if it comes into contact with spray or flame. 
		virtual void overlapAction(Actor* other);

		//Returns true if the dirt should block the other object (if it gets too close!) 
		bool blockObjects(const Actor* other);
	
	private: 
	};

	//////////////////////
	//LIVINGWITHHP CLASS//
	//////////////////////
	class LivingWithHP : public Actor
	{
	public: 
		LivingWithHP(StudentWorld* swptr, int hp, int imageID, double startX, double startY, Direction dir = 0, int depth = 0, double size = 1.0)
			:Actor(swptr, imageID, startX, startY, dir, depth, size), m_health(hp)
		{

		}

		//Allows units with HP to take damage (or heal!) 
		virtual void takeDamage(const int& amt)
		{
			m_health -= amt;
		}

		//Returns current health 
		int hp() const 
		{
			return m_health; 
		}
		virtual ~LivingWithHP()
		{

		}

	private: 
		int m_health; 

	};
		//////////////////
		//SOCRATES CLASS//
		//////////////////
		class Socrates : public LivingWithHP
		{
		public:
			Socrates(StudentWorld* swptr)
				:LivingWithHP(swptr, m_maxHP, IID_PLAYER, 0, VIEW_HEIGHT/2, 0, 0)
			{
			}
			//Socrates performs actions based on player input
			virtual void doSomething(); 

			//Changes amount of spray Charges
			void updateSpray(int amt)
			{
				m_sprayCharges += amt; 
			}

			//Changes amount of flame charges
			void updateFlame(int amt)
			{
				m_flameCharges += amt;
			}

			//Gets current angle so Socrates can move. 
			double getCurrentAngle() const
			{
				return m_positionAngle;
			}

			//Used for movement, changes angle
			void changeCurrentAngle(const double& amt)
			{
				m_positionAngle += amt; 
			}

			//Returns max HP
			int maxSocHP() const
			{
				return m_maxHP; 
			}

		protected: 
			//TODO: Incomplete, but needs to be here to not be pure virtual. 
			virtual void overlapAction(Actor* other)
			{
				//This actually may not need to do anything.. but not 100% sure
			}
		private: 
			int m_sprayCharges = 20; 
			int m_flameCharges = 5; 
			int m_positionAngle = 180; 
			int m_maxHP = 100; 
			void changePosition(int dir); 
		};

	/////////////////
	//PICKUPS CLASS//
	/////////////////

		//A pickup is anything that Socrates can "pick up"
		//Includes all goodies and fungus, because fungus behaves very closely to goodies. 
	class Pickups : public Actor
	{
	public:
		Pickups(StudentWorld* swptr, Socrates* socptr, int lifetime, int imageID, double startX, double startY,
			Direction dir = 0, int depth = 0, double size = 1.0)
			: Actor(swptr, imageID, startX, startY, dir, depth, size), m_lifetime(lifetime), m_socrates(socptr)
		{

		}

		//All goodies have the same doSomething structure. With different
		//overlap actions
		virtual void doSomething();
		
		Socrates* mySoc()
		{
			return m_socrates; 
		}

		//All of these actors can be damaged!
		virtual bool canBeDamaged() const
		{
			return true;
		}

		virtual ~Pickups()
		{
			
		}



	protected: 
		//Will all interact with Socrates in some way. 
		virtual void overlapAction(const Actor* other) = 0;
		//Determines if goodie overlaps with Socrates. 
		bool socOverlap() const;
		void killIfDead(); 
	

	private:
		int m_lifetime; 
		Socrates* m_socrates; 
	};

		////////////////////////
		//RESTORE HEALTH CLASS//
		////////////////////////
		class ResHealth : public Pickups
		{
		public: 
			ResHealth(StudentWorld* swptr, Socrates* socptr, int lifetime, int startX, int startY)
				: Pickups(swptr, socptr, lifetime, IID_RESTORE_HEALTH_GOODIE, startX, startY, 0, 1)
			{

			}
		protected: 
			virtual void overlapAction(Actor* other);
		private: 
		};

		//////////////////////////////
		//RESTORE FLAME GOODIE CLASS//
		//////////////////////////////
		class ResFlame : public Pickups
		{
		public:
			ResFlame(StudentWorld* swptr, Socrates* socptr, int lifetime, int startX, int startY)
				: Pickups(swptr, socptr, lifetime, IID_FLAME_THROWER_GOODIE, startX, startY, 0, 1)
			{

			}
		protected:
			virtual void overlapAction(Actor* other);
		private:
		};

		///////////////////////////
		//EXTRA LIFE GOODIE CLASS//
		///////////////////////////
		class ResLife : public Pickups
		{
		public:
			ResLife(StudentWorld* swptr, Socrates* socptr, int lifetime, int startX, int startY)
				: Pickups(swptr, socptr, lifetime, IID_EXTRA_LIFE_GOODIE, startX, startY, 0, 1)
			{

			}
		protected:
			virtual void overlapAction(Actor* other);
		private:
		};

		///////////////////////////
		//EXTRA LIFE GOODIE CLASS//
		///////////////////////////
		class Fungus : public Pickups
		{
		public:
			Fungus(StudentWorld* swptr, Socrates* socptr, int lifetime, int startX, int startY)
				: Pickups(swptr, socptr, lifetime, IID_FUNGUS, startX, startY, 0, 1)
			{

			}
		protected:
			virtual void overlapAction(Actor* other);
		private:
		};


#endif // ACTOR_H_
