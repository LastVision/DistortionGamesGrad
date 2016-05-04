#include "stdafx.h"

#include <ContactNote.h>
#include <EntityFactory.h>
#include <InputComponent.h>
#include "Level.h"
#include <PhysicsComponent.h>
#include <PhysicsInterface.h>
#include <Scene.h>
#include <TriggerComponent.h>

Level::Level(Prism::Camera& aCamera)
	: myCamera(aCamera)
	, myEntities(1024)
{
	Prism::PhysicsInterface::Create(std::bind(&Level::CollisionCallback, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3)
		, std::bind(&Level::ContactCallback, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4));

	myScene = new Prism::Scene();
	myScene->SetCamera(aCamera);
	myPlayers.Init(2);
	int playerCount = 2;

	for (int i = 0; i < playerCount; ++i)
	{
		Entity* player = EntityFactory::CreateEntity(eEntityType::PLAYER, "player", myScene, CU::Vector3<float>());
		player->GetComponent<InputComponent>()->AddController(-1);
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

	TriggerComponent* firstTrigger = first.GetComponent<TriggerComponent>();

	if (aHasEntered == true && firstTrigger != nullptr && second.GetType() == eEntityType::PLAYER)
	{
		switch (firstTrigger->GetTriggerType())
		{
		case eTriggerType::HAZARD:
			// kill player
			break;
		case eTriggerType::FORCE:
			// push player
			break;
		}
	}
}

void Level::ContactCallback(PhysicsComponent* aFirst, PhysicsComponent* aSecond, CU::Vector3<float> aContactPoint, CU::Vector3<float> aContactNormal)
{
	Entity* first = &aFirst->GetEntity();
	Entity* second = &aSecond->GetEntity();
	bool aHasEntered = true;
	if (first->GetType() == eEntityType::PLAYER)
	{
		first->SendNote<ContactNote>(ContactNote(second, aContactPoint, aContactNormal, aHasEntered));
	}

}