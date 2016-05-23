#pragma once
#include "StateEnums.h"
#include "StateStackProxy.h"

namespace CU
{
	class InputWrapper;
	class ControllerInput;
}

namespace GUI
{
	class Cursor;
	class GUIManager;
}

class StateStackProxy;

class GameState
{
public:

	virtual ~GameState();

	virtual void InitState(StateStackProxy* aStateStackProxy, CU::ControllerInput* aController, GUI::Cursor* aCursor) = 0;
	virtual void EndState() = 0;
	
	virtual const eStateStatus Update(const float& aDeltaTime) = 0;
	virtual void Render() = 0;
	virtual void ResumeState() = 0;
	virtual void PauseState() = 0;
	virtual void OnResize(int aWidth, int aHeight) = 0;

	const bool& IsLetThroughRender() const;

	void HandleControllerInMenu(CU::ControllerInput* aController, GUI::GUIManager* aManager);

protected:

	CU::ControllerInput* myController;
	GUI::Cursor* myCursor;
	StateStackProxy* myStateStack;
	eStateStatus myStateStatus;
	bool myIsLetThrough;
	bool myIsActiveState;

	bool myControllerUpIsDown;
	bool myControllerDownIsDown;
};

inline GameState::~GameState()
{

}

inline const bool& GameState::IsLetThroughRender() const
{
	return myIsLetThrough;
}