#ifndef STUDENTWORLD_H_
#define STUDENTWORLD_H_

#include "GameWorld.h"
#include "GameConstants.h"
#include "Actor.h"
#include <string>
#include <vector>
#include <array>
#include <random>

class GraphObject;
class Actor;
class Ice;
class Iceman;
class Squirt;
class GoldNugget;
struct MazeTracker;

class StudentWorld : public GameWorld
{
public:
	StudentWorld(std::string assetDir);
	virtual ~StudentWorld();

	// manages game objects
	void freeDynamicallyAllocatedData();
	void removeDeadGameObjects();
	void allActorsDoSomething();
	std::string formatText(int le, int li, int h, int sq, int g, int b, int so, int sc);
	void setDisplayText();
	void decBarrels();
	int getBarrelsRemaining();
	bool isOverlappingObjects(int x, int y);
	void addSquirts();
	void addGold();
	void addSonar();
	void placeActorOnField(Actor* a);

	// manages actors interacting with each other
	bool isPickedUpByIceman(int x, int y);
	bool withinDistanceIceman(double radius, int h, int k);
	bool StudentWorld::withinDistanceActor(double radius, int h, int k, Actor* a); 
																				
	void useSonar();
	bool willCollideWithBoulder(int x, int y);
	bool annoyActorsOrFindPickerUpper(Actor* object, int points, double radius, Actor* person);
	bool searchNearbyPersons(Actor* object, int points, double radius);
	bool facingTowardIceMan(Actor* a) const;
	void annoyIceMan(int points);
	GraphObject::Direction lineOfSightToIceMan(Actor* a);
	void protesterLeaves();
	//MazeTracker* checkMaze(MazeTracker* mt);
	void findExit();

	// ice management functions
	bool isTunnel(int x, int y);
	bool isIceAt(int x, int y); // checks if a single square has ice
	bool isIcelessSpot(int x, int y); // checks if a 4x4 grid of is free of ice
	bool isIceInFront(GraphObject::Direction dir, int x, int y);
	bool removeIceInFront(int x, int y);
	void removeIce(int x, int y);

	virtual int init();
	virtual int move();
	virtual void cleanUp();

private:
	
	std::array<std::array<Ice*, VIEW_HEIGHT-SPRITE_HEIGHT>, VIEW_WIDTH> m_iceField;
	std::vector<Actor*> m_gameObjects;
	Iceman* m_iceman;
	int m_barrels;
	int m_protesters;
	int m_targetNumProtesters;
	int m_ticksSinceLastProtesterAdded;
	
	struct MazeTracker
	{
		int x;
		int y;
		bool hasVisited;
		bool hasLeavingProtester;
		bool pathToExit;
	};

	std::array<std::array<MazeTracker*, VIEW_HEIGHT - SPRITE_HEIGHT>, VIEW_WIDTH - SPRITE_WIDTH> m_maze;
};

#endif // STUDENTWORLD_H_
