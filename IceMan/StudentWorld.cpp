#include "StudentWorld.h"
#include "Actor.h"
#include <string>
#include <sstream>
#include <iomanip>
#include <random>
#include <algorithm>
#include <queue>
using namespace std;


GameWorld* createStudentWorld(string assetDir)
{
	return new StudentWorld(assetDir);
}

///////////////////////////////////////////
// StudentWorld implementations
///////////////////////////////////////////

StudentWorld::StudentWorld(std::string assetDir) : GameWorld(assetDir)
{
}

StudentWorld::~StudentWorld()
{
	freeDynamicallyAllocatedData();
}

void StudentWorld::freeDynamicallyAllocatedData()
{
	delete m_iceman;

	Ice* p = m_iceField[0][0];
	for (int i = 0; i < VIEW_WIDTH; ++i)
	{
		for (int j = 0; j < VIEW_HEIGHT - 4; ++j)
		{
			p = m_iceField[i][j];
			m_iceField[i][j] = nullptr;
			delete p;
		}
	}

	for_each(m_gameObjects.begin(), m_gameObjects.end(), [](auto actor) {delete actor; });
	m_gameObjects.clear();
}

void StudentWorld::allActorsDoSomething()
{
	for (size_t i = 0; i < m_gameObjects.size(); ++i)
		m_gameObjects[i]->doSomething();
	if (m_iceman->isDead() == false)
		m_iceman->doSomething();
}

bool StudentWorld::isPickedUpByIceman(int x, int y)
{
	return withinDistanceActor(3.00, x, y, m_iceman);
}

void StudentWorld::decBarrels()
{
	--m_barrels;
}

int StudentWorld::getBarrelsRemaining()
{
	return m_barrels;
}

// checks if objects in newly initialized field are in a tunnel or overlapping other game objects
bool StudentWorld::isOverlappingObjects(int x, int y)
{
	bool overlap = false;

	// check if in tunnel
	if (x > 26 && x < 34 && y > 0) // if x coordinate is in tunnel or up to 3 squares to the left of tunnel. Check if y is up to 3 squares below tunnel
		overlap = true;

	// check if in radius of other objects
	if (!m_gameObjects.empty())
	{
		for (unsigned int n = 0; n < m_gameObjects.size(); ++n)
		{
			if (withinDistanceActor(6.00, x, y, m_gameObjects[n]))
				overlap = true;
		}
	}
	
	return overlap;
}

bool StudentWorld::withinDistanceIceman(double radius, int h, int k)
{
	return withinDistanceActor(radius, h, k, m_iceman);
}

bool StudentWorld::withinDistanceActor(double radius, int h, int k, Actor* a)
{
	// use pythagorean theorem to find radius between two points:
	// the origin of the object that called this fuction, (h, k), and the nearby actor's coordinates, (ax, ay)
	int ax = a->getX(); int ay = a->getY();

	double distance = sqrt(pow((ax - h), 2) + pow((ay - k), 2));
	
	return (distance <= radius);
}

void StudentWorld::useSonar()
{
	for_each(m_gameObjects.begin(), m_gameObjects.end(), [this](Actor* a)
	{
		if (a->isPickupable())
		{
			if (StudentWorld::withinDistanceIceman(12.00, a->getX(), a->getY()))
				a->setVisible(true);
		}
	});
}

