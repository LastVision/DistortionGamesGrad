#include "stdafx.h"

#include <AudioInterface.h>
#include <BounceComponent.h>
#include <BounceNote.h>
#include <Camera.h>
#include <ContactNote.h>
#include <ControllerInput.h>
#include <DirectionalLight.h>
#include <DeferredRenderer.h>
#include "EmitterManager.h"
#include <EmitterMessage.h>
#include <EntityFactory.h>
#include <FadeMessage.h>
#include <FinishLevelMessage.h>
#include "FirstTimeFinishLevelState.h"
#include <InputComponent.h>
#include <InputWrapper.h>
#include "Level.h"
#include <MovementComponent.h>
#include <ModelLoader.h>
#include <OnPlayerLevelComplete.h>
#include <OnDeathMessage.h>
#include <PhysicsComponent.h>
#include <PhysicsInterface.h>
#include <PlayerActiveMessage.h>
#include <PlayAnimationNote.h>
#include <PlayerComponent.h>
#include <PollingStation.h>
#include <PostMaster.h>
#include <ReachedGoalNote.h>
#include <Renderer.h>
#include <ReturnToMenuMessage.h>
#include <Scene.h>
#include <ScoreComponent.h>
#include "ScoreInfo.h"
#include <ScoreComponent.h>
#include "ScoreState.h"
#include "ScrapManager.h"
#include <ScrapMessage.h>
#include <ShouldDieNote.h>
#include "SmartCamera.h"
#include <SoundComponent.h>
#include <SpotLight.h>
#include <SpotLightShadow.h>
#include <SpriteProxy.h>
#include <StomperComponent.h>
#include <PlayerGraphicsComponent.h>
#include <TriggerComponent.h>
#include <TextureContainer.h>
#include "PauseMenuState.h"
#include <PointLight.h>
#include <ReachedGoalMessage.h>
#include <VibrationNote.h>

#include <Texture.h>

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
	, myScrapManagers(4)
	, myDirectionalLights(4)
	, mySpotLights(16)
	, myShouldRenderCountDown(true)
	, myShouldFinishLevel(false)
	, myPressToStartSprite(nullptr)
	, myPlayerDeathInfos(8)
	, myPressToStartAlpha(1.f)
	, myPressToStartIsFading(true)
	, myShortestTimeSincePlayerDeath(0.f)
	, myTimeBeforeRenderingPressToStart(4.f)
	, myTimeBeforeRemovingPressToStartForPlayer(8.f)
{
	Prism::PhysicsInterface::Create(std::bind(&Level::CollisionCallback, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3)
		, std::bind(&Level::ContactCallback, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3
		, std::placeholders::_4, std::placeholders::_5));

	myScene = new Prism::Scene();
	myScene->SetCamera(aCamera);
	myWindowSize = Prism::Engine::GetInstance()->GetWindowSize();
	PostMaster::GetInstance()->Subscribe(this, eMessageType::ON_PLAYER_JOIN | eMessageType::ON_DEATH | eMessageType::REACHED_GOAL);

	myEmitterManager = new Prism::EmitterManager();
	myEmitterManager->Initiate(&myCamera);
	Prism::ModelLoader::GetInstance()->Pause();
	myBackground = Prism::TextureContainer::GetInstance()->GetTexture("Data/Resource/Texture/T_background.dds");
	myDeferredRenderer = new Prism::DeferredRenderer();
	myFullscreenRenderer = new Prism::Renderer();
	myShadowLight = new Prism::SpotLightShadow(aCamera.GetOrientation());
	Prism::ModelLoader::GetInstance()->UnPause();

	CU::Vector2<float> size(256.f, 256.f);
	std::string texturePath("Data/Resource/Texture/Countdown/T_countdown_");
	for (int i = 0; i < 10; ++i)
	{
		myCountdownSprites[i] = Prism::ModelLoader::GetInstance()->LoadSprite(texturePath + std::to_string(i + 1) + ".dds", size, size * 0.5f);
	}

	PollingStation::Create();

	myPressToStartSprite = Prism::ModelLoader::GetInstance()->LoadSprite("Data/Resource/Texture/UI/T_press_to_start.dds"
		, { 512.f, 512.f }, { 256.f, 256.f });
}

