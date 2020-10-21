#include "Actor.h"
#include "StudentWorld.h"

// Students:  Add code to this file, Actor.h, StudentWorld.h, and StudentWorld.cpp

#include <cmath>
using namespace std;

Actor::Actor(StudentWorld* world, int iid, double startX, double startY, Direction dir, int depth) : GraphObject(iid, startX, startY, dir, depth)
{
    m_world = world;
    m_isAlive = true;
};

Actor::~Actor() {}

bool Actor::isAlive() const
{
    if (m_isAlive) return true;
    else return false;
}

void Actor::setDead()
{
    m_isAlive = false;
}

bool Actor::checkOverlap(Actor *a)
{
    double distance = sqrt(pow((getX() - a->getX()), 2) + pow((getY() - a->getY()), 2));
    if (distance <= SPRITE_RADIUS * 2) return true;
    return false;
}

StudentWorld* Actor::getWorld() const
{
    return m_world;
}

bool Actor::isDamageable() const { return false; }
bool Actor::isObstacle() const { return false; }
bool Actor::isEdible() const { return false; }
bool Actor::preventsLevelCompleting() const { return false; }

void Actor::incurDamage(int damage)
{
    setDead();
}

ActivatingObject::ActivatingObject(int lifetime, StudentWorld* world, int iid, double startX, double startY, Direction dir, int depth) : Actor(world, iid, startX, startY, dir, depth)
{
    m_lifetime = lifetime;
};

ActivatingObject::~ActivatingObject() {};

int ActivatingObject::getLifetime() const
{
    return m_lifetime;
}

void ActivatingObject::decrementLifetime()
{
    m_lifetime--;
    if (m_lifetime <= 0) {
        setDead();
    }
}

Projectile::Projectile(int damage, int lifetime, StudentWorld* world, int iid, double startX, double startY, Direction dir) : ActivatingObject(lifetime, world, iid, startX, startY, dir)
{
    m_damage = damage;
};

Projectile::~Projectile() {};

void Projectile::doSomething()
{
    if (!isAlive()) return;
    if (getWorld()->inflictDamage(this))
    {
        setDead();
        return;
    }
    moveAngle(getDirection(), SPRITE_WIDTH);
    decrementLifetime();
}

int Projectile::getDamage() const { return m_damage; }

Food::Food(StudentWorld* world, double startX, double startY) : ActivatingObject(1, world, IID_FOOD, startX, startY, 90)
{};

Food::~Food() {}

void Food::doSomething()
{
    return;
}

Spray::Spray(StudentWorld* world, double startX, double startY, Direction dir) : Projectile(2, 112/SPRITE_WIDTH, world, IID_SPRAY, startX, startY, dir)
{};

bool Food::isEdible() const { return true; }

Spray::~Spray() {}

Flame::Flame(StudentWorld* world, double startX, double startY, Direction dir) : Projectile(5, 32/SPRITE_WIDTH, world, IID_FLAME, startX, startY, dir)
{};

Flame::~Flame() {}

Dirt::Dirt(StudentWorld* world, double startX, double startY) : Actor(world, IID_DIRT, startX, startY, 0, 1)
{
};

Dirt::~Dirt() {}

void Dirt::doSomething() { return; }

bool Dirt::isObstacle() const { return true; }

bool Dirt::isDamageable() const { return true; }

Pit::Pit(StudentWorld* world, double startX, double startY) : Actor(world, IID_PIT, startX, startY, 0, 1)
{
    m_inventory[0] = 5;
    m_inventory[1] = 3;
    m_inventory[2] = 2;
}

Pit::~Pit() {}

bool Pit::preventsLevelCompleting() const
{
    return true;
}

void Pit::doSomething()
{
    bool isEmpty = true;
    for (int i = 0; i < 3; i++)
    {
        if (m_inventory[i] > 0)
        {
            isEmpty = false;
            break;
        }
    }
    
    if (isEmpty)
    {
        setDead();
    }
    else
    {
        int spawnChance = randInt(0, 49);
        if (spawnChance == 0)
        {
            int spawnType = randInt(0, 2);
            while (m_inventory[spawnType] == 0)
            {
                spawnType = randInt(0, 2);
            }
            if (spawnType == 0) getWorld()->addActor(new RegularSalmonella(getWorld(), getX(), getY()));
            else if (spawnType == 1) getWorld()->addActor(new AggressiveSalmonella(getWorld(), getX(), getY())); // have to change this
            else getWorld()->addActor(new EColi(getWorld(), getX(), getY()));
            m_inventory[spawnType]--;
            getWorld()->playSound(SOUND_BACTERIUM_BORN);
        }
    }
}

