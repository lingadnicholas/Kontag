#ifndef STUDENTWORLD_H_
#define STUDENTWORLD_H_

#include "GameWorld.h"
#include <string>
#include <list>

class Actor;
class Socrates;
// Students:  Add code to this file, StudentWorld.cpp, Actor.h, and Actor.cpp

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
    ~StudentWorld(); 

private:
    std::list<Actor*> m_actors; 
    Socrates* m_socrates; 

    //Avoids placement in areas where not allowed. 
    void validPlacement(int& x, int& y);

    //Chooses random areas to place an object, given certain bounds (but not exact enough bounds)
    //Helper function for my helper function
    int placeWithConstraint(const int& lconstraint, const int& uconstraint, const int& randUpper);

    //Returns angle between 2 points
    double angle(int x1, int y1, int x2, int y2);

    //Erases from list
    std::list<Actor*>::iterator eraseSingle(std::list<Actor*>::iterator actorItr);

    //Converts ints into ostringstreams into the proper strings
    void outputString(int displayNum, int numDigits, std::string literal, std::string& display);
    
    //Checks if dirt/food/pits are being placed in incorrect positions. 
    bool invalidOverlap(const Actor* current, const bool checkPit, const bool checkFood, const bool checkDirt) const;
};

#endif // STUDENTWORLD_H_
