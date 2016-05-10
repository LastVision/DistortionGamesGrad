#pragma once

#include "GameState.h"
#include <GrowingArray.h>
#include "Subscriber.h"

class Entity;

namespace Prism
{
	class Camera;
	class Scene;
	class SpriteProxy;
};

class PhysicsComponent;
class SmartCamera;

class Level : public Subscriber, public GameState
{
	friend class LevelFactory;
public:
	Level(Prism::Camera& aCamera);
	~Level();

	void InitState(StateStackProxy* aStateStackProxy, GUI::Cursor* aCursor) override;

	const eStateStatus Update(const float& aDeltaTime) override;
	void Render() override;

	void CollisionCallback(PhysicsComponent* aFirst, PhysicsComponent* aSecond, bool aHasEntered);
	void ContactCallback(PhysicsComponent* aFirst, PhysicsComponent* aSecond, CU::Vector3<float> aContactPoint
		, CU::Vector3<float> aContactNormal, bool aHasEntered);

	void CreatePlayers();

	void EndState() override;
	void ResumeState() override;
	void OnResize(int aWidth, int aHeight) override;

private:
	void operator=(Level&) = delete;

	Prism::SpriteProxy* myBackground;

	Prism::Camera& myCamera;
	Prism::Scene* myScene;
	SmartCamera* mySmartCamera;
	CU::Matrix44<float> dummyMatrix;

	CU::GrowingArray<Entity*> myPlayers;

	CU::GrowingArray<Entity*> myEntities;

	CU::Vector3<float> myStartPosition;
	CU::Vector2<float> myWindowSize;
};

