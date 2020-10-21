#include "StudentWorld.h"
#include "GameConstants.h"
#include <string>
using namespace std;

GameWorld* createStudentWorld(string assetPath)
{
	return new StudentWorld(assetPath);
}

// Students:  Add code to this file, StudentWorld.h, Actor.h and Actor.cpp
#include "Actor.h"
#include <random>
#include <sstream>

StudentWorld::StudentWorld(string assetPath)
: GameWorld(assetPath)
{
    m_player = nullptr; // i guess
}

StudentWorld::~StudentWorld()
{
    cleanUp();
}

int StudentWorld::init()
{
    const double PI = 4 * atan(1);
    const double OUTER_RADIUS = 120;
    
    int L = getLevel(); // must change later
    int foodCount = min(5 * L, 25);
    int dirtCount = max(180 - 20 * L, 20);
    
    // Create and add a new Socrates/player object at location (0, VIEW_HEIGHT/2) to the Petri dish
    m_player = new Socrates(this);
    m_actorList.push_back(m_player);
    
    // Add L pits to the Petri dish at random locations, in a manner such that no two pits overlap with each other (their centers must be more than SPRITE_WIDTH pixels apart from each other). Each pit must be no more than 120 pixels from the center of the Petri dish which is at (VIEW_WIDTH/2, VIEW_HEIGHT/2).
    for (int i = 0; i < L; i++)
    {
        bool respawn = false;
        double theta =  2 * PI * (randInt(0, 100) / 100.0);
        double d = OUTER_RADIUS * (randInt(0, 100) / 100.0) + OUTER_RADIUS * (randInt(0, 100) / 100.0);
        double r = d;
        if (d > OUTER_RADIUS) r = 2 * OUTER_RADIUS - d;
        double x = VIEW_WIDTH/2 + r * cos(theta);
        double y = VIEW_HEIGHT/2 + r * sin(theta);
        
        list<Actor*>::iterator it;
        for (it = m_actorList.begin(); it != m_actorList.end(); it++)
        {
            Actor* a = *it;
            if (checkOverlap(x, a->getX(), y, a->getY()))
            {
                respawn = true;
                break;
            }
        }
        if (respawn)
        {
            respawn = false;
            i--;
        }
        else m_actorList.push_back(new Pit(this, x, y));
    }
    
    // Add min(5 * L, 25) food objects to the Petri dish at random locations, in a manner such that no two food objects overlap with each other or previously-placed pits (their centers must be more than SPRITE_WIDTH pixels apart from each other). Each food object must be no more than 120 pixels from the center of the Petri dish which is at (VIEW_WIDTH/2, VIEW_HEIGHT/2).
    for (int i = 0; i < foodCount; i++)
    {
        bool respawn = false;
        double theta =  2 * PI * (randInt(0, 100) / 100.0);
        double d = OUTER_RADIUS * (randInt(0, 100) / 100.0) + OUTER_RADIUS * (randInt(0, 100) / 100.0);
        double r = d;
        if (d > OUTER_RADIUS) r = 2 * OUTER_RADIUS - d;
        double x = VIEW_WIDTH/2 + r * cos(theta);
        double y = VIEW_HEIGHT/2 + r * sin(theta);
        
        list<Actor*>::iterator it;
        for (it = m_actorList.begin(); it != m_actorList.end(); it++)
        {
            Actor* a = *it;
            if (checkOverlap(x, a->getX(), y, a->getY()))
            {
                respawn = true;
                break;
            }
        }
        if (respawn)
        {
            respawn = false;
            i--;
        }
        else m_actorList.push_back(new Food(this, x, y));
    }
    
    // Add max(180 – 20 * L, 20) dirt objects to the Petri dish at random locations, in a manner such that no dirt objects overlap with previously-placed food objects or pits (their centers must be more than SPRITE_WIDTH pixels apart from each other). It is OK for dirt objects to overlap with each other, however. Each dirt object must be no more than 120 pixels from the center of the Petri dish which is at (VIEW_WIDTH/2, VIEW_HEIGHT/2).
    for (int i = 0; i < dirtCount; i++)
    {
        bool respawn = false;
        double theta =  2 * PI * (randInt(0, 100) / 100.0);
        double d = OUTER_RADIUS * (randInt(0, 100) / 100.0) + OUTER_RADIUS * (randInt(0, 100) / 100.0);
        double r = d;
        if (d > OUTER_RADIUS) r = 2 * OUTER_RADIUS - d;
        double x = VIEW_WIDTH/2 + r * cos(theta);
        double y = VIEW_HEIGHT/2 + r * sin(theta);
        
        list<Actor*>::iterator it = m_actorList.begin();
        for (int i = 0; i < foodCount; i++)
        {
            Actor* a = *it;
            if (checkOverlap(x, a->getX(), y, a->getY()))
            {
                respawn = true;
                break;
            }
            else it++;
        }
        if (respawn)
        {
            respawn = false;
            i--;
        }
        else m_actorList.push_back(new Dirt(this, x, y));
    }
    
    return GWSTATUS_CONTINUE_GAME;
}

