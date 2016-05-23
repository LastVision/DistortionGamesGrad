#include "stdafx.h"

#include <AudioInterface.h>
#include <BounceComponent.h>
#include <BounceNote.h>
#include <Camera.h>
#include <ContactNote.h>
#include <ControllerInput.h>
#include "EmitterManager.h"
#include <DeferredRenderer.h>
#include <EntityFactory.h>
#include <FinishLevelMessage.h>
#include <InputComponent.h>
#include <InputWrapper.h>
#include "Level.h"
#include <MovementComponent.h>
#include <ModelLoader.h>
#include <PhysicsComponent.h>
#include <PhysicsInterface.h>
#include <PlayerActiveMessage.h>
#include <PlayerComponent.h>
#include <PollingStation.h>
#include <PostMaster.h>
#include <Renderer.h>
#include <ReturnToMenuMessage.h>
#include <Scene.h>
#include <ScoreComponent.h>
#include "ScoreInfo.h"
#include "ScoreState.h"
#include "ScrapManager.h"
#include <ScrapMessage.h>
#include <ShouldDieNote.h>
#include "SmartCamera.h"
#include <SpotLightShadow.h>
#include <SpriteProxy.h>
#include <PlayerGraphicsComponent.h>
#include <TriggerComponent.h>
#include <OnPlayerLevelComplete.h>
#include <OnDeathMessage.h>


#include <PointLight.h>

Level::Level(Prism::Camera& aCamera, const int aLevelID)
	: myCamera(aCamera)
	, myEntities(1024)
	, myPlayers(2)
	, mySmartCamera(new SmartCamera(myCamera))
	, myPlayerWinCount(0)
	, myTimeToLevelChange(10.f)
	, myBackground(nullptr)
	, myPlayersPlaying(0)
	, myScores(4)
	, myCurrentCountdownSprite(9)
	, myLevelID(aLevelID)
	, myPointLights(32)
	, myPlayerPointLights(4)
{
	Prism::PhysicsInterface::Create(std::bind(&Level::CollisionCallback, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3)
		, std::bind(&Level::ContactCallback, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3
		, std::placeholders::_4, std::placeholders::_5));

	myScene = new Prism::Scene();
	myScene->SetCamera(aCamera);
	myWindowSize = Prism::Engine::GetInstance()->GetWindowSize();
	myBackground = Prism::ModelLoader::GetInstance()->LoadSprite("Data/Resource/Texture/T_background.dds", myWindowSize, myWindowSize * 0.5f);
	PostMaster::GetInstance()->Subscribe(this, eMessageType::ON_PLAYER_JOIN | eMessageType::ON_DEATH);
	ScrapManager::Create(myScene);
	myEmitterManager = new Prism::EmitterManager();
	myEmitterManager->Initiate(&myCamera);
	Prism::ModelLoader::GetInstance()->Pause();
	myDeferredRenderer = new Prism::DeferredRenderer();
	myFullscreenRenderer = new Prism::Renderer();
	myShadowLight = new Prism::SpotLightShadow(aCamera.GetOrientation());
	Prism::ModelLoader::GetInstance()->UnPause();

	Prism::Audio::AudioInterface::GetInstance()->PostEvent("Play_InGameMusic", 0);

	CU::Vector2<float> size(256.f, 256.f);
	std::string texturePath("Data/Resource/Texture/Countdown/T_countdown_");
	for (int i = 0; i < 10; ++i)
	{
		myCountdownSprites[i] = Prism::ModelLoader::GetInstance()->LoadSprite(texturePath + std::to_string(i + 1) + ".dds", size, size * 0.5f);
	}

	PollingStation::Create();
}

