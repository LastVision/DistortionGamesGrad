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
	, myLevelFactory(nullptr)
	, myFailedLevelHash(false)
	, myHasStartedMusicBetweenLevels(false)
	, myLastLevel(aLevelID)
	, myNextLevel(-1)
{
	myIsActiveState = false;

	//myCameraOrientation.SetPos(CU::Vector3<float>(0, 10, -7.f));

	myCamera = new Prism::Camera(myCameraOrientation);
	myCamera->Update(1.f / 30.f);

	CU::Vector2<int> windowSize = Prism::Engine::GetInstance()->GetWindowSizeInt();
	OnResize(windowSize.x, windowSize.y);
}

InGameState::~InGameState()
{
	PostMaster::GetInstance()->UnSubscribe(this, 0);

	Console::Destroy();
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
	//myLevel = myLevelFactory->LoadLevel(1);


	myIsActiveState = true;

	myText = Prism::ModelLoader::GetInstance()->LoadText(Prism::Engine::GetInstance()->GetFont(Prism::eFont::CONSOLE));
	myText->SetPosition(CU::Vector2<float>(800.f, 800.f));

	PostMaster::GetInstance()->Subscribe(this, eMessageType::LEVEL_FINISHED);

	myNextLevel = 1;
}

void InGameState::EndState()
{
}

const eStateStatus InGameState::Update(const float&)
{
	SET_RUNTIME(false);
	Level* level = nullptr;
	if (myLevelFactory->LoadLevel(level) == true)
	{
		myStateStack->PushMainGameState(level);
	}
	else
	{
		//Game over, push "win"-state
	}

	return myStateStatus;
}

void InGameState::Render()
{
}

void InGameState::ResumeState()
{
	myIsActiveState = true;
	myLevelToLoad = -1;
}

void InGameState::ReceiveMessage(const FinishLevelMessage& aMessage)
{
	myNextLevel = aMessage.myLevelID;
}

void InGameState::OnResize(int aWidth, int aHeight)
{
	aWidth;
	aHeight;
}