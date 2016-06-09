#pragma once
#include "GameState.h"
#include <Matrix.h>
#include <GrowingArray.h>
#include <Subscriber.h>
#include <Tweener.h>

class Level;
class LevelFactory;
class MessageState;


namespace CU
{
	class InputWrapper;
}


namespace Prism
{
	class Camera;
	class Sprite;
	class TextProxy;
	class SpriteProxy;
}

namespace GUI
{
	class GUIManager;
	class Cursor;
}

class InGameState : public GameState, public Subscriber
{
public:
	InGameState(int aLevelID);
	~InGameState();

	void InitState(StateStackProxy* aStateStackProxy, CU::ControllerInput* aController, GUI::Cursor* aCursor) override;
	void EndState() override;

	const eStateStatus Update(const float& aDeltaTime) override;
	void Render() override;
	void ResumeState() override;
	void PauseState() override;

	void ReceiveMessage(const FinishLevelMessage& aMessage) override;

	void OnResize(int aWidth, int aHeight) override;

	void ReceiveMessage(const OnClickMessage& aMessage) override;
	void ReceiveMessage(const ReturnToMenuMessage& aMessage) override;
	void ReceiveMessage(const ReturnToMainMenuMessage& aMessage) override;

private:
	GUI::GUIManager* myGUIManager;
	LevelFactory* myLevelFactory;
	Level* myLevel;

	Prism::Camera* myCamera;
	CU::Matrix44f myCameraOrientation;

	int myLevelToLoad;
	int myNextLevel;

	Prism::TextProxy* myText;
	Prism::SpriteProxy* myLoadingScreen;
	Prism::SpriteProxy* myRotatingThing;
	Prism::SpriteProxy* myBlackSprite;
	float myRotationSpeed;
	float myRotatingScale;
	float myScaleTimer;
	Tweener<float> myTweener;

	bool myHasStartedMusicBetweenLevels;
	int myLastLevel;

	enum class eState
	{
		FIRST_FRAME,
		START_LOADING,
		LOADING,
		LOADING_FINISHED,
		START_GAME,
	};
	eState myState;
	float myFirstFrameTimer;

	bool myIsBetweenLevels;
};