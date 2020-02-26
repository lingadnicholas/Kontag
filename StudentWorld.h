#ifndef STUDENTWORLD_H_
#define STUDENTWORLD_H_

#include "GameWorld.h"
#include <string>
#include <list>
#include "Actor.h"

class Socrates;
class StudentWorld : public GameWorld
{
public:
    StudentWorld(std::string assetPath);
    virtual int init();
    virtual int move();
    virtual void cleanUp();

    //Returns a pointer to socrates
    Socrates* mySoc() const
    {
        return m_socrates; 
    }

    //Allows Socrates to add weapons or bacteria from outside of the class. 
    void addActor(const int& type, const double& x, const double& y, const Direction& dir);

    //Allows weapons to hurt studentworld objects
    void weaponHurt(Weapons* wep); 

    //Allows bacteria to eat food
    void bacteriaOverlapsFood(Bacteria* bac); 

    //Allows bacteria to move if not blocked by dirt. Returns true if doesn't get blocked.
    bool bacteriaCanMoveAction(Bacteria* bac, double newx, double newy);

    //Helps findSocrates function not collide with dirt.
    bool findSocratesHelper(Bacteria* bac, const double& tempX, const double& tempY);

    //Returns a pointer to the closest food actor
    Actor* closestFood(Bacteria* bac, double& dist, double& x, double& y);

    void incrementBacteria()
    {
        m_nBacteria++; 
    }
    ~StudentWorld(); 

private:
    std::list<Actor*> m_actors; 
    Socrates* m_socrates; 
    int m_nBacteria;
    int m_nPits;

    //Avoids placement in areas where not allowed. 
    void validPlacement(double& x, double& y);

    //Chooses random areas to place an object, given certain bounds (but not exact enough bounds)
    //Helper function for my helper function
    int placeWithConstraint(const double& lconstraint, const double& uconstraint, const double& randUpper);

    //Returns angle between 2 points
    double angle(const double& x1, const double& y1, const double& x2, const double& y2) const;

    //Erases from list
    std::list<Actor*>::iterator eraseSingle(std::list<Actor*>::iterator actorItr);

    //Converts ints into ostringstreams into the proper strings
    void outputString(int displayNum, int numDigits, std::string literal, std::string& display);
    
    //Checks if dirt/food/pits are being placed in incorrect positions. 
    bool invalidOverlap(const double& x, const double& y, const bool checkPit, const bool checkFood, const bool checkDirt) const;
};

#endif // STUDENTWORLD_H_
