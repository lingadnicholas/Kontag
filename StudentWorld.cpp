#include "StudentWorld.h"
#include "GameConstants.h"
#include "Actor.h"
#include <string>
#include <cmath>
using namespace std;

//Find better place for this function? 
int placeWithConstraint(const int& lconstraint, const int& uconstraint, const int& randUpper);
GameWorld* createStudentWorld(string assetPath)
{
	return new StudentWorld(assetPath);
}

// Students:  Add code to this file, StudentWorld.h, Actor.h and Actor.cpp

StudentWorld::StudentWorld(string assetPath)
: GameWorld(assetPath)
{
}

int StudentWorld::init()
{
    m_socrates = new Socrates(this); 
    //INITIALIZING DIRT
    int numDirt = max(180 - 20 * getLevel(), 20); 
    for (int i = 0; i < numDirt; i++) 
    {
        //DIRT CANNOT OVERLAP WITH FOOD OR PITS
        int x, y;
        const double OUTER = 120; 
        double coordRadius, outerRadius, xlen, ylen, outerxlen, outerylen; 
        bool xNeg = false, yNeg = false; 

        //CHECK FOR VALID POS!
        do {
            x = placeWithConstraint(0, VIEW_RADIUS + OUTER, VIEW_RADIUS + OUTER); //***BOUNDS ARE WRONG***, IT IS A RADIUS. TRIG TIME
            y = placeWithConstraint(0, VIEW_RADIUS + OUTER, VIEW_RADIUS + OUTER);

            //Get the max lengths x and y can be, with respect to the angle that the created x and y is at
            double angle = (x, y, VIEW_RADIUS, VIEW_RADIUS);
            outerxlen = OUTER * cos(angle);
            outerylen = OUTER * sin(angle); 

           
            if (x < VIEW_RADIUS) //x is "negative" (if the center was 0, 0)
                xlen = (VIEW_RADIUS - x); //For example, if x was 120, and the center of the circle was 128, 
            else if (x == VIEW_RADIUS)
                xlen = 0;
            else //x is positive
                xlen = (x - VIEW_RADIUS);

            if (y < VIEW_RADIUS) //y is "negative" (if the center was 0,0)
                ylen = (VIEW_RADIUS - y); 
            else if (y == VIEW_RADIUS)
                ylen = 0; 
            else //x is positive
                ylen = (y - VIEW_RADIUS); 
            

            coordRadius = sqrt(xlen * xlen + ylen * ylen); //Radius of 
            outerRadius = sqrt(outerxlen * outerxlen + outerylen * outerylen); //Radius of the 
   
            
        } while (coordRadius > outerRadius); 
        
        Dirt* z = new Dirt(this,x, y); 
        m_actors.push_back(z); 
    }
    return GWSTATUS_CONTINUE_GAME;
}

int StudentWorld::move()
{
    // This code is here merely to allow the game to build, run, and terminate after you hit enter.
    // Notice that the return value GWSTATUS_PLAYER_DIED will cause our framework to end the current level.
    std::list<Actor*>::iterator actorItr = m_actors.begin(); 
    while (actorItr != m_actors.end())
    {
        
        (*actorItr)->doSomething(); 
        actorItr++; 
    }

   // decLives(); //Remove 
    return GWSTATUS_PLAYER_DIED;
}

void StudentWorld::cleanUp()
{
    std::list<Actor*>::iterator actorItr = m_actors.begin();
    while (actorItr != m_actors.end())
    {
        delete (*actorItr);
        *actorItr = nullptr;
        actorItr = m_actors.erase(actorItr); 
    }
}

StudentWorld::~StudentWorld()
{
    cleanUp(); 
}


//Useful for placing objects on the screen. Will avoid placement in areas where not allowed. 
int placeWithConstraint(const int& lconstraint, const int& uconstraint, const int& randUpper)
{
    int coord; 
    do
    {
        coord = randInt(0, randUpper);
    } while (coord < lconstraint || coord > uconstraint);

    return coord; 
}

//Returns angle between point 1 and point 2
//Point 2 should be the center of the circle (VIEW_RADIUS, VIEW_RADIUS) 
double angle(int x1, int y1, int x2, int y2)
{
    const double PI = 3.14159265358979323846;
    double y = y1 - y2;
    double x = x1 - x2;
    if (x == 0)
    {
        if (y > 0)
            return 90;
        else
            return 270; 
    }
    double val = y / x;
    return atan(val) * 180 / PI;
}