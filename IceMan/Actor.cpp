#include "Actor.h"
#include "StudentWorld.h"
#include <algorithm>
#include <random>

//////////////////////////////////////
// Actor implementations
//////////////////////////////////////

Actor::Actor(StudentWorld* swp, int imageID, int startX, int startY, Direction dir, double size, unsigned int depth)
	: GraphObject(imageID, startX, startY, dir, size, depth)
{
	m_world = swp;
	m_active = true;
	setVisible(true);
	setState(none);
}

StudentWorld* Actor::getWorld() const
{
	return m_world;
}

// Check if actor will be within bounds after moving in its current direction
bool Actor::withinBounds(int x, int y, Direction dir)
{
	bool canMove = true;

	if ((x <= 0 && dir == left) || (x >= VIEW_WIDTH - 4 && dir == right) || (y >= VIEW_HEIGHT - 4) && dir == up || (y <= 0 && dir == down))
		canMove = false;

	return canMove;
}

bool Actor::isDead()
{
	return (m_active == false);
}

void Actor::setDead()
{
	m_active = false;
}

Actor::State Actor::getState()
{
	return m_state;
}

void Actor::setState(Actor::State state)
{
	m_state = state;
}

void Actor::doSomething()
{
	if (isDead())
	{
		return;
	}
	if (!isPickupable())
	{
		doDifferentiatedStuff();
	}
}

void Actor::setLifetimeTicks(int ticks)
{
	m_lifetimeTicks = ticks;
}

int Actor::getLifetimeTicks()
{
	return m_lifetimeTicks;
}

void Actor::decLifetimeTicks()
{
	--m_lifetimeTicks;
}



//////////////////////////////////////////
// Ice implementations
//////////////////////////////////////////

Ice::Ice(StudentWorld* swp, int x, int y)
	: Actor(swp, IID_ICE, x, y, right, 0.25, 3)
{
	setState(none); 
}

void Ice::doDifferentiatedStuff() {};
bool Ice::isPickupable()
{
	return false;
}

//////////////////////////////////////////
// Person implementations
//////////////////////////////////////////

void Person::setHP(int hp)
{
	m_hitPoints = hp;
}

int Person::getHP()
{
	return m_hitPoints;
}

void Person::decHP(int loss)
{
	if ((m_hitPoints - loss) >= 0)
		m_hitPoints -= loss;
	else
		setHP(0);
}

bool Person::isDead()
{
	if (m_hitPoints == 0 && Actor::isDead() == false)
		setDead();
	return Actor::isDead();
}

////////////////////////////////////
// Iceman implementations
////////////////////////////////////

Iceman::Iceman(StudentWorld* swp) : Person(swp, IID_PLAYER, 30, 60, right, 1.0, 0)
{
	setState(none);
	setHP(10);
	m_unitsOfWater = 5;
	m_sonarCharges = 1;
	m_goldNuggets = 0;
	
}

void Iceman::dig()
{
	if (getWorld()->removeIceInFront(getX(), getY()) == true)
		getWorld()->playSound(SOUND_DIG);
}

int Iceman::getSquirtsLeftInSquirtGun()
{
	return m_unitsOfWater;
}

void Iceman::addSquirts()
{
	m_unitsOfWater += 5;
}

void Iceman::decSquirts()
{
	--m_unitsOfWater;
}

int Iceman::getGoldCount()
{
	return m_goldNuggets;
}

void Iceman::addGold()
{
	++m_goldNuggets;
}

void Iceman::decGold()
{
	if (m_goldNuggets - 1 >= 0)
		--m_goldNuggets;
	else
		m_goldNuggets = 0;
}

int Iceman::getSonarChargeCount()
{
	return m_sonarCharges;
}
void Iceman::addSonar()
{
	++m_sonarCharges;
}
void Iceman::decSonar()
{
	--m_sonarCharges;
}

