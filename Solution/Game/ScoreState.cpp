#include "stdafx.h"

#include "InputWrapper.h"
#include "Score.h"
#include "ScoreInfo.h"
#include "ScoreState.h"


ScoreState::ScoreState(const CU::GrowingArray<const Score*>& someScores, const ScoreInfo& aScoreInfo)
	: myScores(someScores)
	, myScoreInfo(aScoreInfo)
{
}


ScoreState::~ScoreState()
{
}

void ScoreState::InitState(StateStackProxy* aStateStackProxy, GUI::Cursor* aCursor)
{
	myStateStack = aStateStackProxy;
	myCursor = aCursor;

	myStateStatus = eStateStatus::eKeepState;
	myIsLetThrough = true;
	myIsActiveState = true;
}

void ScoreState::EndState()
{
}

const eStateStatus ScoreState::Update(const float& aDeltaTime)
{
	if (CU::InputWrapper::GetInstance()->KeyDown(DIK_RETURN))
	{
		return eStateStatus::ePopMainState;
	}

	aDeltaTime;
	return myStateStatus;
}

void ScoreState::Render()
{
	for each (const Score* score in myScores)
	{
		DEBUG_PRINT(score->myDeathCount);
		DEBUG_PRINT(score->myTime);
	}
}

void ScoreState::ResumeState()
{
}

void ScoreState::PauseState()
{
}

void ScoreState::OnResize(int, int)
{
}
