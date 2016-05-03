#pragma once
#include <GrowingArray.h>
#include "Subscriber.h"

class Entity;

namespace Prism
{
	class Camera;
	class Scene;
};

class PhysicsComponent;

class Level : public Subscriber
{
	friend class LevelFactory;
public:
	Level(Prism::Camera& aCamera);
	~Level();

	void Update(float aDelta);
	void Render();

	void CollisionCallback(PhysicsComponent* aFirst, PhysicsComponent* aSecond, bool aHasEntered);
	void ContactCallback(PhysicsComponent* aFirst, PhysicsComponent* aSecond);

private:
	void operator=(Level&) = delete;



	Prism::Camera& myCamera;
	Prism::Scene* myScene;

	Entity* myPlayer;

	CU::GrowingArray<Entity*> myEntities;
};