void Iceman::doDifferentiatedStuff()
{
	dig();

	int ch;
	if (getWorld()->getKey(ch) == true)
	{
		// user hit a key this tick!
		switch (ch)
		{
		case KEY_PRESS_LEFT:
			if (getDirection() != left)
			{
				setDirection(left);
			}
			else if (withinBounds(getX(), getY(), left) && !getWorld()->willCollideWithBoulder(getX() - 1, getY()))
			{
				moveTo(getX() - 1, getY());
			}
			break;
		case KEY_PRESS_RIGHT:
			if (getDirection() != right)
			{
				setDirection(right);
			}
			else if (withinBounds(getX(), getY(), right) && !getWorld()->willCollideWithBoulder(getX() + 1, getY()))
			{
				moveTo(getX() + 1, getY());
			}
			break;
		case KEY_PRESS_UP:
			if (getDirection() != up)
			{
				setDirection(up);
			}
			else if (withinBounds(getX(), getY(), up) && !getWorld()->willCollideWithBoulder(getX(), getY() + 1))
			{
				moveTo(getX(), getY() + 1);
			}
			break;
		case KEY_PRESS_DOWN:
			if (getDirection() != down)
			{
				setDirection(down);
			}
			else if (withinBounds(getX(), getY(), down) && !getWorld()->willCollideWithBoulder(getX(), getY() - 1))
			{
				moveTo(getX(), getY() - 1);
			}
			break;
		case KEY_PRESS_SPACE:
		{
			if (m_unitsOfWater > 0)
			{
				getWorld()->playSound(SOUND_PLAYER_SQUIRT);
				decSquirts();
				if (withinBounds(getX(), getY(), getDirection()))
				{
					int sqX, sqY; // to hold coordinates for squirt
					switch (getDirection())
					{
					case left:
						sqX = getX() - 4;
						sqY = getY();
						break;
					case right:
						sqX = getX() + 4;
						sqY = getY();
						break;
					case up:
						sqX = getX();
						sqY = getY() + 4;
						break;
					case down:
						sqX = getX();
						sqY = getY() - 4;
						break;
					}

					if (!getWorld()->isIceAt(sqX, sqY) && !getWorld()->isIceAt(sqX + 3, sqY + 3))
					{
						if (withinBounds(sqX - 1, sqY - 1, getDirection()))
						{
							if (!getWorld()->willCollideWithBoulder(sqX, sqY))
							{
								getWorld()->placeActorOnField(new Squirt(getWorld(), sqX, sqY, getDirection()));
							}
						}
					}
				}
			}
			break;
		}
		case KEY_PRESS_TAB:
			if (getGoldCount() > 0)
			{
				decGold();
				getWorld()->placeActorOnField(new GoldNugget(Actor::temporary, getWorld(), getX(), getY()));
			}
			
			break;
		case 'z':
		case 'Z':
			if (m_sonarCharges > 0)
			{
				decSonar();
				getWorld()->useSonar();
				getWorld()->playSound(SOUND_SONAR);
			}
			break;
		case KEY_PRESS_ESCAPE:
			setHP(0);
			break;

		}
	}
}

void Iceman::annoy(int amt)
{
	decHP(amt);
}

//////////////////////////////////////
// Protester implementations
//////////////////////////////////////

Protester::Protester(StudentWorld* swp, int imageID) : Person(swp, imageID, 60, 60, left, 1.0, 0)
{
	setState(stay);
	decideSquaresToMove();

	int N = std::max(0, 3 - static_cast<int>(getWorld()->getLevel()) / 4);
	setRestingTicks(N);

	m_nonRestingTicks = 0;
	m_nonRestingTicksSinceLastShout = 15; // ready to shout
}

void Protester::decideSquaresToMove()
{
	std::default_random_engine engine(getWorld()->getScore());
	std::uniform_int_distribution<int> distributor(8, 60);

	m_numSquaresToMoveInCurrentDirection = distributor(engine);
}

void Protester::annoy(int amt)
{
	decHP(amt);
	if (getHP() > 0)
	{
		getWorld()->playSound(SOUND_PROTESTER_ANNOYED);

		// ""stun"/extend resting state
		int stun = std::max(50, 100 - static_cast<int>(getWorld()->getLevel()) * 10);
		m_ticksToWaitBetweenMoves = stun;
	}
	else
	{
		getWorld()->playSound(SOUND_PROTESTER_GIVE_UP);

		// set resting ticks to 0 so the protester will move next tick
		m_ticksToWaitBetweenMoves = 0;

		// increase score depending on what annoyed protester
		if (amt == 100)
			getWorld()->increaseScore(500);
		// if annoyed for 2 points, check in regular or hardcore annoy functions
		
	}
}


