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

   // FIGURE OUT LATER void eraseSingle(list<Actor*>::iterator actorItr);

};

#endif // STUDENTWORLD_H_