int StudentWorld::move()
{
    // This code is here merely to allow the game to build, run, and terminate after you hit enter.
    // Notice that the return value GWSTATUS_PLAYER_DIED will cause our framework to end the current level.
    
    const double PI = 4 * atan(1);
    
    //decLives();
    bool levelOver = true;
    list<Actor*>::iterator it;
    it = m_actorList.begin();
    while (it != m_actorList.end())
    {
        Actor *a = *it;
        if (a->isAlive())
        {
            a->doSomething();
            if (!m_player->isAlive()) {
                m_player = nullptr;
                decLives();
                return GWSTATUS_PLAYER_DIED;
            }
            if (a->preventsLevelCompleting()) levelOver = false;
        }
        it++;
        
    }
    
    if (levelOver)
    {
        playSound(SOUND_FINISHED_LEVEL);
        return GWSTATUS_FINISHED_LEVEL;
    }
    
    list<Actor*>::iterator dead;
    dead = m_actorList.begin();
    while (dead != m_actorList.end())
    {
        if (!(*dead)->isAlive())
        {
            list<Actor*>::iterator temp;
            temp = dead;
            ++dead;
            m_actorList.erase(temp);
            delete *temp;
        }
        else ++dead;
    }
    
    // Potentially add new actors to the game (e.g., goodies or fungi)
    int L = getLevel();
    int chanceFungus = max(510 - L * 10, 200);
    int i = randInt(0, chanceFungus - 1); // this is the range [0, chanceFungus]... does it matter?
    if (i == 0)
    {
        double theta =  2 * PI * (randInt(0, 100) / 100.0);
        double x = VIEW_RADIUS * cos(theta) + VIEW_WIDTH/2;
        double y = VIEW_RADIUS * sin(theta) + VIEW_HEIGHT/2;
        int fungusLife = max(randInt(0, 300 - 10 * L - 1), 50);
        m_actorList.push_back(new Fungus(fungusLife, this, x, y));
    }
    int chanceGoodie = max(510 - L * 10, 250);
    int j = randInt(0, chanceGoodie - 1);
    if (j == 0)
    {
        double theta =  2 * PI * (randInt(0, 100) / 100.0);
        double x = VIEW_RADIUS * cos(theta) + VIEW_WIDTH/2;
        double y = VIEW_RADIUS * sin(theta) + VIEW_HEIGHT/2;
        int goodieLife = max(randInt(0, 300 - 10 * L - 1), 50);
        int goodieType = randInt(1, 100);
        if (goodieType <= 60) m_actorList.push_back(new RestoreHealthGoodie(goodieLife, this, x, y));
        else if (goodieType > 90) m_actorList.push_back(new ExtraLifeGoodie(goodieLife, this, x, y));
        else m_actorList.push_back(new FlamethrowerGoodie(goodieLife, this, x, y));
    }
    
    // Update the game status line
    ostringstream oss;
    oss.fill('0');
    oss << "Score: ";
    if (getScore() < 0)
    {
        oss << '-';
        oss << setw(5) << abs(getScore()) << "  ";
    }
    else oss << setw(6) << abs(getScore()) << "  ";
    oss << "Level: " << getLevel() << "  ";
    oss << "Lives: " << getLives() << "  ";
    oss << "Health: " << m_player->getHP() << "  ";
    oss << "Sprays: " << m_player->getSprayChargeCount() << "  ";
    oss << "Flames: " << m_player->getFlameChargeCount();
    string s = oss.str();
    setGameStatText(s);
    
    return GWSTATUS_CONTINUE_GAME;
}

