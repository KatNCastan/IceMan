#ifndef ACTOR_H_
#define ACTOR_H_

#include "GraphObject.h"

class StudentWorld;

class Actor : public GraphObject
{
public:

	enum State { none, stay, leave, permanent, temporary, stable, waiting, falling };

	Actor(StudentWorld* swp, int imageID, int startX, int startY, Direction dir, double size, unsigned int depth);
	virtual ~Actor() {};
	
	virtual void annoy(int amt) = 0; // annoy this actor
	virtual void addGold() = 0;
	virtual bool isDead();
	virtual void doSomething();
	virtual bool isPickupable() = 0;
	virtual bool isSolid() = 0;
	virtual bool canPickUpAndBeAnnoyed() const = 0;

protected:
	void setLifetimeTicks(int ticks);
	int getLifetimeTicks();
	void decLifetimeTicks();

	bool withinBounds(int x, int y, Direction dir);
	StudentWorld* getWorld() const;
	virtual void setDead();
	State getState();
	void setState(State state);

private:
	virtual void doDifferentiatedStuff() = 0;

	StudentWorld* m_world;
	bool m_active;
	State m_state;
	int m_lifetimeTicks;
};

class Ice : public Actor
{
public:
	Ice(StudentWorld* swp, int x, int y);
	virtual ~Ice() {};
	virtual void doDifferentiatedStuff() override;
	virtual bool isPickupable();
	virtual bool isSolid() { return true; };
	virtual bool canPickUpAndBeAnnoyed() const override { return false; };
	virtual void annoy(int amt) override {};
	virtual void addGold() override {};
};

class Person : public Actor
{
private:
	int m_hitPoints;
protected:
	void setHP(int hp);
public:
	using Actor::Actor;
	virtual ~Person() {};

	int getHP();
	void decHP(int loss);
	virtual bool isDead() override;
	virtual bool isPickupable() { return false; };
	virtual bool isSolid() { return false; }; // move to cpp?
	virtual bool canPickUpAndBeAnnoyed() const override { return true; };
};

class Iceman : public Person
{
private:
	int m_unitsOfWater;
	int m_sonarCharges;
	int m_goldNuggets;

	void dig();

public:
	Iceman(StudentWorld* swp);
	virtual ~Iceman() {};

	int getSquirtsLeftInSquirtGun();
	void addSquirts();
	void decSquirts();
	int getGoldCount();
	virtual void addGold() override;
	void decGold();
	int getSonarChargeCount();
	void addSonar();
	void decSonar();
	virtual void doDifferentiatedStuff() override;
	virtual void annoy(int amt) override;

};

class Protester : public Person
{
public:
	Protester(StudentWorld* swp, int imageID);
	virtual ~Protester() {};

	virtual void annoy(int amt) override;
	virtual void addGold() = 0;

	void decideSquaresToMove();
	virtual void doDifferentiatedStuff() override;
	void leaveField();
	void setRestingTicks(int ticks);
	int getRestingTicks();
	int getSquaresToMove();

private:
	int m_numSquaresToMoveInCurrentDirection;
	int m_ticksToWaitBetweenMoves;
	
	int m_nonRestingTicks;
	int m_nonRestingTicksSinceLastShout;
};

class RegularProtester : public Protester
{
public:
	RegularProtester(StudentWorld* swp);
	virtual ~RegularProtester() {};

	virtual void annoy(int amt) override;
	virtual void addGold() override;
};

class HardcoreProtester : public Protester
{
public:
	HardcoreProtester(StudentWorld* swp);
	virtual ~HardcoreProtester() {};

	virtual void annoy(int amt) override;
	virtual void addGold() override;

	void trackIceman(); // hardcore only
};

class Goodie : public Actor
{
public:

	Goodie(State state, StudentWorld* swp, int imageID, int startX, int startY, Direction dir, double size, unsigned int depth);
	virtual ~Goodie() {};

	virtual void addGold() override {};
	virtual void annoy(int amt) override {};
	virtual void doSomething();
	virtual void doDifferentiatedStuff() = 0;
	virtual bool isPickupable();
	virtual bool isSolid() { return false; }; // move to cpp?
	virtual bool canPickUpAndBeAnnoyed()  const override { return false; };
	virtual bool canBePickedUpByProtester() = 0;

};

class GoldNugget : public Goodie
{
public:
	GoldNugget(State state, StudentWorld* swp, int x, int y);
	virtual ~GoldNugget() {};

	virtual void doDifferentiatedStuff() override;
	virtual bool canBePickedUpByProtester();
};

class Sonar : public Goodie
{
public:
	Sonar(StudentWorld* swp);
	virtual ~Sonar() {};

	virtual void doDifferentiatedStuff() override;
	virtual bool canBePickedUpByProtester() { return false; };
};

class Barrel : public Goodie
{
public:
	Barrel(StudentWorld* swp, int x, int y);
	virtual ~Barrel() {};

	virtual void doDifferentiatedStuff() override;
	virtual bool canBePickedUpByProtester() { return false; };
};

class Water : public Goodie
{
public:
	Water(StudentWorld* swp, int x, int y);
	virtual ~Water() {};

	virtual void doDifferentiatedStuff() override;
	virtual bool canBePickedUpByProtester() { return false; };
};

class Squirt : public Actor
{
public:
	Squirt(StudentWorld* swp, int x, int y, Direction dir);
	virtual ~Squirt() {};

	virtual void annoy(int amt) override {}; // annoy this actor
	virtual void addGold() override  {};
	virtual bool isPickupable() override { return false; };
	virtual bool canPickUpAndBeAnnoyed() const override { return false; };
	virtual void doDifferentiatedStuff() override;
	virtual bool isSolid() override { return false; };
};

class Boulder : public Actor
{
public:
	Boulder(StudentWorld* swp, int x, int y);
	virtual ~Boulder() {};

	virtual void annoy(int amt) {}; // annoy this actor
	virtual void addGold() {};
	virtual bool isPickupable() { return false; };
	virtual bool canPickUpAndBeAnnoyed() const { return false; };
	virtual void doDifferentiatedStuff() override;
	virtual bool isSolid() { return true; };
};

#endif // ACTOR_H_