Level::~Level()
{
	for (int i = 0; i < 10; ++i)
	{
		SAFE_DELETE(myCountdownSprites[i]);
	}
	Prism::Audio::AudioInterface::GetInstance()->PostEvent("Stop_InGameMusic", 0);
	SAFE_DELETE(myEmitterManager);
	ScrapManager::Destroy();
	SAFE_DELETE(myShadowLight);
	SAFE_DELETE(myBackground);
	SAFE_DELETE(mySmartCamera);
	SAFE_DELETE(myScene);
	SAFE_DELETE(myDeferredRenderer);
	SAFE_DELETE(myFullscreenRenderer);
	SAFE_DELETE(myScoreInfo);
	myEntities.DeleteAll();
	myPlayers.DeleteAll();
	myPointLights.DeleteAll();
	myPlayerPointLights.DeleteAll();
	PostMaster::GetInstance()->UnSubscribe(this, 0);

	PollingStation::Destroy();

#ifdef THREAD_PHYSICS
	Prism::PhysicsInterface::GetInstance()->ShutdownThread();
#endif
	Prism::PhysicsInterface::Destroy();
}

void Level::InitState(StateStackProxy* aStateStackProxy, CU::ControllerInput* aController, GUI::Cursor* aCursor)
{
	myStateStack = aStateStackProxy;
	myController = aController;
	myCursor = aCursor;
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
	
	CU::Vector3<float> cameraPos(mySmartCamera->GetOrientation().GetPos());
	CU::Vector3<float> cameraForward(mySmartCamera->GetOrientation().GetForward());
	CU::Vector3<float> cameraUp(mySmartCamera->GetOrientation().GetUp());
	Prism::Audio::AudioInterface::GetInstance()->SetListenerPosition(cameraPos.x, cameraPos.y, cameraPos.z
		, cameraForward.x, cameraForward.y, cameraForward.z, cameraUp.x, cameraUp.y, cameraUp.z);

	ScrapManager::GetInstance()->Update(aDeltaTime);

	if (CU::InputWrapper::GetInstance()->KeyIsPressed(DIK_V) == true)
	{
		SET_RUNTIME(false);
		myStateStack->PushSubGameState(new ScoreState(myScores, *myScoreInfo, myLevelID));
	}

	if (CU::InputWrapper::GetInstance()->KeyDown(DIK_ESCAPE) == true)
	{
		PostMaster::GetInstance()->SendMessage(ReturnToMenuMessage());
		myIsActiveState = false;
		return eStateStatus::ePopMainState;
	}
	int playersAlive = 0;
	for each(Entity* player in myPlayers)
	{
		if (player->GetComponent<InputComponent>()->GetIsActive() == true)
		{
			++playersAlive;
			player->Update(aDeltaTime);
		}
		else
		{
			player->GetComponent<InputComponent>()->Update(aDeltaTime);
		}
	}
	PollingStation::GetInstance()->SetPlayersAlive(playersAlive);
	for each(Entity* entity in myEntities)
	{
		entity->Update(aDeltaTime);
	}

	if (myPlayerWinCount >= 1)
	{
		myTimeToLevelChange -= aDeltaTime;
		myCurrentCountdownSprite = int(myTimeToLevelChange);
		if (myTimeToLevelChange < 0.f)
		{
			SET_RUNTIME(false);
			PostMaster::GetInstance()->SendMessage(FinishLevelMessage(myLevelToChangeToID));
			myStateStack->PushSubGameState(new ScoreState(myScores, *myScoreInfo, myLevelID));
		}
	}

	
	for (int i = 0; i < myPlayers.Size(); ++i)
	{
		Entity* player = myPlayers[i];
		player->GetComponent<PlayerComponent>()->EvaluateDeath();

		Prism::PointLight* light = myPlayerPointLights[i];
		light->SetPosition(player->GetOrientation().GetPos() );
		light->Update();
	}

	myEmitterManager->UpdateEmitters(aDeltaTime);

	myShadowLight->SetPosition(mySmartCamera->GetOrientation().GetPos4() + CU::Vector4<float>(25.f, -50.f, 1.f, 1.f));
	myShadowLight->GetCamera()->Update(aDeltaTime);

	return myStateStatus;
}

