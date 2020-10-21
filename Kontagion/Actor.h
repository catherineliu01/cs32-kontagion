#ifndef ACTOR_H_
#define ACTOR_H_

#include "GraphObject.h"

// Students:  Add code to this file, Actor.cpp, StudentWorld.h, and StudentWorld.cpp

class StudentWorld;

class Actor : public GraphObject // virtual class
{
public:
    Actor(StudentWorld* world, int iid, double startX, double startY, Direction dir = 0, int depth = 1);
    virtual ~Actor();
    virtual void doSomething() = 0;
    StudentWorld* getWorld() const;
    bool isAlive() const;
    void setDead();
    bool checkOverlap(Actor* a);
    virtual bool isDamageable() const;
    virtual bool isObstacle() const;
    virtual bool isEdible() const;
    virtual bool preventsLevelCompleting() const;
    virtual void incurDamage(int damage);
private:
    StudentWorld* m_world;
    bool m_isAlive;
};

class Agent : public Actor
{
public:
    Agent(int hp, StudentWorld* world, int iid, double startX, double startY, Direction dir = 0, int depth = 0);
    virtual ~Agent();
    void setHP(int newHP);
    int getHP() const;
    virtual void incurDamage(int damage);
    virtual void hurt() = 0;
private:
    int m_hp;
};

class Socrates : public Agent
{
public:
    Socrates(StudentWorld* world);
    virtual ~Socrates();
    virtual void doSomething();
    void addFlameCharges(int charges);
    int getFlameChargeCount() const;
    int getSprayChargeCount() const;
    virtual void hurt();
private:
    int m_sprayCharges;
    int m_flameCharges;
};

class Bacterium : public Agent
{
public:
    Bacterium(int damage, int hp, StudentWorld* world, int iid, double startX, double startY);
    virtual ~Bacterium();
    virtual bool isDamageable() const;
    virtual void doSomething();
    int getDamage() const;
    virtual Bacterium* specializedSpawn(double x, double y) = 0;
    void setMovePlan(int newPlan);
    int getMovePlan() const;
    virtual bool preventsLevelCompleting() const;
private:
    int m_movementPlanDistance;
    int m_damage;
    int m_foodEaten;
};

class Salmonella : public Bacterium
{
public:
    Salmonella(int damage, int hp, StudentWorld* world, double startX, double startY);
    virtual ~Salmonella();
    virtual void hurt();
    virtual void doSomething();
};

class RegularSalmonella : public Salmonella
{
public:
    RegularSalmonella(StudentWorld* world, double startX, double startY);
    virtual ~RegularSalmonella();
    virtual Bacterium* specializedSpawn(double x, double y);
    virtual void doSomething();
};

class AggressiveSalmonella : public Salmonella
{
public:
    AggressiveSalmonella(StudentWorld* world, double startX, double startY);
    virtual ~AggressiveSalmonella();
    virtual Bacterium* specializedSpawn(double x, double y);
    virtual void doSomething();
};

class EColi : public Bacterium
{
public:
    EColi(StudentWorld* world, double startX, double startY);
    virtual ~EColi();
    virtual void hurt();
    virtual void doSomething();
    virtual Bacterium* specializedSpawn(double x, double y);
};

class ActivatingObject : public Actor // virtual class
{
public:
    ActivatingObject(int lifetime, StudentWorld* world, int iid, double startX, double startY, Direction dir, int depth = 1);
    virtual ~ActivatingObject();
    void decrementLifetime();
    int getLifetime() const;
private:
    int m_lifetime;
};

class Food : public ActivatingObject
{
public:
    Food(StudentWorld* world, double startX, double startY);
    virtual ~Food();
    virtual void doSomething();
    virtual bool isEdible() const;
};

class Projectile : public ActivatingObject
{
public:
    Projectile(int damage, int lifetime, StudentWorld* world, int iid, double startX, double startY, Direction dir);
    virtual ~Projectile();
    virtual void doSomething();
    int getDamage() const;
private:
    int m_damage;
};

class Spray : public Projectile
{
public:
    Spray(StudentWorld* world, double startX, double startY, Direction dir);
    virtual ~Spray();
};

class Flame : public Projectile
{
public:
    Flame(StudentWorld* world, double startX, double startY, Direction dir);
    virtual ~Flame();
};

class Goodie : public ActivatingObject
{
public:
    Goodie(int points, int lifetime, StudentWorld* world, int iid, double startX, double startY, Direction dir = 0, int depth = 1);
    virtual ~Goodie();
    virtual bool isDamageable() const;
    void pickUp(Socrates* s);
    virtual void specificGoodiePerk(Socrates* s) = 0;
    int getPoints() const;
    virtual void doSomething();
private:
    int m_points;
};

class RestoreHealthGoodie : public Goodie
{
public:
    RestoreHealthGoodie(int lifetime, StudentWorld* world, double startX, double startY);
    virtual ~RestoreHealthGoodie();
    virtual void specificGoodiePerk(Socrates* s);
};

class FlamethrowerGoodie : public Goodie
{
public:
    FlamethrowerGoodie(int lifetime, StudentWorld* world, double startX, double startY);
    virtual ~FlamethrowerGoodie();
    virtual void specificGoodiePerk(Socrates* s);
};

class ExtraLifeGoodie : public Goodie
{
public:
    ExtraLifeGoodie(int lifetime, StudentWorld* world, double startX, double startY);
    virtual ~ExtraLifeGoodie();
    virtual void specificGoodiePerk(Socrates* s);
};

class Fungus : public Goodie
{
public:
    Fungus(int lifetime, StudentWorld* world, double startX, double startY);
    virtual ~Fungus();
    virtual void specificGoodiePerk(Socrates* s);
};

class Dirt : public Actor
{
public:
    Dirt(StudentWorld* world, double startX, double startY);
    virtual ~Dirt();
    virtual void doSomething();
    virtual bool isDamageable() const;
    virtual bool isObstacle() const;
};

class Pit : public Actor
{
public:
    Pit(StudentWorld* world, double startX, double startY);
    virtual ~Pit();
    virtual void doSomething();
    virtual bool preventsLevelCompleting() const;
private:
    int m_inventory[3];
};

#endif // ACTOR_H_