Level::~Level()
{
	for (int i = 0; i < 10; ++i)
	{
		SAFE_DELETE(myCountdownSprites[i]);
	}
	SAFE_DELETE(myEmitterManager);
	for (int i = 0; i < myScrapManagers.Size(); ++i)
	{
		SAFE_DELETE(myScrapManagers[i]);
	}
	for (int i = 0; i < myPlayerDeathInfos.Size(); ++i)
	{
		SAFE_DELETE(myPlayerDeathInfos[i].myJoinGameSprite);
	}
	SAFE_DELETE(myShadowLight);
	SAFE_DELETE(mySmartCamera);
	SAFE_DELETE(myScene);
	SAFE_DELETE(myDeferredRenderer);
	SAFE_DELETE(myFullscreenRenderer);
	SAFE_DELETE(myScoreInfo);
	SAFE_DELETE(myPressToStartSprite);
	myEntities.DeleteAll();
	myPlayers.DeleteAll();
	myPointLights.DeleteAll();
	myPlayerPointLights.DeleteAll();
	myDirectionalLights.DeleteAll();
	mySpotLights.DeleteAll();
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

	for (int i = 0; i < myPlayers.Size(); ++i)
	{
		myPlayers[i]->GetComponent<MovementComponent>()->SetSpawnVelocity(mySpawnVelocity);

		myScrapManagers.Add(new ScrapManager(myScene, myPlayers[i]->GetComponent<InputComponent>()->GetPlayerID()));
	}
	myController->SetIsInMenu(false);

	//PostMaster::GetInstance()->SendMessage(FadeMessage(1.f / 3.f));

	myScene->SortInstances();


	myFullscreenRenderer->ProcessShadow(myShadowLight, myScene);
}

