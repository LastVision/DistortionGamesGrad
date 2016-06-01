#pragma once
#include "GameState.h"
#include <Subscriber.h>

namespace GUI
{
	class GUIManager;
}

struct Score;
struct ScoreInfo;

class FirstTimeFinishLevelState : public GameState, public Subscriber
{
public:
	FirstTimeFinishLevelState(const CU::GrowingArray<const Score*>& someScores, const ScoreInfo& aScoreInfo, const int aLevelID);
	~FirstTimeFinishLevelState();

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

	const CU::GrowingArray<const Score*>& myScores;
	const ScoreInfo& myScoreInfo;
	const int myLevelID;
	bool myRenderFlag;
};

