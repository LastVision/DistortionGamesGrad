#include "stdafx.h"
#include <ControllerInput.h>
#include <Cursor.h>
#include <FadeMessage.h>
#include "HatState.h"
#include <HatManager.h>
#include "HatUnlockState.h"
#include "HatsSelectionState.h"
#include <GUIManager.h>
#include <InputWrapper.h>
#include <ModelLoader.h>
#include <OnClickMessage.h>
#include <PostMaster.h>
#include <SpriteProxy.h>

HatState::HatState()
	: myGUIManager(nullptr)
{
}

HatState::~HatState()
{
	PostMaster::GetInstance()->UnSubscribe(this, 0);
	SAFE_DELETE(myGUIManager);
	myCursor = nullptr;
}

void HatState::InitState(StateStackProxy* aStateStackProxy, CU::ControllerInput* aController, GUI::Cursor* aCursor)
{
	myIsActiveState = true;
	myIsLetThrough = false;
	myStateStatus = eStateStatus::eKeepState;
	myStateStack = aStateStackProxy;
	myCursor = aCursor;
	myController = aController;
	myCursor->SetShouldRender(true);
	myGUIManager = new GUI::GUIManager(myCursor, "Data/Resource/GUI/GUI_hat.xml", nullptr, -1);

	CU::Vector2<int> windowSize = Prism::Engine::GetInstance()->GetWindowSizeInt();
	OnResize(windowSize.x, windowSize.y);

	InitControllerInMenu(myController, myGUIManager, myCursor);
	myController->SetIsInMenu(true);

	while (Prism::ModelLoader::GetInstance()->IsLoading() == true);
	PostMaster::GetInstance()->Subscribe(this, eMessageType::ON_CLICK);

	PostMaster::GetInstance()->SendMessage(FadeMessage(1.f / 3.f));
}

void HatState::EndState()
{
}

void HatState::OnResize(int aWidth, int aHeight)
{
	myGUIManager->OnResize(aWidth, aHeight);
}

const eStateStatus HatState::Update(const float& aDeltaTime)
{
	if (CU::InputWrapper::GetInstance()->KeyDown(DIK_ESCAPE) == true || myController->ButtonOnDown(eXboxButton::BACK)
		|| myController->ButtonOnDown(eXboxButton::B))
	{
		myIsActiveState = false;
		myCursor->SetShouldRender(false);
		return eStateStatus::ePopMainState;
	}

	HandleControllerInMenu(myController, myGUIManager, myCursor);
	myGUIManager->Update(aDeltaTime);


	return myStateStatus;
}

void HatState::Render()
{
	myGUIManager->Render();
}

void HatState::ResumeState()
{
	InitControllerInMenu(myController, myGUIManager, myCursor);
	myController->SetIsInMenu(true);
	PostMaster::GetInstance()->Subscribe(this, eMessageType::ON_CLICK);
	PostMaster::GetInstance()->SendMessage(FadeMessage(1.f / 3.f));
}

void HatState::PauseState()
{
	PostMaster::GetInstance()->UnSubscribe(this, eMessageType::ON_CLICK);
}

void HatState::ReceiveMessage(const OnClickMessage& aMessage)
{
	switch (aMessage.myEvent)
	{
	case eOnClickEvent::HAT_SELECTION:
		SET_RUNTIME(false);
		myStateStack->PushSubGameState(new HatsSelectionState());
		break;
	case eOnClickEvent::HAT_UNLOCK:
		SET_RUNTIME(false);
		myStateStack->PushSubGameState(new HatUnlockState());
		break;
	case eOnClickEvent::HAT_QUIT:
		myStateStatus = eStateStatus::ePopMainState;
		break;
	}
}