const eStateStatus Level::Update(const float& aDeltaTime)
{
	if (myIsFreeCam == false)
	{
		myShadowLight->SetPosition(mySmartCamera->GetOrientation().GetPos4() + CU::Vector4<float>(25.f, -50.f, 1.f, 1.f));
	}
	else
	{
		myShadowLight->SetPosition(myCamera.GetOrientation().GetPos4() + CU::Vector4<float>(25.f, -50.f, 1.f, 1.f));
	}
	myShadowLight->GetCamera()->Update(aDeltaTime);

#ifndef THREAD_PHYSICS
	Prism::PhysicsInterface::GetInstance()->FrameUpdate();
#endif
	if (myIsFreeCam == true)
	{
		UpdateInput(aDeltaTime);
		myCamera.Update(aDeltaTime);
	}
	else
	{
		mySmartCamera->Update(aDeltaTime);
	}
	CU::Vector3<float> cameraPos(mySmartCamera->GetOrientation().GetPos());
	CU::Vector3<float> cameraForward(mySmartCamera->GetOrientation().GetForward());
	CU::Vector3<float> cameraUp(mySmartCamera->GetOrientation().GetUp());
	Prism::Audio::AudioInterface::GetInstance()->SetListenerPosition(cameraPos.x, cameraPos.y, cameraPos.z
		, cameraForward.x, cameraForward.y, cameraForward.z, cameraUp.x, cameraUp.y, cameraUp.z);

	for (int i = 0; i < myScrapManagers.Size(); ++i)
	{
		myScrapManagers[i]->Update(aDeltaTime);
	}

	if (CU::InputWrapper::GetInstance()->KeyDown(DIK_F9))
	{
		myCamera.SetOrientation(myOrientation);
		myIsFreeCam = !myIsFreeCam;
	}

#ifndef RELEASE_BUILD
	if (CU::InputWrapper::GetInstance()->KeyIsPressed(DIK_V) == true)
	{
		myShouldFinishLevel = true;
	}

	if (CU::InputWrapper::GetInstance()->KeyDown(DIK_1))
	{
		GC::DebugRenderTexture = 0;
	}
	else if (CU::InputWrapper::GetInstance()->KeyDown(DIK_2))
	{
		GC::DebugRenderTexture = 1;
	}
	else if (CU::InputWrapper::GetInstance()->KeyDown(DIK_3))
	{
		GC::DebugRenderTexture = 2;
	}
	else if (CU::InputWrapper::GetInstance()->KeyDown(DIK_4))
	{
		GC::DebugRenderTexture = 3;
	}
	else if (CU::InputWrapper::GetInstance()->KeyDown(DIK_5))
	{
		GC::DebugRenderTexture = 4;
	}
#endif

	if (CU::InputWrapper::GetInstance()->KeyDown(DIK_ESCAPE) == true ||
		(myController->IsConnected() == true && myController->ButtonOnDown(eXboxButton::START)))
	{
		/*PostMaster::GetInstance()->SendMessage(ReturnToMenuMessage());
		myIsActiveState = false;
		return eStateStatus::ePopMainState;*/

		SET_RUNTIME(false);
		myStateStack->PushSubGameState(new PauseMenuState());
	}
	int playersAlive = 0;
	for each(Entity* player in myPlayers)
	{
		if (player->GetComponent<InputComponent>()->GetIsActive() == true 
			&& player->GetComponent<ScoreComponent>()->GetScore()->myReachedGoal == false)
		{
			++playersAlive;
			player->GetComponent<InputComponent>()->SetPlayersWinCount(myPlayerWinCount);
			player->Update(aDeltaTime);
		}
		else
		{
			player->GetComponent<InputComponent>()->Update(aDeltaTime);
			player->GetComponent<ScoreComponent>()->Update(aDeltaTime);
		}
	}
	PollingStation::GetInstance()->SetPlayersAlive(playersAlive);
	for each(Entity* entity in myEntities)
	{
		entity->Update(aDeltaTime);
	}

	float shortestTime = 1000.f;
	bool playerIsActive = false;

	for (int i = 0; i < myPlayers.Size(); ++i)
	{
		Entity* player = myPlayers[i];
		player->GetComponent<PlayerComponent>()->EvaluateDeath();

		Prism::PointLight* light = myPlayerPointLights[i];
		light->SetPosition(player->GetOrientation().GetPos());
		light->Update();

		DeathInfo& info = myPlayerDeathInfos[i];

		if (player->GetComponent<PlayerComponent>()->GetIsAlive() == true)
		{
			info.myHasBeenActive = true;
			info.myShouldRender = false;
			info.myTimeSincePlayerDeath = 0.f;
		}
		else
		{
			if (info.myHasBeenActive == true)
			{
				info.myTimeSincePlayerDeath += aDeltaTime;
				if (info.myTimeSincePlayerDeath >= myTimeBeforeRemovingPressToStartForPlayer + myTimeBeforeRenderingPressToStart)
				{
					info.myHasBeenActive = false;
				}
				else if (info.myTimeSincePlayerDeath >= myTimeBeforeRenderingPressToStart)
				{
					info.myShouldRender = true;
				}
				playerIsActive = true;
				myShortestTimeSincePlayerDeath = fmin(info.myTimeSincePlayerDeath, shortestTime);
			}
		}
	}
	if (playerIsActive == false)
	{
		myShortestTimeSincePlayerDeath += aDeltaTime;
	}

	if (myPlayerWinCount >= 1)
	{
		myTimeToLevelChange -= aDeltaTime;
		myCurrentCountdownSprite = int(myTimeToLevelChange);
		if (myTimeToLevelChange < 0.f || playersAlive == 0)
		{
			myShouldFinishLevel = true;
		}
	}
	else
	{
		myShouldRenderCountDown = true;
	}

	myDeferredRenderer->Update(aDeltaTime);
	myEmitterManager->UpdateEmitters(aDeltaTime);

	if (myShouldFinishLevel == true)
	{
		FinishLevel();
	}

	if (myPressToStartIsFading == true)
	{
		myPressToStartAlpha -= aDeltaTime;
		if (myPressToStartAlpha <= 0.f)
		{
			myPressToStartAlpha = 0.f;
			myPressToStartIsFading = false;
		}
	}
	else
	{
		myPressToStartAlpha += aDeltaTime;
		if (myPressToStartAlpha >= 1.f)
		{
			myPressToStartAlpha = 1.f;
			myPressToStartIsFading = true;
		}
	}

	return myStateStatus;
}

