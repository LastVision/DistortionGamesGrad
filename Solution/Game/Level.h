#pragma once

#include "GameState.h"
#include <GrowingArray.h>
#include "Subscriber.h"

class Entity;

namespace Prism
{
	class Camera;
	class DeferredRenderer;
	class Renderer;
	class Scene;
	class SpriteProxy;
	class SpotLightShadow;
	class EmitterManager;
};

class PhysicsComponent;
class ScrapManager;
class SmartCamera;

struct Score;
struct ScoreInfo;

class Level : public Subscriber, public GameState
{
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
	void PauseState() override;
	void OnResize(int aWidth, int aHeight) override;

	void ReceiveMessage(const OnPlayerJoin& aMessage) override;

	Prism::Scene* GetScene() const;
	void Add(Entity* anEntity);
	void SetSpawnPosition(const CU::Vector3<float>& aSpawnPosition);

private:
	void operator=(Level&) = delete;

	Prism::SpriteProxy* myBackground;

	Prism::Camera& myCamera;
	Prism::Scene* myScene;
	Prism::DeferredRenderer* myDeferredRenderer;
	Prism::Renderer* myFullscreenRenderer;
	Prism::SpotLightShadow* myShadowLight;
	SmartCamera* mySmartCamera;
	CU::Matrix44<float> dummyMatrix;

	CU::GrowingArray<Entity*> myPlayers;

	CU::GrowingArray<Entity*> myEntities;

	CU::GrowingArray<const Score*> myScores;
	ScoreInfo* myScoreInfo;

	Prism::EmitterManager* myEmitterManager;


	CU::Vector3<float> mySpawnPosition;
	CU::Vector2<float> myWindowSize;
	float myTimeToLevelChange;
	short myPlayerWinCount;
	short myPlayersPlaying;
	int myLevelToChangeToID;
};

inline Prism::Scene* Level::GetScene() const
{
	return myScene;
}

inline void Level::SetSpawnPosition(const CU::Vector3<float>& aSpawnPosition)
{
	mySpawnPosition = aSpawnPosition;
}