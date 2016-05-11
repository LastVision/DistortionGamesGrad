#include "stdafx.h"
#include "ScoreState.h"


ScoreState::ScoreState()
{
}


ScoreState::~ScoreState()
{
}

void ScoreState::InitState(StateStackProxy* aStateStackProxy, GUI::Cursor* aCursor)
{
	myStateStack = aStateStackProxy;
	myCursor = aCursor;

	myStateStatus = eStateStatus::ePopMainState;
	myIsLetThrough = true;
	myIsActiveState = true;
}

void ScoreState::EndState()
{
}

const eStateStatus ScoreState::Update(const float& aDeltaTime)
{
	aDeltaTime;
	return myStateStatus;
}

void ScoreState::Render()
{
}

void ScoreState::ResumeState()
{
}

void ScoreState::OnResize(int, int)
{
}
