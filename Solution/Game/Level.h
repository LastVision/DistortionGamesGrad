#pragma once

#include "GameState.h"
#include <GrowingArray.h>
#include "Subscriber.h"
#include <Quaternion.h>
class Entity;

namespace Prism
{
	class Camera;
	class DeferredRenderer;
	class DirectionalLight;
	class Renderer;
	class Scene;
	class SpriteProxy;
	class SpotLightShadow;
	class SpotLight;
	class EmitterManager;
	class PointLight;
	class Texture;
};

class PhysicsComponent;
class ScrapManager;
class SmartCamera;

struct Score;
struct ScoreInfo;

class Level : public Subscriber, public GameState
{
public:
	Level(Prism::Camera& aCamera, const int aLevelID);
	~Level();

	void InitState(StateStackProxy* aStateStackProxy, CU::ControllerInput* aController, GUI::Cursor* aCursor) override;

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
	void ReceiveMessage(const OnDeathMessage& aMessage) override;

	Prism::Scene* GetScene() const;
	void Add(Entity* anEntity);
	void CreateScoreInfo(float aShortTime, float aMediumTime, float aLongTime);
	void Add(Prism::PointLight* aLight);
	void Add(Prism::DirectionalLight* aLight);
	void Add(Prism::SpotLight* aLight);
	void SetSpawnPosition(const CU::Vector3<float>& aSpawnPosition);
	void SetSpawnVelocity(const CU::Vector2<float>& aSpawnVelocity);

private:

	void KillPlayer(Entity* aPlayer, const CU::Vector2<float>& aGibsVelocity = { 0.f, 0.f });

	void operator=(Level&) = delete;

	Prism::Texture* myBackground;

	Prism::Camera& myCamera;
	Prism::Scene* myScene;
	Prism::DeferredRenderer* myDeferredRenderer;
	Prism::Renderer* myFullscreenRenderer;
	Prism::SpotLightShadow* myShadowLight;
	CU::GrowingArray<Prism::PointLight*> myPointLights;
	CU::GrowingArray<Prism::DirectionalLight*> myDirectionalLights;
	CU::GrowingArray<Prism::SpotLight*> mySpotLights;
	SmartCamera* mySmartCamera;
	CU::Matrix44<float> dummyMatrix;

	CU::GrowingArray<Entity*> myPlayers;
	CU::GrowingArray<Entity*> myEntities;
	CU::GrowingArray<Prism::PointLight*> myPlayerPointLights;

	CU::GrowingArray<ScrapManager*> myScrapManagers;
	CU::GrowingArray<const Score*> myScores;
	ScoreInfo* myScoreInfo;

	Prism::EmitterManager* myEmitterManager;

	Prism::SpriteProxy* myCountdownSprites[10];
	int myCurrentCountdownSprite;

	CU::Vector2<float> mySpawnVelocity;
	CU::Vector3<float> mySpawnPosition;
	CU::Vector2<float> myWindowSize;
	float myTimeToLevelChange;
	short myPlayerWinCount;
	short myPlayersPlaying;
	int myLevelToChangeToID;
	const int myLevelID;

	bool myIsFreeCam = false;

	CU::Matrix44f myOrientation;
	CU::Quaternion myPitch;
	CU::Quaternion myYaw;
	CU::Vector2<float> myCursorPosition;
	void UpdateInput(float aDeltaTime);


};

inline Prism::Scene* Level::GetScene() const
{
	return myScene;
}

inline void Level::SetSpawnPosition(const CU::Vector3<float>& aSpawnPosition)
{
	mySpawnPosition = aSpawnPosition;
}

inline void Level::SetSpawnVelocity(const CU::Vector2<float>& aSpawnVelocity)
{
	mySpawnVelocity = aSpawnVelocity;
}