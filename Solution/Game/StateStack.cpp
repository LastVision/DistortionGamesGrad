#include "stdafx.h"

#include <ControllerInput.h>
#include "StateStack.h"
#include <DL_Assert.h>
#include "GameState.h"
#include <ModelLoader.h>

StateStack::StateStack()
	: myController(new CU::ControllerInput(eControllerID::Controller1))
	, myCursor(nullptr)
	, myShouldRender(true)
{
	myMainIndex = -1;
	mySubIndex = -1;
	myGameStates.Init(16);

	myStateStackProxy = new StateStackProxy(*this);
}

StateStack::~StateStack()
{
	SAFE_DELETE(myController);
	delete myStateStackProxy;
}

void StateStack::PopSubGameState()
{
	DL_ASSERT_EXP(myGameStates.Size() > 0, "Can't pop an empty stack.");

	myGameStates[myMainIndex][mySubIndex]->EndState();

	myGameStates[myMainIndex].DeleteCyclicAtIndex(mySubIndex);
	--mySubIndex;

	if (mySubIndex >= 0)
	{
		myGameStates[myMainIndex][mySubIndex]->ResumeState();
	}
}

void StateStack::PopMainGameState()
{
	while (myGameStates[myMainIndex].Size() > 0)
	{
		PopSubGameState();
	}

	myGameStates.RemoveCyclicAtIndex(myMainIndex);
	--myMainIndex;

	if (myMainIndex >= 0)
	{
		mySubIndex = myGameStates[myMainIndex].Size() - 1;
	}
	else
	{
		mySubIndex = -1;
	}

	if (myMainIndex >= 0 && mySubIndex >= 0)
	{
		myGameStates[myMainIndex][mySubIndex]->ResumeState();
	}
}

void StateStack::PushSubGameState(GameState* aSubGameState, bool aShouldPause)
{
	DL_ASSERT_EXP(myGameStates.Size() > 0, "Can't push sub game state, no main game state present.");
	DL_ASSERT_EXP(myMainIndex < 20 && mySubIndex < 20, "Can't add more than 20 states, it's unreasonable!");

	if (aShouldPause == true)
	{
		if (myMainIndex >= 0 && mySubIndex >= 0)
		{
			myGameStates[myMainIndex][mySubIndex]->PauseState();
		}
	}

	myGameStates[myMainIndex].Add(aSubGameState);
	//Prism::ModelLoader::GetInstance()->Pause();
	aSubGameState->InitState(myStateStackProxy, myController, myCursor);
	//Prism::ModelLoader::GetInstance()->UnPause();
	
	SET_RUNTIME(true);

	mySubIndex = myGameStates[myMainIndex].Size() - 1;

	myShouldRender = false;
}

void StateStack::PushMainGameState(GameState* aMainGameState)
{
	if (myMainIndex >= 0 && mySubIndex >= 0)
	{
		myGameStates[myMainIndex][mySubIndex]->PauseState();
	}

	myGameStates.Add(CU::GrowingArray<GameState*, int>(16));
	myMainIndex = myGameStates.Size() - 1;
	PushSubGameState(aMainGameState, false);
}

bool StateStack::UpdateCurrentState(const float& aDeltaTime)
{
	myShouldRender = true;
	myController->Update(aDeltaTime);

	switch (myGameStates[myMainIndex][mySubIndex]->Update(aDeltaTime))
	{
	case eStateStatus::ePopSubState:
		PopSubGameState();
		break;
	case eStateStatus::ePopMainState:
		PopMainGameState();
		break;
	case eStateStatus::eKeepState:
		break;
	}

	DEBUG_PRINT(myMainIndex);
	DEBUG_PRINT(mySubIndex);

	return myGameStates.Size() > 0;
}

void StateStack::RenderCurrentState()
{
	DL_ASSERT_EXP(myGameStates.Size() > 0, "Can't render, no gamestate present.");
	if (myShouldRender == true)
	{
		RenderStateAtIndex(mySubIndex);
	}
}

void StateStack::ResumeState()
{
	myGameStates[myMainIndex][mySubIndex]->ResumeState();
}

void StateStack::OnResizeCurrentState(int aWidth, int aHeight)
{
	myGameStates[myMainIndex][mySubIndex]->OnResize(aWidth, aHeight);
}

void StateStack::OnResize(int aWidth, int aHeight)
{
	for (int i = 0; i < myGameStates.Size(); ++i)
	{
		for (int j = 0; j < myGameStates[i].Size(); ++j)
		{
			myGameStates[i][j]->OnResize(aWidth, aHeight);
		}
	}
}


void StateStack::RenderStateAtIndex(int aIndex)
{
	if (aIndex < 0)
	{
		return;
	}

	if (myGameStates[myMainIndex][aIndex]->IsLetThroughRender() == true)
	{
		RenderStateAtIndex(aIndex - 1);
	}

	myGameStates[myMainIndex][aIndex]->Render();
}

void StateStack::Clear()
{
	while (myGameStates.Size() > 0)
	{
		PopMainGameState();
	}
}