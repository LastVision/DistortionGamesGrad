#include "stdafx.h"

#include <ContactNote.h>
#include <ControllerInput.h>
#include <EntityFactory.h>
#include <FinishLevelMessage.h>
#include <InputComponent.h>
#include "Level.h"
#include <MovementComponent.h>
#include <ModelLoader.h>
#include <OnDeathMessage.h>
#include <PhysicsComponent.h>
#include <PhysicsInterface.h>
#include <PlayerActiveMessage.h>
#include <PostMaster.h>
#include <Scene.h>
#include "ScoreState.h"
#include "ScrapManager.h"
#include <ScrapMessage.h>
#include "SmartCamera.h"
#include <SpriteProxy.h>
#include <TriggerComponent.h>
#include <OnPlayerLevelComplete.h>

Level::Level(Prism::Camera& aCamera)
	: myCamera(aCamera)
	, myEntities(1024)
	, myPlayers(2)
	, mySmartCamera(new SmartCamera(myCamera))
	, myPlayerWinCount(0)
	, myTimeToLevelChange(10.f)
	, myBackground(nullptr)
{
	Prism::PhysicsInterface::Create(std::bind(&Level::CollisionCallback, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3)
		, std::bind(&Level::ContactCallback, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3
		, std::placeholders::_4, std::placeholders::_5));

	myScene = new Prism::Scene();
	myScene->SetCamera(aCamera);
	mySmartCamera->SetStartPosition(myStartPosition);
	myWindowSize = Prism::Engine::GetInstance()->GetWindowSize();
	myBackground = Prism::ModelLoader::GetInstance()->LoadSprite("Data/Resource/Texture/T_background.dds", myWindowSize, myWindowSize * 0.5f);

	ScrapManager::Create(myScene);
}

Level::~Level()
{
	ScrapManager::Destroy();
	SAFE_DELETE(myBackground);
	SAFE_DELETE(mySmartCamera);
	SAFE_DELETE(myScene);
	myEntities.DeleteAll();
	myPlayers.DeleteAll();


#ifdef THREAD_PHYSICS
	Prism::PhysicsInterface::GetInstance()->ShutdownThread();
#endif
	Prism::PhysicsInterface::Destroy();
}

void Level::InitState(StateStackProxy* aStateStackProxy, GUI::Cursor* aCursor)
{
	myCursor = aCursor;
	myStateStack = aStateStackProxy;
	myIsLetThrough = false;
	myIsActiveState = true;
	myStateStatus = eStateStatus::eKeepState;
}

const eStateStatus Level::Update(const float& aDeltaTime)
{
#ifndef THREAD_PHYSICS
	Prism::PhysicsInterface::GetInstance()->FrameUpdate();
#endif
	mySmartCamera->Update(aDeltaTime);
	ScrapManager::GetInstance()->Update(aDeltaTime);

	for each(Entity* player in myPlayers)
	{
		player->Update(aDeltaTime);
	}

	for each(Entity* entity in myEntities)
	{
		entity->Update(aDeltaTime);
	}

	return myStateStatus;
}

void Level::Render()
{
	myBackground->Render(myWindowSize * 0.5f);
	myScene->Render();
}

void Level::CollisionCallback(PhysicsComponent* aFirst, PhysicsComponent* aSecond, bool aHasEntered)
{
	Entity& first = aFirst->GetEntity();
	Entity& second = aSecond->GetEntity();

	TriggerComponent* firstTrigger = first.GetComponent<TriggerComponent>();

	if (firstTrigger != nullptr && second.GetType() == eEntityType::PLAYER)
	{
		if (aHasEntered == true)
		{
			switch (firstTrigger->GetTriggerType())
			{
			case eTriggerType::HAZARD:
				// kill player
				break;
			case eTriggerType::FORCE:

				CU::Vector3<float> velocity = { second.GetComponent<MovementComponent>()->GetVelocity().x, second.GetComponent<MovementComponent>()->GetVelocity().y, 0.f };

				if (abs(CU::Dot(velocity, first.GetOrientation().GetUp()) < 0.85f))
				{
					second.GetComponent<MovementComponent>()->SetVelocity(second.GetComponent<MovementComponent>()->GetVelocity() * 0.5f);
				}

				second.GetComponent<MovementComponent>()->SetInSteam(true
					, { first.GetOrientation().GetUp().x * 0.5f, first.GetOrientation().GetUp().y * 0.5f });
				break;
			}
		}
		else if (firstTrigger->GetTriggerType() == eTriggerType::FORCE)
		{
			second.GetComponent<MovementComponent>()->SetInSteam(false);
		}
	}
}