GraphObject::Direction  StudentWorld::lineOfSightToIceMan(Actor* a)
{
	bool clearPath = true;
	GraphObject::Direction protesterDir = GraphObject::none;

	if (a->getX() == m_iceman->getX()) // lined up vertically
	{
		int yDistance = a->getY() - m_iceman->getY();

		if (yDistance > 0)
		{
			protesterDir = GraphObject::down;

			for (int n = a->getY(); n >= m_iceman->getY() + 4; --n)
			{
				if (isIceInFront(protesterDir, a->getX(), n))
					clearPath = false;
				if (willCollideWithBoulder(a->getX(), n))
					clearPath = false;
				if (!clearPath)
					break;
			}
		}
		else
		{
			protesterDir = GraphObject::up;

			for (int n = a->getY() + 4; n <= m_iceman->getY(); ++n)
			{
				if (isIceInFront(protesterDir, a->getX(), n))
					clearPath = false;
				if (willCollideWithBoulder(a->getX(), n))
					clearPath = false;
				if (!clearPath)
					break;
			}

		}
		
	}
	else if (a->getY() == m_iceman->getY()) // lined up horizontally
	{
		int xDistance = a->getX() - m_iceman->getX();

		if (xDistance > 0)
		{
			protesterDir = GraphObject::left;

			for (int n = 1; n <= xDistance; ++n)
			{
				if (isIceInFront(protesterDir, a->getX() + n, a->getY()))
					clearPath = false;
				if (willCollideWithBoulder(a->getX() + n, a->getY()))
					clearPath = false;
				if (!clearPath)
					break;
			}
		}
		else
		{
			protesterDir = GraphObject::right;

			for (int n = 4; n >= xDistance; --n)
			{
				if (isIceInFront(protesterDir, a->getX() + n, a->getY()))
					clearPath = false;
				if (willCollideWithBoulder(a->getX() + n, a->getY()))
					clearPath = false;
				if (!clearPath)
					break;
			}
		}
	}

	if (!clearPath)
		protesterDir = GraphObject::none;

	return protesterDir;
}

void StudentWorld::protesterLeaves()
{
	--m_protesters;
}

void StudentWorld::findExit()
{
	//scan for leaving protesters from exit
	queue<MazeTracker*> path;

	m_maze[60][60]->hasVisited = true;
	path.push(m_maze[60][60]);

	int i = 60; int j = 60;
	while (!path.empty())
	{
		

	}
}

string StudentWorld::formatText(int le, int li, int h, int sq, int g, int b, int so, int sc)
{
	ostringstream oss;
	oss << "Lvl: " << setw(2) << le << "  Lives: " << setw(1) << li << "  Hlth: " << setw(3) << h << "%"
		<< "  Wtr: " << setw(2) << sq << "  Gld: " << setw(2) << g << "  Oil Left: " << setw(2) << b
		<< "  Sonar: " << setw(2) << so << "  Scr: " << setw(6) << setfill('0') << sc;
	return oss.str();
}

void StudentWorld::setDisplayText()
{
	int level = getLevel();
	int lives = getLives();
	int health = 10 * m_iceman->getHP(); 
	int squirts = m_iceman->getSquirtsLeftInSquirtGun();
	int gold = m_iceman->getGoldCount();
	int barrelsLeft = getBarrelsRemaining();
	int sonar = m_iceman->getSonarChargeCount();
	int score = getScore();
	
	string s = formatText(level, lives, health, squirts, gold, barrelsLeft, sonar, score);
	
	setGameStatText(s); // calls our provided GameWorld::setGameStatText
}

void StudentWorld::removeDeadGameObjects()
{
	auto toRemove = find_if(m_gameObjects.begin(), m_gameObjects.end(), [](auto obj) {return obj->isDead(); });

	if (toRemove != m_gameObjects.end())
	{
		delete (*toRemove);
		m_gameObjects.erase(toRemove);
	}
}

void StudentWorld::addSquirts()
{
	m_iceman->addSquirts();
}
void StudentWorld::addGold()
{
	m_iceman->addGold();
}
void StudentWorld::addSonar()
{
	m_iceman->addSonar();
}

void StudentWorld::placeActorOnField(Actor* a)
{
	m_gameObjects.push_back(a);
}

// Determines if a tunnel should be dug at the given coordinates. Returns true if the coordinates of the Ice are within range of the tunnel
bool StudentWorld::isTunnel(int x, int y)
{
	return (x > 29 && x < 34 && y > 3 && y < VIEW_HEIGHT-SPRITE_HEIGHT);
}