void Level::Render()
{
	if (GC::OptionsUseShadows == true && GC::EnableDynamicShadows == true)
	{
		myFullscreenRenderer->ProcessShadow(myShadowLight, myScene);
	}

	myDeferredRenderer->Render(myScene, myBackground, myShadowLight, myEmitterManager);
	myScene->DebugPrint();

	if (GC::DebugRenderTexture == 0)
	{
		myFullscreenRenderer->Render(myDeferredRenderer->GetFinishedTexture(), myDeferredRenderer->GetFinishedTexture()
			, myDeferredRenderer->GetDepthStencilTexture(), Prism::ePostProcessing::BLOOM);
	}
	else
	{
		myFullscreenRenderer->DebugRender(myDeferredRenderer->GetGBuffer());
	}

	for (int i = 0; i < myPlayers.Size(); ++i)
	{
		Entity* player = myPlayers[i];

		if (player->GetComponent<MovementComponent>() != nullptr)
		{
			player->GetComponent<MovementComponent>()->Render();
		}
	}

	if (myPlayerWinCount >= 1 && myShouldRenderCountDown == true)
	{
		CU::Vector2<float> countPos(myWindowSize.x * 0.5f, myWindowSize.y * 0.9f);
		myCountdownSprites[myCurrentCountdownSprite]->Render(countPos);
	}

#ifndef _DEBUG
	if (myPlayerWinCount == 0)
	{
		if (PollingStation::GetInstance()->GetPlayersAlive() == 0)
		{
			if (myShortestTimeSincePlayerDeath >= myTimeBeforeRenderingPressToStart)
			{
				myPressToStartSprite->Render({ myWindowSize.x * 0.5f, myWindowSize.y * 0.3f }, { 1.f, 1.f }
					, { 1.f, 1.f, 1.f, myPressToStartAlpha });
			}
		}
		else
		{
			for (int i = 0; i < myPlayerDeathInfos.Size(); i++)
			{
				DeathInfo& info = myPlayerDeathInfos[i];
				if (info.myShouldRender == true && info.myHasBeenActive == true)
				{
					info.myJoinGameSprite->Render({ myWindowSize.x * 0.5f - 200.f + i * 400.f, myWindowSize.y * 0.2f }
						, { 1.f, 1.f }, { 1.f, 1.f, 1.f, myPressToStartAlpha });
				}
			}
		}
	}
#endif
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
				float SteamLength = first.GetComponent<PhysicsComponent>()->GetHeight();

				if ((currentVelocity.x > 0.f && currentVelocity.y > 0.f) && abs(CU::Dot(velocity, first.GetOrientation().GetUp()) < 0.85f))
				{
					second.GetComponent<MovementComponent>()->SetVelocity(currentVelocity * 0.5f);
				}

				second.GetComponent<MovementComponent>()->SetInSteam(true, force, SteamLength
					, { first.GetOrientation().GetUp().x, first.GetOrientation().GetUp().y }, first.GetOrientation().GetPos());
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
				KillPlayer(first, first->GetComponent<MovementComponent>()->GetVelocity());

				CU::Vector3f dir = second->GetOrientation().GetPos() - first->GetOrientation().GetPos();
				CU::Normalize(dir);
				PostMaster::GetInstance()->SendMessage(EmitterMessage("Saw_Blade", first->GetOrientation().GetPos(), true, -dir, true));
				PostMaster::GetInstance()->SendMessage(EmitterMessage("Oil", first->GetOrientation().GetPos(), true, -dir, true));
				Prism::Audio::AudioInterface::GetInstance()->PostEvent("Play_Sawed", second->GetComponent<SoundComponent>()->GetAudioSFXID());

			}
			break;
		case eEntityType::SPIKE:
			if (aHasEntered == true)
			{
				KillPlayer(first);
				CU::Vector3f dir = aContactNormal;
				CU::Normalize(dir);
				PostMaster::GetInstance()->SendMessage(EmitterMessage("Spike", first->GetOrientation().GetPos(), true, second->GetOrientation().GetUp(), true));
				PostMaster::GetInstance()->SendMessage(EmitterMessage("Oil", first->GetOrientation().GetPos(), true, second->GetOrientation().GetUp(), true));
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
					PostMaster::GetInstance()->SendMessage(EmitterMessage("Bounce", second->GetOrientation().GetPos(), true, second->GetOrientation().GetUp()));
				}
				else if (first->GetComponent<MovementComponent>()->IsInDashFly() == true)
				{
					KillPlayer(first);
				}
			}
			break;
		case eEntityType::STOMPER:
			if (aHasEntered == true)
			{
				float dot = CU::Dot(aContactNormal, second->GetOrientation().GetUp());

				if (dot > 0.001f)
				{
					if (second->IsStomperMoving() == true)
					{
						KillPlayer(first);
						PostMaster::GetInstance()->SendMessage(EmitterMessage("Stomper", first->GetOrientation().GetPos(), true));
					}
					else
					{
						first->GetComponent<InputComponent>()->SetStandingOnStomper(second);
					}
				}
			}
			else
			{
				first->GetComponent<InputComponent>()->SetStandingOnStomper(nullptr);
			}
			break;
		case eEntityType::ACID_DROP:
			if (aHasEntered == true)
			{
				Prism::Audio::AudioInterface::GetInstance()->PostEvent("Play_Acid", second->GetComponent<SoundComponent>()->GetAudioSFXID());
				KillPlayer(first);
				second->SetShouldBeRemoved(true);
				CU::Vector3f dir = aContactNormal;
				CU::Normalize(dir);
				PostMaster::GetInstance()->SendMessage(EmitterMessage("Acid", second->GetOrientation().GetPos(), true, -dir, true));
			}
			break;
		case eEntityType::GOAL_POINT:
			if (aHasEntered == true)
			{
				
				//myPlayerWinCount++;
				//first->GetComponent<ScoreComponent>()->ReachedGoal();

				first->SendNote(ReachedGoalNote(second));


			}
			break;
		default:
			first->GetComponent<PlayerComponent>()->HandleCollision(second);
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
				if (first->IsInScene() == true)
				{
					second->SendNote(BounceNote());
				}
			case STEAM:
			case SPIKE:
				//aFirst->AddForce(second->GetOrientation().GetUp(), 10.f);
				break;
			case SAW_BLADE:
				if (aFirst->GetEntity().IsScrapSpawnedThisFrame() == false)
				{
					aFirst->AddForce(first->GetOrientation().GetPos() - second->GetOrientation().GetPos(), 10.f);
				}
				else
				{
					int hereAScrapIsSpawnedAndShouldNotGetForce = 5;
					hereAScrapIsSpawnedAndShouldNotGetForce;
				}
				break;
			case GOAL_POINT:
				break;
			case ACID_DROP:
				second->SetShouldBeRemoved(true);
				break;
			default:
				break;
			}
		}
	}
	else if (first->GetType() == eEntityType::ACID_DROP)
	{
		if (aHasEntered == true && second->GetType() != eEntityType::ACID && second->GetType() != eEntityType::ACID_DROP)
		{
			first->SetShouldBeRemoved(true);

			Prism::Audio::AudioInterface::GetInstance()->PostEvent("Play_Acid", first->GetComponent<SoundComponent>()->GetAudioSFXID());
			if (second->GetType() == eEntityType::PLAYER)
			{
				KillPlayer(second);
				//Acid death effect
			}
			else
			{
				CU::Vector3f dir = aContactNormal;
				CU::Normalize(dir);
				PostMaster::GetInstance()->SendMessage(EmitterMessage("Acid", second->GetOrientation().GetPos(), true, dir, true));
				myDeferredRenderer->AddDecal(aContactPoint, -aContactNormal, eDecalType::LAVA);
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
	mySmartCamera->AddPlayer(&player->GetOrientation(), &player->GetComponent<MovementComponent>()->GetAverageVelocity());
	//mySmartCamera->AddOrientation(&dummyMatrix);

	player = EntityFactory::CreateEntity(eEntityType::PLAYER, "player", myScene, mySpawnPosition, CU::Vector3f(), CU::Vector3f(1, 1, 1), 2);
	player->GetComponent<InputComponent>()->AddController(eControllerID::Controller2);
	player->GetComponent<InputComponent>()->SetPlayerID(2);
	player->GetComponent<InputComponent>()->ResetIsInLevel();

	player->AddToScene();
	myPlayers.Add(player);

	for each(Entity* player in myPlayers)
	{
		myScores.Add(player->GetComponent<ScoreComponent>()->GetScore());

		Prism::PointLight* light = new Prism::PointLight(false);
		light->SetColor({ 1.f, 1.f, 1.f, 5.f });
		light->SetRange(4.f);
		myPlayerPointLights.Add(light);
		myScene->AddLight(light);

		myPlayerDeathInfos.Add(DeathInfo());
	}

	myPlayerDeathInfos[0].myJoinGameSprite = Prism::ModelLoader::GetInstance()->LoadSprite("Data/Resource/Texture/UI/T_press_to_start_player_one.dds"
		, { 256.f, 256.f }, { 128.f, 128.f });
	myPlayerDeathInfos[1].myJoinGameSprite = Prism::ModelLoader::GetInstance()->LoadSprite("Data/Resource/Texture/UI/T_press_to_start_player_two.dds"
		, { 256.f, 256.f }, { 128.f, 128.f });

	mySmartCamera->AddPlayer(&player->GetOrientation(), &player->GetComponent<MovementComponent>()->GetAverageVelocity());

	mySmartCamera->SetActivePlayerCount(0);
	mySmartCamera->SetStartPosition(mySpawnPosition);
}

void Level::EndState()
{
	for each(Entity* player in myPlayers)
	{
		player->SendNote(VibrationNote(0, 0, 0));
	}
}

void Level::ResumeState()
{
	//höj ingame musik

	if (GC::NightmareMode == true)
	{
		Prism::Audio::AudioInterface::GetInstance()->PostEvent("Increase_NightmareInGame", 0);
	}
	else
	{
		Prism::Audio::AudioInterface::GetInstance()->PostEvent("Increase_InGameMusic", 0);
	}

	myController->SetIsInMenu(false);
	PostMaster::GetInstance()->SendMessage(FadeMessage(1.f / 3.f));
}

void Level::PauseState()
{
	if (GC::NightmareMode == true)
	{
		Prism::Audio::AudioInterface::GetInstance()->PostEvent("Lower_NightmareInGame", 0);
	}
	else
	{
		Prism::Audio::AudioInterface::GetInstance()->PostEvent("Lower_InGameMusic", 0);
	}
	for each(Entity* player in myPlayers)
	{
		player->SendNote(VibrationNote(0, 0, 0));
	}
}

void Level::OnResize(int aWidth, int aHeight)
{
	myDeferredRenderer->OnResize(static_cast<float>(aWidth), static_cast<float>(aHeight));
	myFullscreenRenderer->OnResize(static_cast<float>(aWidth), static_cast<float>(aHeight));
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
			myDeferredRenderer->AddDecal(player->GetOrientation().GetPos(), player->GetOrientation().GetRight(), eDecalType::OIL);
		}
	}
}