void Level::ContactCallback(PhysicsComponent* aFirst, PhysicsComponent* aSecond, CU::Vector3<float> aContactPoint
	, CU::Vector3<float> aContactNormal, bool aHasEntered)
{
	Entity* first = &aFirst->GetEntity();
	Entity* second = &aSecond->GetEntity();
	if (first->GetType() == eEntityType::PLAYER)
	{
		first->SendNote<ContactNote>(ContactNote(second, aContactPoint, aContactNormal, aHasEntered));

		switch (second->GetType())
		{
		case eEntityType::SAW_BLADE:
		case eEntityType::SPIKE:
			//first->Reset();
			if (aHasEntered == true)
			{
				//ScrapManager::GetInstance()->SpawnScrap(eScrapPart::HEAD, first->GetOrientation().GetPos()
				//	, first->GetComponent<MovementComponent>()->GetVelocity());

				PostMaster::GetInstance()->SendMessage<ScrapMessage>(ScrapMessage(eScrapPart::HEAD
					, first->GetOrientation().GetPos(), first->GetComponent<MovementComponent>()->GetVelocity()));

				PostMaster::GetInstance()->SendMessage(OnDeathMessage(first->GetComponent<InputComponent>()->GetPlayerID()));
				first->SetPosition(myStartPosition);
				aFirst->TeleportToPosition(myStartPosition);
			}
			break;
		case eEntityType::BOUNCER:
			if (aHasEntered == true)
			{
				first->GetComponent<MovementComponent>()->SetVelocity({ second->GetOrientation().GetUp().x * 0.1f
					, second->GetOrientation().GetUp().y * 0.1f });
			}
			break;
		case eEntityType::GOAL_POINT:
			if (aHasEntered == true)
			{

				TriggerComponent* firstTrigger = second->GetComponent<TriggerComponent>();
				DL_ASSERT_EXP(firstTrigger != nullptr, "Goal point has to have a trigger component");
				PostMaster::GetInstance()->SendMessage(OnPlayerLevelComplete(first->GetComponent<InputComponent>()->GetPlayerID()));
				myPlayerWinCount++;
				if (myPlayerWinCount > 1)
				{
					PostMaster::GetInstance()->SendMessage(FinishLevelMessage(firstTrigger->GetLevelID()));

					SET_RUNTIME(false);
					myStateStack->PushSubGameState(new ScoreState());
				}

			}
			break;
		}
	}
}

void Level::CreatePlayers()
{
	Entity* player = EntityFactory::CreateEntity(eEntityType::PLAYER, "player", myScene, myStartPosition);
	player->GetComponent<InputComponent>()->AddController(eControllerID::Controller1);
	player->GetComponent<InputComponent>()->SetPlayerID(1);

	player->AddToScene();
	myPlayers.Add(player);
	mySmartCamera->AddOrientation(&player->GetOrientation());
	//mySmartCamera->AddOrientation(&dummyMatrix);

	player = EntityFactory::CreateEntity(eEntityType::PLAYER, "player", myScene, myStartPosition);
	player->GetComponent<InputComponent>()->AddController(eControllerID::Controller2);
	player->GetComponent<InputComponent>()->SetPlayerID(2);

	player->AddToScene();
	myPlayers.Add(player);
	mySmartCamera->AddOrientation(&player->GetOrientation());

	mySmartCamera->SetActivePlayerCount(0);
}

void Level::EndState()
{

}

void Level::ResumeState()
{

}

void Level::OnResize(int aWidth, int aHeight)
{
	aWidth;
	aHeight;
}
