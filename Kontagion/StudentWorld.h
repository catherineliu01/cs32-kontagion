#ifndef STUDENTWORLD_H_
#define STUDENTWORLD_H_

#include "GameWorld.h"
#include <string>

// Students:  Add code to this file, StudentWorld.cpp, Actor.h, and Actor.cpp

#include <list>
class Actor;
class Goodie;
class Bacterium;
class Projectile;
class Socrates;

class StudentWorld : public GameWorld
{
public:
    StudentWorld(std::string assetPath);
    virtual ~StudentWorld();
    virtual int init();
    virtual int move();
    virtual void cleanUp();
    void addActor(Actor* a);
    bool inflictDamage(Projectile* src);
    bool checkOverlap(double x1, double x2, double y1, double y2);
    bool pickUpGoodie(Goodie* src);
    bool damageSocrates(Bacterium* src);
    bool blockMovement(double x, double y);
    int locateFood(double x, double y);
    int locateSocrates(double x, double y, int detectionBound);
    bool eatFood(Bacterium* src);
    
private:
    std::list<Actor*> m_actorList;
    Socrates* m_player;
};

#endif // STUDENTWORLD_H_