void Level::ReceiveMessage(const ReachedGoalMessage& aMessage)
{
	const TriggerComponent* firstTrigger = aMessage.myGoalEntity->GetComponent<TriggerComponent>();
	DL_ASSERT_EXP(firstTrigger != nullptr, "Goal point has to have a trigger component");

	PostMaster::GetInstance()->SendMessage(OnPlayerLevelComplete(aMessage.myPlayer->GetComponent<InputComponent>()->GetPlayerID()));
	PostMaster::GetInstance()->SendMessage(EmitterMessage("Goal", aMessage.myPlayer->GetOrientation().GetPos()));

	myLevelToChangeToID = firstTrigger->GetLevelID();

	++myPlayerWinCount;

	if (myPlayerWinCount >= myPlayersPlaying)
	{
		myShouldFinishLevel = true;
	}

	Prism::Audio::AudioInterface::GetInstance()->PostEvent("Play_LevelWon", 0);
}

void Level::Add(Entity* anEntity)
{
	myEntities.Add(anEntity);
	myEntities.GetLast()->AddToScene();
	myEntities.GetLast()->Reset();
}

void Level::Add(Prism::DirectionalLight* aLight)
{
	myDirectionalLights.Add(aLight);
	myScene->AddLight(aLight);
}

void Level::Add(Prism::SpotLight* aLight)
{
	mySpotLights.Add(aLight);
	myScene->AddLight(aLight);
}

