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

        //CHECK FOR VALID POS!
        do {
            x = placeWithConstraint(0, VIEW_RADIUS + OUTER, VIEW_RADIUS + OUTER); 
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
            

            coordRadius = sqrt(xlen * xlen + ylen * ylen); //Radius of the coordinate
            outerRadius = sqrt(outerxlen * outerxlen + outerylen * outerylen); //Boundary -- coord radius can't be this size or bigger
   
            
        } while (coordRadius > outerRadius); 
        
        Dirt* z = new Dirt(this,x, y); 
        m_actors.push_back(z); 
    }
    return GWSTATUS_CONTINUE_GAME;
}

int StudentWorld::move()
{

    //Let all actors do what they need to do!
    m_socrates->doSomething(); 
    list<Actor*>::iterator actorItr = m_actors.begin(); 
    while (actorItr != m_actors.end())
    {
        
        (*actorItr)->doSomething(); 
        actorItr++; 
    }

    //Look for dead actors!
    if (!m_socrates->alive()) //If Socrates is dead, then the level is over!
        return GWSTATUS_PLAYER_DIED; 

    /*  CAUSES CRASH
    //If other actors are dead, remove them
    actorItr = m_actors.begin();
    while (actorItr != m_actors.end())
    {
        if (!(*actorItr)->alive())
        {
            //DUPLICATING CODE.... I WANT TO USE A FUNCTION BUT
            //HAVING TROUBLE PASSING IN AN ITERATOR TO A FUNCTION
            delete(*actorItr);
            *actorItr = nullptr;
            actorItr = m_actors.erase(actorItr);
        }
    }
    */
    //TODO: Add new goodies

    return GWSTATUS_CONTINUE_GAME;
}

void StudentWorld::cleanUp()
{
    list<Actor*>::iterator actorItr = m_actors.begin();
    while (actorItr != m_actors.end())
    {
        delete(*actorItr);
        *actorItr = nullptr;
        actorItr = m_actors.erase(actorItr);
    }
}

StudentWorld::~StudentWorld()
{
    cleanUp(); 
}
////////////////////////////
//PRIVATE HELPER FUNCTIONS//
////////////////////////////
/* FIGUIRE OUT LATER
//Correctly erases a single actor from the game. 
void StudentWorld::eraseSingle(list<Actor*>::iterator actorItr)
{
    delete(*actorItr); 
    *actorItr = nullptr;
    actorItr = m_actors.erase(actorItr); 
}
*/ 
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
    const double PI = 4 * atan(1);
    double y = y1 - y2;
    double x = x1 - x2;
    if (x == 0)
        return 90;
    double val = y / x;
    return atan(val) * 180 / PI;
}