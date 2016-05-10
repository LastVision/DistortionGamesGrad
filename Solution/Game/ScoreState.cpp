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
}

void ScoreState::EndState()
{
}

const eStateStatus ScoreState::Update(const float& aDeltaTime)
{
	return eStateStatus::ePopMainState;
}

void ScoreState::Render()
{
}

void ScoreState::ResumeState()
{
}

void ScoreState::OnResize(int aWidth, int aHeight)
{
}