// Determines if Ice is at the specified coordinates
bool StudentWorld::isIceAt(int x, int y) 
{
	// Verifies if coordinates are within ice field
	if (x >= 0 && x < VIEW_WIDTH && y >= 0 && y < VIEW_HEIGHT-SPRITE_HEIGHT)
		return m_iceField[x][y] != nullptr;
	return false;
}

bool StudentWorld::isIcelessSpot(int x, int y)
{
	bool iceless = true;
	for (int i = x; i < x + SPRITE_WIDTH; ++i)
	{
		for (int j = y; j < y + SPRITE_HEIGHT; ++j)
		{
			if (isIceAt(i, j))
				iceless = false;
		}
	}
	return iceless;
}

// coordinates can be within the actor's 4x4 sprite or can be in front
bool StudentWorld::isIceInFront(GraphObject::Direction dir, int x, int y)
{
	bool blockedPath = false;

	switch (dir)
	{
	case GraphObject::left:
		for (int j = y; j <= y + 3; ++j)
		{
			if (isIceAt(x, j))
			{
				blockedPath = true;
			}
		}
		break;
	case GraphObject::right:
		for (int j = y; j <= y + 3; ++j)
		{
			if (isIceAt(x + 3, j))
			{
				blockedPath = true;
			}
		}
		break;
	case GraphObject::up:
		for (int i = x; i <= x + 3; ++i)
		{
			if (isIceAt(i, y + 3))
			{
				blockedPath = true;
			}
		}
		break;
	case GraphObject::down:
		for (int i = x; i <= x + 3; ++i)
		{
			if (isIceAt(i, y))
			{
				blockedPath = true;
			}
		}
		break;
	}

	return blockedPath;
}

// Removes 4 squares of Ice in front of Iceman, depending on his direction. Returns true if at least one square of Ice is removed
bool StudentWorld::removeIceInFront(int x, int y)
{
	bool removed = false;

	if (isIceInFront(m_iceman->getDirection(), x, y))
	{
		removed = true;

		GraphObject::Direction dir = m_iceman->getDirection();
		switch (dir)
		{
		case GraphObject::left:
			for (int j = y; j <= y + 3; ++j)
			{
				removeIce(x, j);
			}
			break;
		case GraphObject::right:
			for (int j = y; j <= y + 3; ++j)
			{
				removeIce(x + 3, j);
			}
			break;
		case GraphObject::up:
			for (int i = x; i <= x + 3; ++i)
			{
				removeIce(i, y + 3);
			}
			break;
		case GraphObject::down:
			for (int i = x; i <= x + 3; ++i)
			{
				removeIce(i, y);
			}
			break;
		}
	}
	
	return removed;
}

// Remove a single square of ice
void StudentWorld::removeIce(int x, int y)
{
	if (isIceAt(x, y))
	{
		Ice* p = m_iceField[x][y];
		m_iceField[x][y] = nullptr;
		delete p;
	}
}

bool StudentWorld::willCollideWithBoulder(int x, int y) // x and y are moveTo coordinates of Person
{
	// parameters cant be implicitly captured...
	auto collidingBoulder = find_if(m_gameObjects.begin(), m_gameObjects.end(), [this, &x, &y](auto obj)
	{
		return (obj->isSolid() && withinDistanceActor(3.00, x, y, obj));
	});

	return (collidingBoulder != m_gameObjects.end()); // return whether there is a boulder the actor can collide with

}

// auxilary function for searchNearbyPersons. Checks if object is a pick up or an annoyer and either adds gold or annoys person within radius
bool StudentWorld::annoyActorsOrFindPickerUpper(Actor* object, int points, double radius, Actor* person)
{
	bool pickedUpOrAnnoyed = false;

	if (person->canPickUpAndBeAnnoyed())
	{
		if (withinDistanceActor(radius, object->getX(), object->getY(), person))
		{
			if (object->isPickupable() && !pickedUpOrAnnoyed) // object can only be picked up once
			{
				person->addGold();
				pickedUpOrAnnoyed = true;
			}
			else if (!object->isPickupable())
			{
				person->annoy(points);
				pickedUpOrAnnoyed = true;
			}
		}
	}

	return pickedUpOrAnnoyed;
}

