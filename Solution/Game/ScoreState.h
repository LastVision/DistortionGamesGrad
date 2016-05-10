#pragma once
#include "GameState.h"

class ScoreState : public GameState
{
public:
	ScoreState();
	~ScoreState();

	void InitState(StateStackProxy* aStateStackProxy, GUI::Cursor* aCursor) override;
	void EndState() override;
	const eStateStatus Update(const float& aDeltaTime) override;
	void Render() override;
	void ResumeState() override;
	void OnResize(int aWidth, int aHeight) override;

};