Bacterium::Bacterium(int damage, int hp, StudentWorld* world, int iid, double startX, double startY) : Agent(hp, world, iid, startX, startY, 90, 0)
{
    m_movementPlanDistance = 0;
    m_damage = damage;
    m_foodEaten = 0;
}

Bacterium::~Bacterium() {}

bool Bacterium::preventsLevelCompleting() const { return true; }

bool Bacterium::isDamageable() const { return true; }

int Bacterium::getDamage() const
{
    return m_damage;
}

void Agent::incurDamage(int damage)
{
    setHP(getHP() - damage);
    hurt();
}

void Salmonella::hurt()
{
    if (getHP() > 0) getWorld()->playSound(SOUND_SALMONELLA_HURT);
    else
    {
        setDead();
        getWorld()->playSound(SOUND_SALMONELLA_DIE);
        getWorld()->increaseScore(100);
        int spawnFood = randInt(0, 1);
        if (spawnFood == 0) getWorld()->addActor(new Food(getWorld(), getX(), getY()));
    }
}

void Socrates::hurt()
{
    if (getHP() > 0) getWorld()->playSound(SOUND_PLAYER_HURT);
    else
    {
        getWorld()->playSound(SOUND_PLAYER_DIE);
        setDead();
    }
}

void EColi::hurt()
{
    if (getHP() > 0) getWorld()->playSound(SOUND_ECOLI_HURT);
    else
    {
        setDead();
        getWorld()->playSound(SOUND_ECOLI_DIE);
        getWorld()->increaseScore(100);
        int spawnFood = randInt(0, 1);
        if (spawnFood == 0) getWorld()->addActor(new Food(getWorld(), getX(), getY()));
    }
}

void Bacterium::setMovePlan(int newPlan) { m_movementPlanDistance = newPlan; }
int Bacterium::getMovePlan() const { return m_movementPlanDistance; }

void Bacterium::doSomething()
{
    if (!getWorld()->damageSocrates(this))
    {
        // step 3 - reproduce
        if (m_foodEaten >= 3)
        {
            double newX = getX();
            if (getX() < VIEW_WIDTH/2) newX += SPRITE_WIDTH/2;
            else if (getX() > VIEW_WIDTH/2) newX -= SPRITE_WIDTH/2;
            double newY = getY();
            if (getY() < VIEW_HEIGHT/2) newY += SPRITE_WIDTH/2;
            else if (getY() > VIEW_HEIGHT/2) newY -= SPRITE_WIDTH/2;

            getWorld()->playSound(SOUND_BACTERIUM_BORN);
            getWorld()->addActor(specializedSpawn(newX, newY)); // idk about this
            m_foodEaten = 0;
        }
        else // step 4 - eat food
        {
            if (getWorld()->eatFood(this)) m_foodEaten++;
        }
    }
}
//
void RegularSalmonella::doSomething()
{
    if (!isAlive()) return;
    Bacterium::doSomething();
    Salmonella::doSomething();
}

Salmonella::Salmonella(int damage, int hp, StudentWorld* world, double startX, double startY) : Bacterium(damage, hp, world, IID_SALMONELLA, startX, startY)
{
}

Salmonella::~Salmonella() {}

void Salmonella::doSomething()
{
    const double PI = 4 * atan(1);
    
    // step 5
    if (getMovePlan() > 0)
    {
        setMovePlan(getMovePlan() - 1);
        double newX = getX() + 3 * cos(getDirection() * PI / 180.0);
        double newY = getY() + 3 * sin(getDirection() * PI / 180.0);
        double distanceFromCenter = sqrt(pow(newX - VIEW_WIDTH/2, 2) + pow(newY - VIEW_HEIGHT/2, 2));
        if (getWorld()->blockMovement(newX, newY) || distanceFromCenter >= VIEW_RADIUS)
        {
            setDirection(randInt(0, 359));
            setMovePlan(10);
        }
        else moveAngle(getDirection(), 3);
        return;
    }
    else // step 6
    {
        int newDir = getWorld()->locateFood(getX(), getY());
        if (newDir > 0)
        {
            double newX = getX() + 3 * cos(newDir * PI / 180.0);
            double newY = getY() + 3 * sin(newDir * PI / 180.0);
            if (!getWorld()->blockMovement(newX, newY))
            {
                setDirection(newDir);
                moveAngle(newDir, 3);
                return;
            }
        }
        setDirection(randInt(0, 359));
        setMovePlan(10);
        return;
    }
}

RegularSalmonella::RegularSalmonella(StudentWorld* world, double startX, double startY) : Salmonella(1, 4, world, startX, startY) {}
RegularSalmonella::~RegularSalmonella() {}

Bacterium* RegularSalmonella::specializedSpawn(double x, double y) // iffy
{
    return new RegularSalmonella(getWorld(), x, y);
}

