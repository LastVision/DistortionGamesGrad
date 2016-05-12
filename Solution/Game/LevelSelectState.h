#pragma once
#include "GameState.h"

namespace GUI
{
	class GUIManager;
	class Cursor;
}

class LevelSelectState : public GameState
{
public:
	LevelSelectState();
	~LevelSelectState();

	void InitState(StateStackProxy* aStateStackProxy, GUI::Cursor* aCursor) override;
	void EndState() override;

	const eStateStatus Update(const float& aDeltaTime) override;
	void Render() override;
	void ResumeState() override;
	void OnResize(int aWidth, int aHeight) override;

private:

	GUI::GUIManager* myGUIManager;
};

