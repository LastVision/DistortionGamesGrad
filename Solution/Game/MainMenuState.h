#pragma once
#include "GameState.h"
#include <Subscriber.h>
#include <Tweener.h>
namespace CU
{
	class InputWrapper;
}

namespace Prism
{
	class Camera;
	class SpriteProxy;
}

namespace GUI
{
	class GUIManager;
	class Cursor;
}

class MainMenuState : public GameState, public Subscriber
{
public:
	MainMenuState();
	~MainMenuState();

	void InitState(StateStackProxy* aStateStackProxy, CU::ControllerInput* aController, GUI::Cursor* aCursor) override;
	void EndState() override;

	const eStateStatus Update(const float& aDeltaTime) override;
	void Render() override;
	void ResumeState() override;
	void PauseState() override;

	void OnResize(int aWidth, int aHeight) override;

	void ReceiveMessage(const OnClickMessage& aMessage) override;

private:
	GUI::GUIManager* myGUIManager;

	Tweener<float> myTweener;
	CU::Vector2<float> myWindowSize;
	float myMenuAlpha;

	CU::Vector2<float> myGUIStartPosition;
	CU::Vector2<float> myGUIEndPosition;
	CU::Vector2<float> myGUIPosition;
	
	bool myHasRunOnce;
};

