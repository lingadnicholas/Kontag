#include "StudentWorld.h"
#include "GameConstants.h"
#include "Actor.h"
#include <string>
#include <cmath>
#include <sstream>
#include <iomanip> 
using namespace std;

//Find better place for this function? 
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

    //INTIALIZE L PITS
    int numPits = getLevel(); 
    m_nBacteria = 0;
    m_nPits = numPits; 
    for (int i = 0; i < numPits; i++)
    {
        double x, y; 
        do {
            validPlacement(x, y);
        } while (invalidOverlap(x, y, true, false, false)); //Pits cannot overlap with other pits. 

        Pit* newPit = new Pit(this, x, y); 
        m_actors.push_back(newPit); 
    }

    //INITIALIZE FOOD
    int numFood = min(5 * getLevel(), 25); 
    for (int i = 0; i < numFood; i++)
    {
        double x, y; 
        do {
            validPlacement(x, y);
        } while (invalidOverlap(x, y, true, true, false)); //Food can overlap with existing dirt (but no dirt exists yet) but not other food or pits.

        Food* newFood = new Food(this, x, y);
        m_actors.push_back(newFood);
    }
   

    //INITIALIZING DIRT
    int numDirt = max(180 - 20 * getLevel(), 20); 
    for (int i = 0; i < numDirt; i++) 
    {
        double x, y;
        do {
            validPlacement(x, y);
        } while (invalidOverlap(x, y, true, true, false)); //Dirt can overlap with existing dirt but not existing food or pits 

        Dirt* newDirt = new Dirt(this, x, y);
        m_actors.push_back(newDirt);
    }
    return GWSTATUS_CONTINUE_GAME;
}

int StudentWorld::move()
{

    m_socrates->doSomething(); 
    list<Actor*>::iterator actorItr = m_actors.begin(); 

    //Let all actors do what they need to do!
    while (actorItr != m_actors.end())
    {
        //Living actors do something
        if ((*actorItr)->alive())
        {
            (*actorItr)->doSomething();
        }
        //If an actor kills socrates, then then game over! 
        if (!m_socrates->alive())
        {
            delete m_socrates; 
            m_socrates = nullptr; 
            decLives(); 
            return GWSTATUS_PLAYER_DIED;
        }

        actorItr++; 
    }
    if (m_nPits == 0 && m_nBacteria == 0)
    {
        playSound(SOUND_FINISHED_LEVEL); 
        return GWSTATUS_FINISHED_LEVEL;
    }
    //Add fungus
    const double PI = 4 * atan(1); 
    int chanceFungus = min(510 - getLevel() * 10, 200); 
    int randFung = randInt(0, chanceFungus); 
   
    //Should we add a fungus this tick? 
    if (randFung == 0)
    {
        int randomLife = max(rand() % (300 - 10 * getLevel()), 50);

        //Pick a random angle for a random position
        double randomAngle = randInt(0, 2 * PI); 
        //Place a new fungus VIEW_RADIUS pixels from the center of the Petri dish.
        Fungus* newFung = new Fungus(this, m_socrates, randomLife, 
            VIEW_RADIUS + VIEW_RADIUS * cos(randomAngle),
            VIEW_RADIUS + VIEW_RADIUS * sin(randomAngle)); 
        m_actors.push_back(newFung); 
    }

    //Add goodies
    int chanceGoodie = min(510 - getLevel() * 10, 250); 
    int randGoodie = randInt(0, chanceGoodie); 

    //Should we add a goodie this tick? 
    if (randGoodie == 0)
    {
        const int spawnHP = 1; 
        const int spawnFlame = 2; 
        const int spawnLife = 3; 
        //Pick a number from 0-99. This determines what goodie spawns! 
        int RNG = randInt(0, 99);

        //Other random variables to determine life and position of new goodie
        int randomAngle = randInt(0, 2 * PI);
        int randomLife = max(rand() % (300 - 10 * getLevel()), 50);

        int choice;

        if (RNG >= 0 && RNG < 60) //60% chance to be a restore health goodie
            choice = spawnHP;
        else if (RNG >= 60 && RNG < 90) // 30% chance to be a flame thrower goodie
            choice = spawnFlame;
        else //10% chance to be an extra-life goodie
            choice = spawnLife; 
        //Spawns in chosen goodie. 
        switch (choice)
        {
        case spawnHP: 
        {
            ResHealth* newHP = new ResHealth(this, m_socrates, randomLife,
                VIEW_RADIUS + VIEW_RADIUS * cos(randomAngle),
                VIEW_RADIUS + VIEW_RADIUS * sin(randomAngle));
            m_actors.push_back(newHP);
        }
            break;
        case spawnFlame: 
        {
            ResFlame* newFlame = new ResFlame(this, m_socrates, randomLife,
                VIEW_RADIUS + VIEW_RADIUS * cos(randomAngle),
                VIEW_RADIUS + VIEW_RADIUS * sin(randomAngle));
            m_actors.push_back(newFlame);
        }
            break; 
        case spawnLife: 
        {
            ResLife* newLife = new ResLife(this, m_socrates, randomLife,
                VIEW_RADIUS + VIEW_RADIUS * cos(randomAngle),
                VIEW_RADIUS + VIEW_RADIUS * sin(randomAngle));
            m_actors.push_back(newLife); 
        }
            break; 
        }
    }

    //First gather information that must be displayed. 
    int score = getScore(); 
    int level = getLevel(); 
    int lives = getLives(); 
    int health = m_socrates->hp(); 
    int spray = m_socrates->getSpray(); 
    int flame = m_socrates->getFlame();

    //Put everything into a string
    string display;
    outputString(score, 6, "Score: ", display);
    outputString(level, 1, " Level: ", display); 
    outputString(lives, 1, " Lives: ", display); 
    outputString(health, 3, " Health: ", display); 
    outputString(spray, 2, " Sprays: ", display); 
    outputString(flame, 1, " Flame: ", display); 

    //Now display it all on screen
    setGameStatText(display); 

    //If actors are dead, remove them
    actorItr = m_actors.begin();
    while (actorItr != m_actors.end())
    {
        if (!(*actorItr)->alive())
        {
            if ((*actorItr)->canDamageSocrates() && !(*actorItr)->canPickUp()) //If bacteria
            {
                m_nBacteria--; 
                    int RNG = randInt(0, 1);
                    if (RNG == 0)
                    {
                        Food* newFood = new Food(this, (*actorItr)->getX(), (*actorItr)->getY());
                          m_actors.push_back(newFood); 
                    }
                
            }
            else if (!(*actorItr)->canBeDamaged() && !(*actorItr)->blockOtherObjects() && !(*actorItr)->isBacteriaInteractable()) //If pit
            {
                m_nPits--; 
            }
            actorItr = eraseSingle(actorItr);
        }
        else
            actorItr++;
    }

  
    return GWSTATUS_CONTINUE_GAME;
}