AggressiveSalmonella::AggressiveSalmonella(StudentWorld* world, double startX, double startY) : Salmonella(2, 10, world, startX, startY) {}
AggressiveSalmonella::~AggressiveSalmonella() {}

void AggressiveSalmonella::doSomething()
{
    const double PI = 4 * atan(1);
    
    if (!isAlive()) return;
    
    bool socratesFound = false;
    int newDir = getWorld()->locateSocrates(getX(), getY(), 72);
    if (newDir > 0)
    {
        socratesFound = true;
        
        double newX = getX() + 3 * cos(newDir * PI / 180.0);
        double newY = getY() + 3 * sin(newDir * PI / 180.0);
        
        setDirection(newDir);
        if (!getWorld()->blockMovement(newX, newY))
        {
            moveAngle(newDir, 3);
        }
    }
    
    Bacterium::doSomething();
    
    if (socratesFound) return;
    else Salmonella::doSomething();
    
}

Bacterium* AggressiveSalmonella::specializedSpawn(double x, double y) { return new AggressiveSalmonella(getWorld(), x, y); }

EColi::EColi(StudentWorld* world, double startX, double startY) : Bacterium(4, 5, world, IID_ECOLI, startX, startY)
{
}

EColi::~EColi() {}

void EColi::doSomething()
{
    const double PI = 4 * atan(1);
    
    if (!isAlive()) return;
    
    Bacterium::doSomething();
    
    int newDir = getWorld()->locateSocrates(getX(), getY(), 256);
    if (newDir > 0)
    {
        setMovePlan(10);
        
        double newX = getX() + 2 * cos(newDir * PI / 180.0);
        double newY = getY() + 2 * sin(newDir * PI / 180.0);
        
        setDirection(newDir);
        
        while (getMovePlan() >= 0)
        {
            if (!getWorld()->blockMovement(newX, newY))
            {
                moveAngle(newDir, 2);
                return;
            }
            else
            {
                setMovePlan(getMovePlan() - 1);
                newDir = (newDir + 10) % 360;
            }
        }
        
        return;
    }
}

Bacterium* EColi::specializedSpawn(double x, double y) { return new EColi(getWorld(), x, y); }

Goodie::Goodie(int points, int lifetime, StudentWorld* world, int iid, double startX, double startY, Direction dir, int depth) : ActivatingObject(lifetime, world, iid, startX, startY, dir, depth)
{
    m_points = points;
};

Goodie::~Goodie() {}

bool Goodie::isDamageable() const { return true; }

int Goodie::getPoints() const { return m_points; }

// action that occurs upon goodie pick up
void Goodie::pickUp(Socrates* s)
{
    // increment points - todo
    getWorld()->increaseScore(getPoints());
    setDead();
    specificGoodiePerk(s);
}

void Goodie::doSomething()
{
    if (!isAlive()) return;
    if (getWorld()->pickUpGoodie(this)) return;
    decrementLifetime();
}

RestoreHealthGoodie::RestoreHealthGoodie(int lifetime, StudentWorld* world, double startX, double startY) : Goodie(250, lifetime, world, IID_RESTORE_HEALTH_GOODIE, startX, startY)
{
};

RestoreHealthGoodie::~RestoreHealthGoodie() {}

void RestoreHealthGoodie::specificGoodiePerk(Socrates* s)
{
    getWorld()->playSound(SOUND_GOT_GOODIE);
    s->setHP(100); // maxHealth
}

FlamethrowerGoodie::FlamethrowerGoodie(int lifetime, StudentWorld* world, double startX, double startY) : Goodie(300, lifetime, world, IID_FLAME_THROWER_GOODIE, startX, startY)
{
};

FlamethrowerGoodie::~FlamethrowerGoodie() {}

void FlamethrowerGoodie::specificGoodiePerk(Socrates* s)
{
    getWorld()->playSound(SOUND_GOT_GOODIE);
    s->addFlameCharges(5);
}

ExtraLifeGoodie::ExtraLifeGoodie(int lifetime, StudentWorld* world, double startX, double startY) : Goodie(500, lifetime, world, IID_EXTRA_LIFE_GOODIE, startX, startY)
{
};

ExtraLifeGoodie::~ExtraLifeGoodie() {}

void ExtraLifeGoodie::specificGoodiePerk(Socrates* s)
{
    getWorld()->playSound(SOUND_GOT_GOODIE);
    getWorld()->incLives();
}

Fungus::Fungus(int lifetime, StudentWorld* world, double startX, double startY) : Goodie(-50, lifetime, world, IID_FUNGUS, startX, startY)
{
};

Fungus::~Fungus() {}

void Fungus::specificGoodiePerk(Socrates* s)
{
    s->incurDamage(20);
}

