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
    Actor* mySoc() const
    {
        return m_socrates; 
    }
    ~StudentWorld(); 

private:
    std::list<Actor*> m_actors; 
    Actor* m_socrates; 

    //Avoids placement in areas where not allowed. 
    void StudentWorld::validPlacement(int& x, int& y);

    //Chooses random areas to place an object, given certain bounds (but not exact enough bounds)
    //Helper function for my helper function
    int placeWithConstraint(const int& lconstraint, const int& uconstraint, const int& randUpper);

    //Returns angle between 2 points
    double angle(int x1, int y1, int x2, int y2);
   // FIGURE OUT LATER void eraseSingle(list<Actor*>::iterator actorItr);

};

#endif // STUDENTWORLD_H_