void Level::KillPlayer(Entity* aPlayer, const CU::Vector2<float>& aGibsVelocity)
{
	int playerID = aPlayer->GetComponent<InputComponent>()->GetPlayerID();
	if (aPlayer->GetComponent<PlayerGraphicsComponent>()->GetHeadActive() == true)
	{
		PostMaster::GetInstance()->SendMessage<ScrapMessage>(ScrapMessage(eScrapPart::HEAD
			, aPlayer->GetOrientation().GetPos(), aGibsVelocity, playerID));
	}
	if (aPlayer->GetComponent<PlayerGraphicsComponent>()->GetLegsActive() == true)
	{
		PostMaster::GetInstance()->SendMessage<ScrapMessage>(ScrapMessage(eScrapPart::LEGS
			, aPlayer->GetOrientation().GetPos(), aGibsVelocity, playerID));
	}
	aPlayer->SendNote(ShouldDieNote());
}

void Level::UpdateInput(float aDeltaTime)
{
	const float moveSpeed = 25.f;
	if (CU::InputWrapper::GetInstance()->KeyIsPressed(DIK_W))
	{
		myCamera.MoveForward(moveSpeed * aDeltaTime);
	}
	if (CU::InputWrapper::GetInstance()->KeyIsPressed(DIK_S))
	{
		myCamera.MoveForward(-moveSpeed * aDeltaTime);
	}
	if (CU::InputWrapper::GetInstance()->KeyIsPressed(DIK_SPACE))
	{
		myCamera.MoveUp(moveSpeed * aDeltaTime);
	}
	if (CU::InputWrapper::GetInstance()->KeyIsPressed(DIK_X))
	{
		myCamera.MoveUp(-moveSpeed * aDeltaTime);
	}
	if (CU::InputWrapper::GetInstance()->KeyIsPressed(DIK_D))
	{
		myCamera.MoveRight(moveSpeed * aDeltaTime);
	}
	if (CU::InputWrapper::GetInstance()->KeyIsPressed(DIK_A))
	{
		myCamera.MoveRight(-moveSpeed * aDeltaTime);
	}

	const float rotateSpeed = 25.f;

	if (CU::InputWrapper::GetInstance()->KeyIsPressed(DIK_UPARROW))
	{
		myCamera.RotateX(-rotateSpeed * aDeltaTime);
	}
	if (CU::InputWrapper::GetInstance()->KeyIsPressed(DIK_DOWNARROW))
	{
		myCamera.RotateX(rotateSpeed * aDeltaTime);
	}
	if (CU::InputWrapper::GetInstance()->KeyIsPressed(DIK_RIGHTARROW))
	{
		myCamera.RotateY(rotateSpeed * aDeltaTime);
	}
	if (CU::InputWrapper::GetInstance()->KeyIsPressed(DIK_LEFTARROW))
	{
		myCamera.RotateY(-rotateSpeed * aDeltaTime);
	}
	if (CU::InputWrapper::GetInstance()->KeyIsPressed(DIK_Q))
	{
		myCamera.RotateZ(rotateSpeed * aDeltaTime);
	}
	if (CU::InputWrapper::GetInstance()->KeyIsPressed(DIK_E))
	{
		myCamera.RotateZ(-rotateSpeed * aDeltaTime);
	}

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

void Level::FinishLevel()
{
	myShouldRenderCountDown = false;
	PostMaster::GetInstance()->SendMessage(FinishLevelMessage(myLevelToChangeToID));
	if (GC::FirstTimeScoreSubmit == true)
	{
		SET_RUNTIME(false);
		myStateStack->PushSubGameState(new ScoreState(myScores, *myScoreInfo, myLevelID));
	}
	else
	{
		SET_RUNTIME(false);
		myStateStack->PushSubGameState(new FirstTimeFinishLevelState(myScores, *myScoreInfo, myLevelID));
	}
}