// search for persons to annoy or pick up object depending on what class the object is
bool StudentWorld::searchNearbyPersons(Actor * object, int points, double radius)
{
	bool success = false;
	
	for_each(m_gameObjects.begin(), m_gameObjects.end(), [this, &object, &points, &radius, &success](Actor* person)
	{
		if (annoyActorsOrFindPickerUpper(object, points, radius, person))
			success = true;
	});

	return success;
}

bool StudentWorld::facingTowardIceMan(Actor* a) const
{
	bool isFacingIceman = false;

	GraphObject::Direction dir = a->getDirection();
	int ax = a->getX(); int ay = a->getY();
	int ix = m_iceman->getX(); int iy = m_iceman->getY();

	if (dir == GraphObject::left || dir == GraphObject::right)
	{
		if (ay >= iy - SPRITE_HEIGHT && ay <= iy + SPRITE_HEIGHT) // check if y coordinates are within 12 squares of iceman
		{
			// check if x coordinates are within 4 squares of iceman, depending on direction
			if (dir == GraphObject::left)
				isFacingIceman = (ax >= ix && ax <= ix + SPRITE_WIDTH);
			if (dir == GraphObject::right)
				isFacingIceman = (ax >= ix - SPRITE_WIDTH && ax <= ix);
		}
	}
	else if (dir == GraphObject::up || dir == GraphObject::down)
	{
		if (ax >= ix - SPRITE_WIDTH && ax <= ix + SPRITE_WIDTH)  // check if x coordinates are within 12 squares of iceman
		{
			// check if y coordinates are within 4 squares of iceman, depending on direction
			if (dir == GraphObject::up)
				isFacingIceman = (ay >= iy && ay <= iy + SPRITE_HEIGHT);
			if (dir == GraphObject::down)
				isFacingIceman = (ay >= iy - SPRITE_HEIGHT && ay <= iy);
		}
	}

	return isFacingIceman;
}

void StudentWorld::annoyIceMan(int points)
{
	m_iceman->decHP(points);
}

int StudentWorld::init()
{
	
	// generates random numbers based on seed (the score). Needs static keyword or same numbers will be generated
	static default_random_engine engine(getScore()); 
	static uniform_int_distribution<int> distributorX(0, VIEW_WIDTH-SPRITE_WIDTH); // Needed for even distrubution of generating random numbers 0 through 60
	static uniform_int_distribution<int> distributorY(0, VIEW_HEIGHT-2* SPRITE_HEIGHT); // Ditsribute random y coordinates (0 through 56)

	// initialize ice field
	for (int i = 0; i < VIEW_WIDTH; ++i)
	{
		for (int j = 0; j < VIEW_HEIGHT-SPRITE_HEIGHT; ++j)
		{
			if (isTunnel(i, j))
				m_iceField[i][j] = nullptr;
			else
				m_iceField[i][j] = new Ice(this, i, j);
		}
	}

	// sets up maze (to help protesters search for exit)
	for (int i = 0; i < VIEW_WIDTH - SPRITE_WIDTH; ++i)
	{
		for (int j = 0; j < VIEW_HEIGHT - SPRITE_HEIGHT; ++j)
		{
			bool isExit = false;
			if (i == 60 && j == 60)
				isExit = true;
			m_maze[i][j] = new MazeTracker{ i, j, false, false, isExit };
		}
	}

	// generate boulders
	int B = min(static_cast<int>(getLevel()) / 2 + 2, 9);
	for (int n = 0; n < B; ++n)
	{
		int x, y;
		
		// randomly generate boulder coordinates while they are not in tunnel, overlapping each other, or placed too low
		do
		{
			x = distributorX(engine);
			y = distributorY(engine);
		} while (isOverlappingObjects(x, y) || y < 20 );

		// clear 4x4 grid
		for (int i = x; i < x + 4; ++i)
		{
			for (int j = y; j < y + 4; ++j)
				removeIce(i, j);
		}

		m_gameObjects.push_back(new Boulder(this, x, y));
	}

	// generate oil barrels L
	int L = min(2 + static_cast<int>(getLevel()), 21);
	m_barrels = L;

	for (int i = 0; i < L; ++i)
	{
		int x, y;

		do
		{
			x = distributorX(engine);
			y = distributorY(engine);
		} while(isOverlappingObjects(x, y));

		m_gameObjects.push_back(new Barrel(this, x, y));
	}
	
	// GENERATE GOLD
	int G = max(5 - static_cast<int>(getLevel()) / 2, 2);
	for (int i = 0; i < G; ++i)
	{
		int x, y;

		do
		{
			x = distributorX(engine);
			y = distributorY(engine);
		} while (isOverlappingObjects(x, y));

		m_gameObjects.push_back(new GoldNugget(Goodie::permanent, this, x, y));
	}

	// prepare to add protesters
	m_ticksSinceLastProtesterAdded = max(25, 200 - static_cast<int>(getLevel()));
	m_targetNumProtesters = min(15, 2 + static_cast<int>(getLevel() * 1.5));
	m_protesters = 0;

	// Allocate and insert a valid Iceman object into the game world at the proper location
	m_iceman = new Iceman(this);

	return GWSTATUS_CONTINUE_GAME;
}