void Level::Render()
{
	myFullscreenRenderer->ProcessShadow(myShadowLight, myScene);
	//myBackground->Render(myWindowSize * 0.5f);
	//myScene->Render();

	myDeferredRenderer->Render(myScene, myBackground, myShadowLight, myEmitterManager);

	myFullscreenRenderer->Render(myDeferredRenderer->GetFinishedTexture(), myDeferredRenderer->GetEmissiveTexture()
		, myDeferredRenderer->GetDepthStencilTexture(), Prism::ePostProcessing::BLOOM);


	for each(Entity* player in myPlayers)
	{
		if (player->GetComponent<MovementComponent>() != nullptr)
		{
			player->GetComponent<MovementComponent>()->Render();
		}
	}

	if (myPlayerWinCount >= 1)
	{
		CU::Vector2<float> countPos(myWindowSize.x * 0.5f, myWindowSize.y * 0.9f);
		myCountdownSprites[myCurrentCountdownSprite]->Render(countPos);
	}
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
				CU::Vector2<float> currentVelocity = second.GetComponent<MovementComponent>()->GetVelocity();
				CU::Vector3<float> velocity = { currentVelocity.x, currentVelocity.y, 0.f };
				float force = firstTrigger->GetForce();

				if ((currentVelocity.x > 0.f && currentVelocity.y > 0.f) && abs(CU::Dot(velocity, first.GetOrientation().GetUp()) < 0.85f))
				{
					second.GetComponent<MovementComponent>()->SetVelocity(currentVelocity * 0.5f);
				}

				second.GetComponent<MovementComponent>()->SetInSteam(true
					, { first.GetOrientation().GetUp().x * force, first.GetOrientation().GetUp().y * force });
				break;
			}
		}
		else if (firstTrigger->GetTriggerType() == eTriggerType::FORCE)
		{
			second.GetComponent<MovementComponent>()->SetInSteam(false);
		}
	}
	else if (second.GetType() == eEntityType::SCRAP)
	{
		if (aHasEntered == true)
		{
			switch (firstTrigger->GetTriggerType())
			{
			case eTriggerType::HAZARD:
				break;
			case eTriggerType::FORCE:
				aSecond->AddForce(first.GetOrientation().GetUp(), 10.f);
				break;
			}
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
		if (aHasEntered == true)
		{
			first->SendNote<ContactNote>(ContactNote(second, aContactPoint, aContactNormal, aHasEntered));
		}

		switch (second->GetType())
		{
		case eEntityType::SAW_BLADE:
			if (aHasEntered == true)
			{
				PostMaster::GetInstance()->SendMessage<ScrapMessage>(ScrapMessage(eScrapPart::HEAD
					, first->GetOrientation().GetPos(), first->GetComponent<MovementComponent>()->GetVelocity()));

				PostMaster::GetInstance()->SendMessage<ScrapMessage>(ScrapMessage(eScrapPart::LEGS
					, first->GetOrientation().GetPos(), first->GetComponent<MovementComponent>()->GetVelocity()));

				first->SendNote(ShouldDieNote());
			}
			break;
		case eEntityType::SPIKE:
			if (aHasEntered == true)
			{
				PostMaster::GetInstance()->SendMessage<ScrapMessage>(ScrapMessage(eScrapPart::HEAD
					, first->GetOrientation().GetPos(), { 0.f, 0.f }));

				PostMaster::GetInstance()->SendMessage<ScrapMessage>(ScrapMessage(eScrapPart::LEGS
					, first->GetOrientation().GetPos(), { 0.f, 0.f }));

				first->SendNote(ShouldDieNote());
			}
			break;
		case eEntityType::BOUNCER:
			if (aHasEntered == true)
			{
				float dot = CU::Dot(aContactNormal, second->GetOrientation().GetUp());
				float force = second->GetComponent<BounceComponent>()->GetForce();

				if (dot > 0.001f)
				{
					first->GetComponent<MovementComponent>()->SetVelocity({ second->GetOrientation().GetUp().x * force
						, second->GetOrientation().GetUp().y * force });
					second->SendNote(BounceNote());
				}
			}
			break;
		case eEntityType::GOAL_POINT:
			if (aHasEntered == true)
			{
				TriggerComponent* firstTrigger = second->GetComponent<TriggerComponent>();
				DL_ASSERT_EXP(firstTrigger != nullptr, "Goal point has to have a trigger component");
				PostMaster::GetInstance()->SendMessage(OnPlayerLevelComplete(first->GetComponent<InputComponent>()->GetPlayerID()));
				myPlayerWinCount++;
				first->GetComponent<ScoreComponent>()->ReachedGoal();

				myLevelToChangeToID = firstTrigger->GetLevelID();
				if (myPlayerWinCount >= myPlayersPlaying)
				{
					PostMaster::GetInstance()->SendMessage(FinishLevelMessage(myLevelToChangeToID));

					SET_RUNTIME(false);
					myStateStack->PushSubGameState(new ScoreState(myScores, *myScoreInfo, myLevelID));
				}

			}
			break;
		}
	}
	else if (first->GetType() == eEntityType::SCRAP)
	{
		if (aHasEntered == true)
		{
			switch (second->GetType())
			{
			case BOUNCER:
			case STEAM:
			case SPIKE:
				aFirst->AddForce(second->GetOrientation().GetUp(), 10.f);
				break;
			case SAW_BLADE:
				aFirst->AddForce(first->GetOrientation().GetPos() - second->GetOrientation().GetPos(), 10.f);
				break;
			case GOAL_POINT:
				//if (aSecond->GetSubtype() == "body")
				//{
				//	//finish level with this player part
				//}
				////break;
			default:
				break;
			}
		}
	}
}