void StudentWorld::cleanUp()
{
    if (m_socrates != nullptr)
        delete m_socrates; 
    list<Actor*>::iterator actorItr = m_actors.begin();
    while (actorItr != m_actors.end())
    {
        actorItr = eraseSingle(actorItr);
    }
}

StudentWorld::~StudentWorld()
{
    cleanUp(); 
}

////////////////////////////
//PRIVATE HELPER FUNCTIONS//
////////////////////////////
//FIGUIRE OUT LATER
//Correctly erases a single actor from the game. 
list<Actor*>::iterator StudentWorld::eraseSingle(list<Actor*>::iterator actorItr)
 {
    delete(*actorItr); 
    *actorItr = nullptr;
    actorItr = m_actors.erase(actorItr); 
    return actorItr; 
}

//Useful for placing objects on the screen. Will avoid placement in areas where not allowed. 
int StudentWorld::placeWithConstraint(const double& lconstraint, const double& uconstraint, const double& randUpper)
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
double StudentWorld::angle(const double& x1, const double& y1, const double& x2, const double& y2) const
{
    const double PI = 4 * atan(1);
    double y = y1 - y2;
    double x = x1 - x2;
    if (x == 0)
        return 90;
    double val = y / x;
    return atan(val) * 180 / PI;
}

void StudentWorld::validPlacement(double& x, double& y)
{
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
}

void StudentWorld::outputString(int displayNum, int numDigits, string literal, string& display)
{
    ostringstream oss_displayNext;
    //If you are displaying a SCORE and it is negative.....
    if (numDigits == 6 && displayNum < 0)
    {
        displayNum *= -1; //Make positive for display purposes
        oss_displayNext << "-";
        numDigits--; 
    }
    oss_displayNext.fill('0'); //Fills remainder with 0s
 
    oss_displayNext << setw(numDigits) << displayNum; 
    //Append necessary information to end of string
    display += literal;
    display += oss_displayNext.str();
}

//Return true if overlap is not allowed to happen 
//Return false otherwise. 
bool StudentWorld::invalidOverlap(const double& x, const double& y, const bool checkPit, const bool checkFood, const bool checkDirt) const
{
    list<Actor*>::const_iterator actorItr = m_actors.begin();
    while (actorItr != m_actors.end())
    {
        if (checkPit)
        {
            if (!(*actorItr)->canBeDamaged() && !(*actorItr)->blockOtherObjects() && !(*actorItr)->isBacteriaInteractable()) //Identify if the actor is a pit
            {
                if ((*actorItr)->overlaps(x, y))
                    return true; 
            }
        }
        if (checkFood)
        {
            if ((*actorItr)->isBacteriaInteractable() && !(*actorItr)->usedBySocrates()) //Identify if the actor is food
            {
                if ((*actorItr)->overlaps(x, y))
                    return true; 
            }
        }
        if (checkDirt)
        {
            if ((*actorItr)->canBeDamaged() && (*actorItr)->blockOtherObjects()) //Identify if the actor is dirt.
            {
                if ((*actorItr)->overlaps(x, y))
                    return true;
            }
        }
        actorItr++; 
    }
    return false; 
}

void StudentWorld::addActor(const int& type, const double& x, const double& y, const Direction& dir)
{
    if (type == 0) //Push spray
    {
        Spray* newSpray = new Spray(this, x, y, dir);
        m_actors.push_back(newSpray);
    }
    else if (type == 1) //Push flame
    {
        Flame* newFlame = new Flame(this, x, y, dir);
        m_actors.push_back(newFlame);
    }
    
    
    else if (type == 2) //Push Salmonella 
    {
        Salmonella* newSalmon = new Salmonella(this, x, y); 
        m_actors.push_back(newSalmon); 
        m_nBacteria++; 
    }
   
    
    else if (type == 3) //Push aggressive salmonella 
    {
        AggroSalmonella* newAggro = new AggroSalmonella(this, x, y); 
        m_actors.push_back(newAggro); 
        m_nBacteria++; 
    }
    
    
    else if (type == 4) //Push ecoli
    {
        EColi* newEcoli = new EColi(this, x, y);
        m_actors.push_back(newEcoli); 
        m_nBacteria++; 
    }
    
    else
    {
        Food* newFood = new Food(this, x, y);
        m_actors.push_back(newFood);
    }
    
        return;
}