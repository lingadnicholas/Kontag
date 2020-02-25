#ifndef ACTOR_H_
#define ACTOR_H_

#include "GraphObject.h"
#include <cmath>
class StudentWorld; 
class Socrates; 

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

	//Pure virtual functions:
	//Every actor does something. 
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

	//Most actors do not have HP.
	virtual bool hasHP() const
	{
		return false; 
	}


	//Most objects are not bacteria interactables
	virtual bool isBacteriaInteractable() const
	{
		return false; 
	}

	//Most objects do not damage socrates
	virtual bool canDamageSocrates() const
	{
		return false;
	}
	//Most objects don't block other objects. 
	virtual bool blockOtherObjects() const
	{
		return false;
	}

	//Most types of bacteria are salmonella. a few are not. 
	virtual bool isSalmon() const
	{
		return false; 
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

	//Allows units with HP to take damage (or heal!) Should do nothing here.
	virtual void takeDamage(const int& amt)
	{
		return; 
	}

	//aggro + ecoli chase socrates. regular doesnt
	virtual bool chasesSocrates() const {
		return false; 
	}

	virtual bool canPickUp() const
	{
		return false;
	}
	virtual ~Actor()
	{

	}




	//Returns a pointer to the StudentWorld it is associated with. 
	StudentWorld* myWorld() const
	{
		return m_studentworld; 
	}

	//PUBLIC HELPER FUNCTIONS (StudentWorld uses these too..)
	double radialDistance(const Actor* first, const Actor* second) const;
	double  radialDistance(const double& x, const double& y) const;
	double radialDistance(const Actor* first, const double& x, const double& y) const;
	bool overlaps(const Actor* first, const Actor* second) const;
	bool overlaps(const double& x, const double& y) const;
	bool overlaps(const Actor* first, const double& x, const double& y) const;
protected:
	//Protected, because only used by objects derived from this class. 
	virtual void overlapAction(Actor* other)
	{
		return; 
	}

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
			: Actor(swptr, IID_DIRT, startX, startY, 0, 1) 
		{

		}

		//Dirt does not do anything per tick.
		virtual void doSomething()
		{
			return; //Dirt can't do anything! 
		}

		
		//Dirt blocks other objects
		virtual bool blockOtherObjects() const
		{
			return true;
		}


		//Returns true if the dirt should block the other object (if it gets too close!) 
		bool blockObjects(const Actor* other);

	protected:
		//Dirt is set to dead if it comes into contact with spray or flame. 
		virtual void overlapAction(Actor* other);

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

		//All of these objects have HP -- that's why they're grouped here! 
		virtual bool hasHP() const
		{
			return true;
		}
		int hp() const
		{
			return m_health;
		}


		//If an object has HP, then it can take damage. (Or heal) 
		virtual void takeDamage(const int& amt);

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
				:LivingWithHP(swptr, 100, IID_PLAYER, 0, VIEW_HEIGHT/2, 0, 0)
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
				if (m_flameCharges > 5)
					m_flameCharges = 5;
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

			//Returns flamethrower charges
			int getFlame() const
			{
				return m_flameCharges; 
			}

			//Returns spray charges
			int getSpray() const
			{
				return m_sprayCharges; 
			}
		protected: 
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

		//////////////////
		//BACTERIA CLASS//
		//////////////////
		class Bacteria : public LivingWithHP
		{
		public:
			Bacteria(StudentWorld* swptr, int HP, int imageID, int dmg, int movePixels, double startX, double startY)
				: LivingWithHP(swptr, HP, imageID, startX, startY), m_dmgAmt(dmg), m_movePixels(movePixels)
			{
				
			}

			//Both types of salmonella share a similar doSomething. But a fungus will be different.
			virtual void doSomething() = 0; 
			
			//Increase food eaten
			void incrementFood()
			{
			m_foodEaten++;
			}

			//All types of bacteria have a very similar overlapAction (damage Socrates)
			virtual void overlapAction(Actor* other); 

			virtual bool canDamageSocrates() const {
				return true; 
			}

			//Bacteria do be getting stuck on dirt tho 
			virtual bool blockedByDirt() const
			{
				return true;
			}

			virtual bool hasHP() const
			{
				return true; 
			}

			virtual ~Bacteria()
			{

			}
		protected: 
			int foodEaten() const
			{
				return m_foodEaten; 
			}

			void spawnSelf();

			void resetFood()
			{
				m_foodEaten = 0;
			}

			int getMPD() const
			{
				return m_MPD; 
			}

			void increaseMPD() {
				m_MPD++; 
			}

			void resetMPD()
			{
				m_MPD = 0; 
			}

			//Can check against different types if it overlaps. 
			void overlapsFood();

			void failedToMove();
			bool movementPlanDistance(); 

			//Gets location of closest food
			bool getClosestFood();

			//Checks if location is inside circle
			bool insideCircle(double& newX, double& newY);

			bool findSocrates(int dist);

			void checkFood(); 

			double angleInDeg(const double& otherx, const double& othery); 
		private: 
			int m_dmgAmt; 
			int m_MPD = 0; //movement plan distance 
			int m_foodEaten = 0; 
			int m_movePixels; //How many pixels obj moves
		};

		class Salmonella : public Bacteria
		{
		public: 
			Salmonella(StudentWorld* swptr, double startX, double startY, int HP = 4, int dmg = 1)
				: Bacteria(swptr, HP, IID_SALMONELLA, dmg, 3 ,startX, startY)
			{

			}

			virtual void doSomething(); 

			//Most types of bacteria are salmonella. a few are not. 
			virtual bool isSalmon() const
			{
				return true;
			}
			virtual ~Salmonella()
			{

			}
		};

		class AggroSalmonella : public Salmonella
		{
		public: 
			AggroSalmonella(StudentWorld* swptr, double startX, double startY)
				: Salmonella(swptr, startX, startY, 10, 2)
			{

			}
			virtual bool chasesSocrates() const
			{
				return true; 
			}
			virtual void doSomething();

		private: 
		};

		class EColi : public Bacteria
		{
		public: 
			EColi(StudentWorld* swptr, double startX, double startY, int HP = 5, int dmg = 4)
				: Bacteria(swptr, HP, IID_ECOLI, dmg, 2, startX, startY)
			{

			}

			virtual bool chasesSocrates() const
			{
				return true; 
			}

			virtual void doSomething();
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

		virtual bool canPickUp() const
		{
			return true;
		}

		virtual ~Pickups()
		{
			
		}



	protected: 
		//Will all interact with Socrates in some way. 
		virtual void overlapAction(Actor* other) = 0;
		//Determines if goodie overlaps with Socrates. 
		bool socOverlap() const;
	

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

		////////////////
		//FUNGUS CLASS//
		////////////////
		class Fungus : public Pickups
		{
		public:
			Fungus(StudentWorld* swptr, Socrates* socptr, int lifetime, int startX, int startY)
				: Pickups(swptr, socptr, lifetime, IID_FUNGUS, startX, startY, 0, 1)
			{

			}

			virtual bool canDamageSocrates() const
			{
				return true; 
			}
		protected:
			virtual void overlapAction(Actor* other);
		private:
		};

	//////////////////////////
	//BACTERIA INTERACTABLES//
	//////////////////////////
	class BacteriaInteractable : public Actor
	{
	public: 
		BacteriaInteractable(StudentWorld* swptr, int imageID, double startX, double startY, Direction dir = 0, int depth = 0, double size = 1.0)
			: Actor(swptr, imageID, startX, startY, dir, depth, size)
		{

		}
		//Considered Bacteria Interactables: Food and Socrates' Weapons
		virtual bool isBacteriaInteractable() const
		{
			return true; 
		}

		//All of bacteria interactables can NOT be damaged! 
		virtual bool canBeDamaged() const
		{
			return false; 
		}


		virtual ~BacteriaInteractable()
		{

		}
	protected:
	private: 
	};
		//////////////
		//FOOD CLASS//
		//////////////
	class Food : public BacteriaInteractable
	{
	public: 
		Food(StudentWorld* swptr, double startX, double startY)
			:BacteriaInteractable(swptr, IID_FOOD, startX, startY, 90, 1)
		{

		}

		//Food doesn't do anything. 
		virtual void doSomething() {
			return; 
		}

	protected: 
		//Food does nothing in itself when it overlaps with another.
		//Such an action should be taken care of in the bacteria class. 
		virtual void overlapAction(Actor* other)
		{
			return;
		}
	private:
		};
			/////////////////
			//WEAPONS CLASS//
			/////////////////
			class Weapons : public BacteriaInteractable
			{
			public:
				//TODO: THIS CLASS SHOULD BE PURE VIRTUAL. BUT I DEFINE DOSOMETHING()
				Weapons(StudentWorld* swptr, int imageID, double startX, double startY, Direction dir, int maxTravelDist, int damageAmt )
					: BacteriaInteractable(swptr, imageID, startX, startY, dir), m_originalX(startX), 
					m_originalY(startY), m_maxTravelDist(maxTravelDist), m_damageAmt(damageAmt)
				{

				}
				//Weapons are all used by Socrates. 
				virtual bool usedBySocrates() const
				{
					return true; 
				}

				//A weapon's overlap action will check if it overlaps with a damageable object
				//And then damage it. 
				virtual void overlapAction(Actor* other);

				//Flames and sprays will try to attack an overlapping object if it can
				//And then move forward SPRITE_RADIUS*2 pixels 
				//Dissipiate if it reaches its max travel distance. 
				virtual void doSomething();

				virtual ~Weapons()
				{

				}
			protected:
				

				//Checks if a weapon has reached its max travel distandce. 
				bool checkMaxDist() const
				{
					if (radialDistance(m_originalX, m_originalY) >= m_maxTravelDist)
						return true;
					else
						return false; 
				}
			private: 
				double m_maxTravelDist; 
				int m_damageAmt; 
				double m_originalX;
				double m_originalY; 
			};

				///////////////
				//FLAME CLASS//
				///////////////
				class Flame : public Weapons
				{
				public: 
					Flame(StudentWorld* swptr, double startX, double startY, Direction dir) 
						: Weapons(swptr, IID_FLAME, startX, startY, dir, 32, 5)
					{
					}
				private:
				};

				///////////////
				//SPRAY CLASS//
				///////////////
				class Spray : public Weapons
				{
				public:
					Spray(StudentWorld* swptr, double startX, double startY, Direction dir)
						: Weapons(swptr, IID_SPRAY, startX, startY, dir, 112, 2)
					{
					}
				private:
				};
	/////////////
	//PIT CLASS//
	/////////////
	class Pit : public Actor
	{
	public:
		Pit(StudentWorld* swptr, double startX, double startY)
			: m_numRegSalmon(5), m_numAggSalmon(3), m_numEColi(2), Actor(swptr, IID_PIT, startX, startY, 0, 1)
		{

		}
		//How to classify a pit: can't be damaged, doesn't block other objects. 
		virtual bool canBeDamaged() const
		{
			return false; 
		}

		void decrementRegSalmon()
		{
			m_numRegSalmon--; 
		}

		void decrementAggSalmon()
		{
			m_numAggSalmon--; 
		}

		void decrementNumEColi()
		{
			m_numEColi--; 
		}

		bool anyRegSalmon() const
		{
			if (m_numRegSalmon > 0)
				return true;
			return false; 
		}

		bool anyAggSalmon() const
		{
			if (m_numAggSalmon > 0)
				return true;
			return false;
		}

		bool anyEColi() const
		{
			if (m_numEColi > 0)
				return true;
			return false;
		}

		virtual void doSomething(); 


	protected:

	private: 
		int m_numRegSalmon;
		int m_numAggSalmon;
		int m_numEColi; 
	};

#endif // ACTOR_H_