int StudentWorld::move()
{
	// Update the Game Status Line
	setDisplayText(); // update the score/lives/level text at screen top

	// Give each Actor a chance to do something
	allActorsDoSomething();

	// Chance to add new actors
	static default_random_engine engine(getScore());

	// add protesters
	int T = max(25, 200 - static_cast<int>(getLevel()));
	if (m_ticksSinceLastProtesterAdded >= T && m_protesters < m_targetNumProtesters)
	{
		int probabilityOfHardcore = min(90, static_cast<int>(getLevel()) * 10 + 30);
		static uniform_int_distribution<int> distributorHardcore(0, 90);
		
		int generatedNum = distributorHardcore(engine);

		if (generatedNum <= probabilityOfHardcore)
			m_gameObjects.push_back(new HardcoreProtester(this));
		else
			m_gameObjects.push_back(new RegularProtester(this));

		++m_protesters;
		m_ticksSinceLastProtesterAdded = 0;
	}

	// add goodies
	int G = static_cast<int>(getLevel()) * 25 + 300;
	
	static uniform_int_distribution<int> distributorGoodie(1, G);

	int generatedNum = distributorGoodie(engine);
	if (generatedNum == 1)
	{
		static uniform_int_distribution<int> distributorGoodieType(1, 5);
		int type = distributorGoodieType(engine);

		if (type == 1)
		{
			m_gameObjects.push_back(new Sonar(this));
		}
		else // generate water pool
		{
			static uniform_int_distribution<int> distributorX(0, VIEW_WIDTH - SPRITE_WIDTH);
			static uniform_int_distribution<int> distributorY(0, VIEW_HEIGHT - 2 * SPRITE_HEIGHT);

			int waterX;
			int waterY;

			do
			{
				waterX = distributorX(engine);
				waterY = distributorY(engine);
			} while (!isIcelessSpot(waterX, waterY));

			m_gameObjects.push_back(new Water(this, waterX, waterY));
		}
	}

	// Remove newly-dead actors after each tick
	removeDeadGameObjects();

	if (m_iceman->isDead())
	{
		playSound(SOUND_PLAYER_GIVE_UP);
		decLives();
		return GWSTATUS_PLAYER_DIED;
	}

	// If the player has collected all of the Barrels on the level, then
	// return the result that the player finished the level
	if (getBarrelsRemaining() == 0)
	{
		playSound(SOUND_FINISHED_LEVEL);
		return GWSTATUS_FINISHED_LEVEL;
	}

	++m_ticksSinceLastProtesterAdded;
	return GWSTATUS_CONTINUE_GAME;
}

void StudentWorld::cleanUp()
{
	freeDynamicallyAllocatedData();
}