void Protester::doDifferentiatedStuff()
{
	if (m_ticksToWaitBetweenMoves != 0)
	{
		--m_ticksToWaitBetweenMoves;
		return;
	}
	else if (getState() == leave)
	{
		// TO DO maze solving algorithm
		setDead();
		getWorld()->protesterLeaves();
	}

	// no more ticks to wait
	else if (m_ticksToWaitBetweenMoves == 0)
	{
		// annoy iceman
		if (getWorld()->withinDistanceIceman(4.00, getX(), getY()))
		{
			if (getWorld()->facingTowardIceMan(this))
			{
				if (m_nonRestingTicksSinceLastShout >= 15)
				{
					getWorld()->playSound(SOUND_PROTESTER_YELL);
					getWorld()->annoyIceMan(2);
					m_nonRestingTicksSinceLastShout = 0; // reset shout ticks
					return;
				}
				else
					++m_nonRestingTicksSinceLastShout;

				++m_nonRestingTicks;
			}
		}

		// if not near iceman, then move or turn
		else if (!getWorld()->withinDistanceIceman(4.00, getX(), getY()))
		{
			++m_nonRestingTicks;
			++m_nonRestingTicksSinceLastShout;

			//chase iceman
			Direction chaseDir = getWorld()->lineOfSightToIceMan(this);
			if (chaseDir != none)
			{
				setDirection(chaseDir);

				switch (chaseDir)
				{
				case left:
					if (withinBounds(getX() - 1, getY(), left))
						moveTo(getX() - 1, getY());
					break;
				case right:
					if (withinBounds(getX() + 1, getY(), right))
						moveTo(getX() + 1, getY());
					break;
				case up:
					if (withinBounds(getX(), getY() + 1, up))
						moveTo(getX(), getY() + 1);
					break;
				case down:
					if (withinBounds(getX(), getY() - 1, down))
						moveTo(getX(), getY() - 1);
					break;
				}

				m_numSquaresToMoveInCurrentDirection = 0;
			}

			// if finished walking in direction, pick new direction
			else if (m_numSquaresToMoveInCurrentDirection <= 0)
			{

				static std::default_random_engine engine(getWorld()->getScore());
				static std::uniform_int_distribution<int> distributor(1, 4);
				int newDirection;

				bool clearPath = false;
				while (!clearPath)
				{
					newDirection = distributor(engine);

					switch (newDirection)
					{
					case 1:
						clearPath = (!getWorld()->isIceInFront(up, getX(), getY() + 1) && !getWorld()->willCollideWithBoulder(getX(), getY() + 1));
						if (clearPath && withinBounds(getX(), getY(), up))
							setDirection(up);
						break;
					case 2:
						clearPath = (!getWorld()->isIceInFront(down, getX(), getY() - 1) && !getWorld()->willCollideWithBoulder(getX(), getY() - 1));
						if (clearPath && withinBounds(getX(), getY(), down))
							setDirection(down);
						break;
					case 3:
						clearPath = (!getWorld()->isIceInFront(left, getX() - 1, getY()) && !getWorld()->willCollideWithBoulder(getX() - 1, getY()));
						if (clearPath && withinBounds(getX(), getY(), left))
							setDirection(left);
						break;
					case 4:
						clearPath = (!getWorld()->isIceInFront(right, getX() + 1, getY()) && !getWorld()->willCollideWithBoulder(getX() + 1, getY()));
						if (clearPath && withinBounds(getX(), getY(), right))
							setDirection(right);
						break;
					}
				}

				// determine new number of squares to move
				decideSquaresToMove();
			}

			// if at intersection and 200 non resting ticks pased
			if (m_nonRestingTicks >= 200)
			{
				bool changedDir = false;

				// if at up/down intersection
				if (getDirection() == left || getDirection() == right)
				{
					// if protester can move up
					if (!getWorld()->isIceInFront(up, getX(), getY() + 1))
					{
						if (!getWorld()->willCollideWithBoulder(getX(), getY() + 1))
						{
							if (withinBounds(getX(), getY(), up))
							{
								setDirection(up);
								changedDir = true;
							}
						}
					}
					// if protester can move down
					else if (!getWorld()->isIceInFront(down, getX(), getY() - 1))
					{
						if (!getWorld()->willCollideWithBoulder(getX(), getY() - 1))
						{
							if (withinBounds(getX(), getY(), down))
							{
								setDirection(down);
								changedDir = true;
							}
						}
					}
					// if protester can move up or down
					else if (!getWorld()->isIceInFront(up, getX(), getY() + 1) && !getWorld()->isIceInFront(down, getX(), getY() - 1))
					{
						if (!getWorld()->willCollideWithBoulder(getX(), getY() + 1) && !getWorld()->willCollideWithBoulder(getX(), getY() - 1))
						{
							if (withinBounds(getX(), getY(), up) && withinBounds(getX(), getY(), down))
							{
								int randomDir = rand() % 2 + 1;

								switch (randomDir)
								{
								case 1:
									setDirection(up);
									break;
								case 2:
									setDirection(down);
									break;
								}

								changedDir = true;
							}
						}
					}
				}
				// if at left/right intersection
				else if (getDirection() == up || getDirection() == down)
				{
					// if protester can move left
					if (!getWorld()->isIceInFront(left, getX() - 1, getY()))
					{
						if (!getWorld()->willCollideWithBoulder(getX() - 1, getY()))
						{
							if (withinBounds(getX(), getY(), left))
							{
								setDirection(left);
								changedDir = true;
							}
						}
					}
					// if protester can move right
					else if (!getWorld()->isIceInFront(right, getX() + 1, getY()))
					{
						if (!getWorld()->willCollideWithBoulder(getX() + 1, getY()))
						{
							if (withinBounds(getX(), getY(), right))
							{
								setDirection(right);
								changedDir = true;
							}
						}
					}
					// if protester can move left or right
					else if (!getWorld()->isIceInFront(left, getX() - 1, getY()) && !getWorld()->isIceInFront(right, getX() + 1, getY()))
					{
						if (!getWorld()->willCollideWithBoulder(getX() - 1, getY()) && !getWorld()->willCollideWithBoulder(getX() + 1, getY()))
						{
							if (withinBounds(getX(), getY(), left) && withinBounds(getX(), getY(), right))
							{
								int randomDir = rand() % 2 + 1;

								switch (randomDir)
								{
								case 1:
									setDirection(left);
									break;
								case 2:
									setDirection(right);
									break;
								}

								changedDir = true;
							}
						}
					}
				}

				if (changedDir)
				{
					decideSquaresToMove();
					m_nonRestingTicks = 0;
				}
			}
			
			// if okay to walk in direction he's facing
			if (m_numSquaresToMoveInCurrentDirection > 0 && withinBounds(getX(), getY(), getDirection()))
			{
				bool moved = false;
				switch (getDirection())
				{
				case left:
					if (!getWorld()->isIceInFront(getDirection(), getX() - 1, getY()) && !getWorld()->willCollideWithBoulder(getX() - 1, getY()))
					{
						moveTo(getX() - 1, getY());
						moved = true;
					}
					break;
				case right:
					if (!getWorld()->isIceInFront(getDirection(), getX() + 1, getY()) && !getWorld()->willCollideWithBoulder(getX() + 1, getY()))
					{
						moveTo(getX() + 1, getY());
						moved = true;
					}
					break;
				case up:
					if (!getWorld()->isIceInFront(getDirection(), getX(), getY() + 1) && !getWorld()->willCollideWithBoulder(getX(), getY() + 1))
					{
						moveTo(getX(), getY() + 1);
						moved = true;
					}
					break;
				case down:
					if (!getWorld()->isIceInFront(getDirection(), getX(), getY() - 1) && !getWorld()->willCollideWithBoulder(getX(), getY() - 1))
					{
						moveTo(getX(), getY() - 1);
						moved = true;
					}
					break;
				}

				if (!moved)
					m_numSquaresToMoveInCurrentDirection = 0; // must change direction next "tick"
				else
					--m_numSquaresToMoveInCurrentDirection;
			}

			// if he ran into ice or a boulder before finished walking in the direction
			else if (!withinBounds(getX(), getY(), getDirection()))
				m_numSquaresToMoveInCurrentDirection = 0;

			//reset resting ticks
			int N = std::max(0, 3 - static_cast<int>(getWorld()->getLevel()) / 4);
			setRestingTicks(N);
		}
	}

}