void Level::CreatePlayers()
{
	Entity* player = EntityFactory::CreateEntity(eEntityType::PLAYER, "player", myScene, mySpawnPosition, CU::Vector3f(), CU::Vector3f(1, 1, 1), 1);
	player->GetComponent<InputComponent>()->AddController(eControllerID::Controller1);
	player->GetComponent<InputComponent>()->SetPlayerID(1);
	player->GetComponent<InputComponent>()->ResetIsInLevel();
	player->AddToScene();
	myPlayers.Add(player);
	mySmartCamera->AddOrientation(&player->GetOrientation());
	//mySmartCamera->AddOrientation(&dummyMatrix);

	player = EntityFactory::CreateEntity(eEntityType::PLAYER, "player", myScene, mySpawnPosition, CU::Vector3f(), CU::Vector3f(1,1,1), 2);
	player->GetComponent<InputComponent>()->AddController(eControllerID::Controller2);
	player->GetComponent<InputComponent>()->SetPlayerID(2);
	player->GetComponent<InputComponent>()->ResetIsInLevel();

	player->AddToScene();
	myPlayers.Add(player);

	for each(Entity* player in myPlayers)
	{
		myScores.Add(player->GetComponent<ScoreComponent>()->GetScore());

		Prism::PointLight* light = new Prism::PointLight(-1, false);
		light->SetColor({ 1.f, 1.f, 1.f, 5.f });
		light->SetRange(4.f);
		myPlayerPointLights.Add(light);
		myScene->AddLight(light);
	}

	mySmartCamera->AddOrientation(&player->GetOrientation());

	mySmartCamera->SetActivePlayerCount(0);
	mySmartCamera->SetStartPosition(mySpawnPosition);

}


void Level::EndState()
{

}

void Level::ResumeState()
{

}

void Level::PauseState()
{

}

void Level::OnResize(int aWidth, int aHeight)
{
	aWidth;
	aHeight;
}

void Level::ReceiveMessage(const OnPlayerJoin&)
{
	myPlayersPlaying++;
}

void Level::ReceiveMessage(const OnDeathMessage& aMessage)
{
	for each(Entity* player in myPlayers)
	{
		if (player->GetComponent<InputComponent>()->GetPlayerID() == aMessage.myPlayerID)
		{
			myDeferredRenderer->AddDecal(player->GetOrientation().GetPos(), player->GetOrientation().GetRight(), "Data/Resource/Texture/Decal/T_decal_test.dds");
		}
	}
}

void Level::Add(Entity* anEntity)
{
	myEntities.Add(anEntity);
	myEntities.GetLast()->AddToScene();
	myEntities.GetLast()->Reset();
}

void Level::CreateScoreInfo(float aShortTime, float aMediumTime, float aLongTime)
{
	DL_ASSERT_EXP(myScoreInfo == nullptr, "Can't create Score Info twice.");
	myScoreInfo = new ScoreInfo(aShortTime, aMediumTime, aLongTime);
}

void Level::Add(Prism::PointLight* aLight)
{
	myPointLights.Add(aLight);
	myScene->AddLight(aLight);
}
