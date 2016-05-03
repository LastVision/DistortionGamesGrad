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
	void ContactCallback(PhysicsComponent* aFirst, PhysicsComponent* aSecond, CU::Vector3<float> aContactPoint);

private:
	void operator=(Level&) = delete;



	Prism::Camera& myCamera;
	Prism::Scene* myScene;

	CU::GrowingArray<Entity*> myPlayers;

	CU::GrowingArray<Entity*> myEntities;
};