void Protester::leaveField()
{
	setState(leave);
}
void Protester::setRestingTicks(int ticks)
{
	m_ticksToWaitBetweenMoves = ticks;
}

int Protester::getRestingTicks()
{
	return m_ticksToWaitBetweenMoves;
}
int Protester::getSquaresToMove()
{
	return m_numSquaresToMoveInCurrentDirection;
}

///////////////////////////////////////
// Regular Protester implementations
///////////////////////////////////////

RegularProtester::RegularProtester(StudentWorld* swp) : Protester(swp, IID_PROTESTER)
{
	setHP(5);
}

void RegularProtester::addGold()
{
	getWorld()->playSound(SOUND_PROTESTER_FOUND_GOLD);
	getWorld()->increaseScore(25);
	leaveField();
}

void RegularProtester::annoy(int amt)
{
	Protester::annoy(amt);
	if (getHP() <= 0)
	{
		leaveField();

		if (amt == 2)
			getWorld()->increaseScore(100);
	}
}

///////////////////////////////////////
// Hardcore Protester implemenations
//////////////////////////////////////

HardcoreProtester::HardcoreProtester(StudentWorld* swp) : Protester(swp, IID_HARD_CORE_PROTESTER)
{
	setHP(20);
}
void HardcoreProtester::addGold()
{
	getWorld()->playSound(SOUND_PROTESTER_FOUND_GOLD);
	getWorld()->increaseScore(50);

	int ticksToStare = std::max(50, 100 - static_cast<int>(getWorld()->getLevel()) * 10);
	setRestingTicks(getRestingTicks() + ticksToStare);
}

