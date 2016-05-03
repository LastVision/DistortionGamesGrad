#include "stdafx.h"

#include "Level.h"
#include <Scene.h>
#include <EntityFactory.h>
#include <PhysicsComponent.h>
#include <PhysicsInterface.h>
#include <InputComponent.h>

Level::Level(Prism::Camera& aCamera)
	: myCamera(aCamera)
	, myEntities(1024)
{
	Prism::PhysicsInterface::Create(std::bind(&Level::CollisionCallback, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3)
		, std::bind(&Level::ContactCallback, this, std::placeholders::_1, std::placeholders::_2));

	myScene = new Prism::Scene();
	myScene->SetCamera(aCamera);
	myPlayers.Init(2);
	int playerCount = 2;

	for (int i = 0; i < playerCount; ++i)
	{
		Entity* player = EntityFactory::CreateEntity(eEntityType::PLAYER, "player", myScene, CU::Vector3<float>());
		player->GetComponent<InputComponent>()->AddController(myPlayers.Size());
		player->AddToScene();
		myPlayers.Add(player);
	}
}

Level::~Level()
{
	SAFE_DELETE(myScene);
	myEntities.DeleteAll();
	myPlayers.DeleteAll();

#ifdef THREAD_PHYSICS
	Prism::PhysicsInterface::GetInstance()->ShutdownThread();
#endif
	Prism::PhysicsInterface::Destroy();
}

void Level::Update(float aDelta)
{
	for each(Entity* player in myPlayers)
	{
		player->Update(aDelta);
	}

	for each(Entity* entity in myEntities)
	{
		entity->Update(aDelta);
	}
}

void Level::Render()
{
	myScene->Render();
}

void Level::CollisionCallback(PhysicsComponent* aFirst, PhysicsComponent* aSecond, bool aHasEntered)
{
	Entity& first = aFirst->GetEntity();
	Entity& second = aSecond->GetEntity();

	if (aHasEntered == true && second.GetType() == eEntityType::PLAYER)
	{
		switch (first.GetType())
		{
		case eEntityType::SAW_BLADE:
		case eEntityType::SPIKE:
			// kill player
			break;
		}
	}
}

void Level::ContactCallback(PhysicsComponent* aFirst, PhysicsComponent* aSecond)
{
	//Entity& first = aFirst->GetEntity();
	//Entity& second = aSecond->GetEntity();

	//if (aFirst->GetPhysicsType() == ePhysics::DYNAMIC)
	//{
	//	if (first.GetComponent<SoundComponent>() != nullptr)
	//	{
	//		Prism::Audio::AudioInterface::GetInstance()->PostEvent("Play_PropBounce", first.GetComponent<SoundComponent>()->GetAudioSFXID());
	//	}
	//}
	//else if (aSecond->GetPhysicsType() == ePhysics::DYNAMIC)
	//{
	//	if (second.GetComponent<SoundComponent>() != nullptr)
	//	{
	//		Prism::Audio::AudioInterface::GetInstance()->PostEvent("Play_PropBounce", second.GetComponent<SoundComponent>()->GetAudioSFXID());
	//	}
	//}
	//else
	//{
	//	int whatIsCollidingHere = 15;
	//	whatIsCollidingHere;
	//}
}