void StudentWorld::cleanUp()
{
//    std::list<Actor*>::iterator it;
//    it = m_actorList.begin();
//    while (it != m_actorList.end())
//    {
//        Actor* a = *it;
//        it++;
//        delete a;
//    }
    for (std::list<Actor*>::iterator it = m_actorList.begin(); it != m_actorList.end(); ++it)
    {
        delete *it;
    }
    m_actorList.clear();
}

void StudentWorld::addActor(Actor *a)
{
    m_actorList.push_back(a);
}

// returns true if damage is inflicted
bool StudentWorld::inflictDamage(Projectile* src)
{
    list<Actor*>::iterator it;
    for (it = m_actorList.begin(); it != m_actorList.end(); it++)
    {
        Actor* target = *it;
        if (target->isAlive())
        {
            if (target->isDamageable() && src->checkOverlap(target))
            {
                target->incurDamage(src->getDamage());
                return true;
            }
        }
    }
    return false;
}

bool StudentWorld::blockMovement(double x, double y)
{
    list<Actor*>::iterator it;
    for (it = m_actorList.begin(); it != m_actorList.end(); it++)
    {
        Actor* target = *it;
        if (target->isAlive())
        {
            double distance = sqrt(pow(x - target->getX(), 2) + pow(y - target->getY(), 2));
            if (target->isObstacle() && distance <= SPRITE_WIDTH/2)
            {
                return true;
            }
        }
    }
    return false;
}

// returns true if interaction occurs
bool StudentWorld::pickUpGoodie(Goodie* src) // can we generalize this?
{
    if (src->checkOverlap(m_player))
    {
        src->pickUp(m_player);
        return true;
    }
    return false;
}

bool StudentWorld::damageSocrates(Bacterium* src)
{
    if (src->checkOverlap(m_player))
    {
        m_player->incurDamage(src->getDamage());
        return true;
    }
    return false;
}

bool StudentWorld::checkOverlap(double x1, double x2, double y1, double y2) // maybe unnecessary
{
    double distance = sqrt(pow(x1 - x2, 2) + pow(y1 - y2, 2));
    if (distance <= SPRITE_RADIUS * 2) return true;
    else return false;
}

// returns -1 if no food found
int StudentWorld::locateFood(double x, double y)
{
    const double PI = 4 * atan(1);
    list<Actor*>::iterator it;
    double minDistance = 129; // > 128
    double theta = 0;
    for (it = m_actorList.begin(); it != m_actorList.end(); it++)
    {
        Actor* target = *it;
        if (target->isAlive())
        {
            double distance = sqrt(pow(target->getX() - x, 2) + pow(target->getY() - y, 2));
            if (target->isEdible() && distance <= 128)
            {
                if (distance < minDistance)
                {
                    minDistance = distance;
                    if (target->getX() == x && target->getY() > y) theta = PI / 2; // prevent division by zero
                    else if (target->getX() == x && target->getY() < y) theta = 3 * PI / 2;
                    else theta = atan((target->getY() - y) / (target->getX() - x));
                    if (target->getX() < x) theta += PI;
                    if (theta < 0) theta += 2 * PI;
                }
            }
        }
    }
    if (minDistance <= 128)
    {
//        cout << theta * 180.0 / PI;
        return theta * 180.0 / PI;
    }
    return -1;
}

int StudentWorld::locateSocrates(double x, double y, int detectionBound)
{
    const double PI = 4 * atan(1);
    double theta = 0;
    double distance = sqrt(pow(m_player->getX() - x, 2) + pow(m_player->getY() - y, 2));
    if (distance <= detectionBound)
    {
        if (m_player->getX() == x && m_player->getY() > y) theta = PI / 2; // prevent division by zero
        else if (m_player->getX() == x && m_player->getY() < y) theta = 3 * PI / 2;
        else theta = atan((m_player->getY() - y) / (m_player->getX() - x));
        if (m_player->getX() < x) theta += PI;
        if (theta < 0) theta += 2 * PI;
        return theta * 180.0 / PI;
    }
    return -1;
}

bool StudentWorld::eatFood(Bacterium* src)
{
    list<Actor*>::iterator it;
    for (it = m_actorList.begin(); it != m_actorList.end(); it++)
    {
        Actor* target = *it;
        if (target->isAlive())
        {
            if (target->isEdible() && src->checkOverlap(target))
            {
                target->setDead();
                return true;
            }
        }
    }
    return false;
}