void HardcoreProtester::annoy(int amt)
{
	Protester::annoy(amt);
	if (getHP() <= 0)
	{
		if (amt == 2)
			getWorld()->increaseScore(250);
	}
}

/////////////////////////////////////
// Goodie Implementation
////////////////////////////////////

Goodie::Goodie(State state, StudentWorld* swp, int imageID, int startX, int startY, Direction dir, double size, unsigned int depth)
: Actor(swp, imageID, startX, startY, dir, size, depth)
{
	setState(state);
}

bool Goodie::isPickupable()
{
	return true;
}

void Goodie::doSomething()
{
	Actor::doSomething();
	if (getState() == permanent)
	{
		if (!isVisible() && getWorld()->withinDistanceIceman(4.00, getX(), getY()))
		{
			setVisible(true);
		}
	}
	if (getWorld()->withinDistanceIceman(3.00, getX(), getY()) && !canBePickedUpByProtester())
	{
		setDead();
		setVisible(false);
		getWorld()->playSound(SOUND_GOT_GOODIE);
	}
	if (getState() == temporary)
	{
		if (getLifetimeTicks() <= 0)
			setDead();
		decLifetimeTicks();
	}
	doDifferentiatedStuff();
	
}


////////////////////////////////////
// Sonar Implementations
////////////////////////////////////

Sonar::Sonar(StudentWorld* swp) : Goodie(temporary, swp, IID_SONAR, 0, 60, right, 1.0, 2)
{
	int T = std::max(100, 300 - 10 * static_cast<int>(getWorld()->getLevel()));
	setLifetimeTicks(T);
}

void Sonar::doDifferentiatedStuff()
{
	if (getWorld()->isPickedUpByIceman(getX(), getY()))
	{
		getWorld()->increaseScore(75);
		getWorld()->addSonar();
	}
}

////////////////////////////////////
// GoldNugget Implementation
////////////////////////////////////

GoldNugget::GoldNugget(State state, StudentWorld* swp, int x, int y) : Goodie(state, swp, IID_GOLD, x, y, right, 1.0, 2)
{
	if (state == permanent)
	{
		setVisible(false);

	}
	else if (state == temporary)
	{
		setVisible(true);
	}
	setLifetimeTicks(100);
}

void GoldNugget::doDifferentiatedStuff()
{
	switch (getState())
	{
	case permanent:
		if (getWorld()->withinDistanceIceman(3.00, getX(), getY()))
		{
			getWorld()->increaseScore(10);
			getWorld()->addGold();
		}
		break;
	case temporary:
		if (getWorld()->searchNearbyPersons(this, 0, 3.00))
			setDead();
		break;
	}
}

bool GoldNugget::canBePickedUpByProtester()
{
	return (getState() == temporary);
}

