#include "stdafx.h"

#include <AudioInterface.h>
#include "Level.h"
#include "LevelFactory.h"
#include <Camera.h>
#include "Console.h"
#include <CommonHelper.h>
#include <GameStateMessage.h>
#include <EffectContainer.h>
#include <EntityFactory.h>
#include "InGameState.h"
#include <InputWrapper.h>
#include <ModelLoader.h>
#include <MurmurHash3.h>
#include <PostMaster.h>
#include <ScriptSystem.h>
#include <VTuneApi.h>
#include <TextProxy.h>
#include <SpriteProxy.h>
#include <Cursor.h>
#include <FinishLevelMessage.h>

InGameState::InGameState(int aLevelID)
	: myGUIManager(nullptr)
	, myLevelToLoad(aLevelID)
	, myState(eInGameState::LEVEL)
	, myLevel(nullptr)
	, myLevelFactory(nullptr)
	, myFailedLevelHash(false)
	, myHasStartedMusicBetweenLevels(false)
	, myLastLevel(aLevelID)
{
	myIsActiveState = false;

	//myCameraOrientation.SetPos(CU::Vector3<float>(0, 10, -7.f));

	myCamera = new Prism::Camera(myCameraOrientation);
	myCamera->Update(1.f / 30.f);

	CU::Vector2<int> windowSize = Prism::Engine::GetInstance()->GetWindowSizeInt();
	OnResize(windowSize.x, windowSize.y);
	PostMaster::GetInstance()->Subscribe(eMessageType::LEVEL_FINISHED, this);
}

InGameState::~InGameState()
{
	PostMaster::GetInstance()->UnSubscribe(eMessageType::LEVEL_FINISHED, this);
	PostMaster::GetInstance()->UnSubscribe(eMessageType::GAME_STATE, this);
	Console::Destroy();
	SAFE_DELETE(myLevel);
	SAFE_DELETE(myCamera);
	SAFE_DELETE(myLevelFactory);
	SAFE_DELETE(myText);
}

void InGameState::InitState(StateStackProxy* aStateStackProxy, GUI::Cursor* aCursor)
{
	myIsLetThrough = false;
	myStateStack = aStateStackProxy;
	myStateStatus = eStateStatus::eKeepState;
	myCursor = aCursor;
	myCursor->SetShouldRender(false);

	EntityFactory::GetInstance()->LoadEntities("GeneratedData/LI_entity.xml");
	myLevelFactory = new LevelFactory("GeneratedData/LI_level.xml", *myCamera);
	myLevel = myLevelFactory->LoadLevel(1);

	myIsActiveState = true;

	myText = Prism::ModelLoader::GetInstance()->LoadText(Prism::Engine::GetInstance()->GetFont(Prism::eFont::CONSOLE));
	myText->SetPosition(CU::Vector2<float>(800.f, 800.f));

	PostMaster::GetInstance()->Subscribe(eMessageType::GAME_STATE, this);
}

void InGameState::EndState()
{
}

const eStateStatus InGameState::Update(const float& aDeltaTime)
{
	if (myLevel != nullptr)
	{
		myLevel->Update(aDeltaTime);
	}

	return myStateStatus;
}

void InGameState::Render()
{
	myLevel->Render();
}

void InGameState::ResumeState()
{
	myIsActiveState = true;
	myLevelToLoad = -1;
}

void InGameState::ReceiveMessage(const GameStateMessage& aMessage)
{
	if (myState != eInGameState::LEVEL)
	{
		switch (aMessage.myGameState)
		{
		case eGameState::LOAD_LEVEL:
			myLevelToLoad = aMessage.myID;
			myState = eInGameState::LOAD_LEVEL;
			break;
		}
	}
}


void InGameState::ReceiveMessage(const FinishLevelMessage& aMessage)
{
	SET_RUNTIME(false);
	myLevel = myLevelFactory->LoadLevel(aMessage.myLevelID);
	SET_RUNTIME(true);
}

void InGameState::OnResize(int aWidth, int aHeight)
{
	aWidth;
	aHeight;
}