Agent::Agent(int hp, StudentWorld* world, int iid, double startX, double startY, Direction dir, int depth) : Actor(world, iid, startX, startY)
{
    m_hp = hp;
};

Agent::~Agent() {}

void Agent::setHP(int newHP)
{
    m_hp = newHP;
}

int Agent::getHP() const { return m_hp; }

Socrates::Socrates(StudentWorld* world) : Agent(100, world, IID_PLAYER, 0, VIEW_HEIGHT/2)
{
    m_sprayCharges = 20;
    m_flameCharges = 5;
};

Socrates::~Socrates() {}

void Socrates::addFlameCharges(int charges)
{
    m_flameCharges += charges;
}

int Socrates::getFlameChargeCount() const { return m_flameCharges; }
int Socrates::getSprayChargeCount() const { return m_sprayCharges; }

void Socrates::doSomething()
{
    const double PI = 4 * atan(1);
    int ch;
    if (getWorld()->getKey(ch))
    {
        switch(ch)
        {
            case KEY_PRESS_SPACE:
            {
                if (m_sprayCharges >= 1)
                {
                    double x = getX() + (SPRITE_WIDTH * cos(getDirection() * PI / 180.0)); // this seems farther away than the skeleton game
                    double y = getY() + (SPRITE_WIDTH * sin(getDirection() * PI / 180.0));
                    getWorld()->addActor(new Spray(getWorld(), x, y, getDirection()));
                    
                    // decrement spray charges
                    m_sprayCharges--;
                    
                    // play sound
                    getWorld()->playSound(SOUND_PLAYER_SPRAY);
                }
                break;
            }
            case KEY_PRESS_ENTER:
            {
                if (m_flameCharges >= 1)
                {
                    int theta = 0;
                    double x = 0;
                    double y = 0;
                    for (int i = 0; i < 16; i++) // magic #
                    {
                        // spawn flames - todo
                        x = getX() + SPRITE_WIDTH * cos(theta * PI / 180.0);
                        y = getY() + SPRITE_WIDTH * sin(theta * PI / 180.0);
                        getWorld()->addActor(new Flame(getWorld(), x, y, theta));
                        theta += 22;
                    }
                    
                    // decrement # of flame charges
                    m_flameCharges--;
                    
                    // play sound
                    getWorld()->playSound(SOUND_PLAYER_FIRE);
                }
                break;
            }
            case KEY_PRESS_LEFT:
            {
                // convert to polar & change coordinate plane so that (0,0) is located at center of petri dish
                double currentX = getX() - VIEW_WIDTH/2;
                double currentY = getY() - VIEW_HEIGHT/2;
                double currentTheta;
                if (currentX == 0 && currentY > 0) currentTheta = PI / 2; // prevent division by zero
                else if (currentX == 0 && currentY < 0) currentTheta = 3 * PI / 2;
                else currentTheta = atan(currentY/currentX);
                if (currentX < 0) currentTheta = PI + currentTheta;
                double newTheta = currentTheta + (5.0/180) * PI;
                double newX = VIEW_WIDTH/2 * cos(newTheta);
                double newY = VIEW_HEIGHT/2 * sin(newTheta);
                
                // revert coordinate plane
                int thetaInDegrees = newTheta * 180.0/PI;
                Direction newDir = (180 + thetaInDegrees) % 360;
                setDirection(newDir);
                moveTo(newX + VIEW_WIDTH/2, newY + VIEW_HEIGHT/2);
                break;
            }
            case KEY_PRESS_RIGHT:
            {
                // convert to polar & change coordinate plane so that (0,0) is located at center of petri dish
                double currentX = getX() - VIEW_WIDTH/2;
                double currentY = getY() - VIEW_HEIGHT/2;
                double currentTheta;
                if (currentX == 0 && currentY > 0) currentTheta = PI / 2; // prevent division by zero
                else if (currentX == 0 && currentY < 0) currentTheta = 3 * PI / 2;
                else currentTheta = atan(currentY/currentX);
                if (currentX < 0) currentTheta = PI + currentTheta;
                double newTheta = currentTheta - (5.0/180) * PI;
                double newX = VIEW_WIDTH/2 * cos(newTheta);
                double newY = VIEW_HEIGHT/2 * sin(newTheta);
                
                // revert coordinate plane
                int thetaInDegrees = newTheta * 180.0/PI;
                Direction newDir = (180 + thetaInDegrees) % 360;
                setDirection(newDir);
                moveTo(newX + VIEW_WIDTH/2, newY + VIEW_HEIGHT/2);
                break;
            }
        }
    }
    else
    {
        if (m_sprayCharges < 20) m_sprayCharges++;
    }
}