/////////////////////////////////////
// Barrel of Oil Implementation
/////////////////////////////////////

Barrel::Barrel(StudentWorld* swp, int x, int y) : Goodie(permanent, swp, IID_BARREL, x, y, right, 1.0, 2)
{
	setVisible(false);
}


void Barrel::doDifferentiatedStuff()
{
	if (getWorld()->withinDistanceIceman(3.00, getX(), getY()))
	{
		getWorld()->increaseScore(1000);
		getWorld()->decBarrels();
	}
}


///////////////////////////////////////
// Water Pool
/////////////////////////////////////

Water::Water(StudentWorld* swp, int x, int y) : Goodie(temporary, swp, IID_WATER_POOL, x, y, right, 1.0, 2)
{
	int T = std::max(100, 300 - 10 * static_cast<int>(getWorld()->getLevel()));
	setLifetimeTicks(T);
}

void Water::doDifferentiatedStuff()
{
	if (getWorld()->withinDistanceIceman(3.00, getX(), getY()))
	{
		getWorld()->addSquirts();
		getWorld()->increaseScore(100);
	}
}


///////////////////////////////////
// Squirt Implementation
///////////////////////////////////

Squirt::Squirt(StudentWorld* swp, int x, int y, Direction dir) : Actor(swp, IID_WATER_SPURT, x, y, dir, 1.0, 1)
{
	setLifetimeTicks(4);
}

void Squirt::doDifferentiatedStuff()
{
	if (getWorld()->searchNearbyPersons(this, 2, 3.00))
	{
		setDead();
	}
	else if (getLifetimeTicks() <= 0 || !withinBounds(getX(), getY(), getDirection()) || getWorld()->willCollideWithBoulder(getX(), getY()))
	{
		setDead();
	}
	else if (withinBounds(getX(), getY(), getDirection()))
	{
		bool pathClear = true;

		switch (getDirection())
		{
		case left:
			pathClear = (!getWorld()->isIceInFront(getDirection(), getX() - 1, getY()));

			if (pathClear && !getWorld()->willCollideWithBoulder(getX() - 4, getY()))
				moveTo(getX() - 1, getY());
			break;
		case right:
			pathClear = (!getWorld()->isIceInFront(getDirection(), getX() + 4, getY()));

			if (pathClear && !getWorld()->willCollideWithBoulder(getX() + 4, getY()))
				moveTo(getX() + 1, getY());
			break;
		case up:
			pathClear = (!getWorld()->isIceInFront(getDirection(), getX(), getY()+4));

			if (pathClear && !getWorld()->willCollideWithBoulder(getX(), getY()+4))
				moveTo(getX(), getY() + 1);
			break;
		case down:
			pathClear = (!getWorld()->isIceInFront(getDirection(), getX(), getY()-1));

			if (pathClear && !getWorld()->willCollideWithBoulder(getX(), getY() - 4))
				moveTo(getX(), getY() - 1);
			break;
		}
		decLifetimeTicks();

		if (!pathClear)
			setDead();
	}
}



/////////////////////////////////
// Boulder
///////////////////////////////

Boulder::Boulder(StudentWorld* swp, int x, int y) : Actor(swp, IID_BOULDER, x, y, down, 1.0, 1)
{
	setState(stable);
}

void Boulder::doDifferentiatedStuff()
{
	if (getState() == stable && !getWorld()->isIceInFront(getDirection(), getX(), getY() - 1))
	{
		setState(waiting);
		setLifetimeTicks(30);
	}
	else if (getState() == waiting)
	{
		if (getLifetimeTicks() != 0)
			decLifetimeTicks();
		else
		{
			setState(falling);
			getWorld()->playSound(SOUND_FALLING_ROCK);
		}
	}
	else if (getState() == falling)
	{
		// annoy iceman and/or protesters
		if (getWorld()->withinDistanceIceman(3.00, getX(), getY()))
			getWorld()->annoyIceMan(100);

		getWorld()->searchNearbyPersons(this, 100, 3.00);

		// fall until collision
		if (!getWorld()->isIceInFront(getDirection(), getX(), getY() - 1) && withinBounds(getX(), getY(), down) && !getWorld()->willCollideWithBoulder(getX(), getY() - 4))
			moveTo(getX(), getY() - 1);
		else
			setDead();
	}
}
