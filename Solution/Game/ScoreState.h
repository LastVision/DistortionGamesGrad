#pragma once
#include "GameState.h"
#include <Subscriber.h>

class ScoreWidget;


struct Score;
struct ScoreInfo;

class ScoreState : public GameState, public Subscriber
{
public:
	ScoreState(const CU::GrowingArray<const Score*>& someScores, const ScoreInfo& aScoreInfo, const int aLevelID);
	~ScoreState();

	void InitState(StateStackProxy* aStateStackProxy, CU::ControllerInput* aController, GUI::Cursor* aCursor) override;
	void EndState() override;
	const eStateStatus Update(const float& aDeltaTime) override;
	void Render() override;
	void ResumeState() override;
	void PauseState() override;
	void OnResize(int aWidth, int aHeight) override;
	void ReceiveMessage(const OnClickMessage& aMessage) override;

private:
	void operator=(const ScoreState&) = delete;
	GUI::GUIManager* myGUIManager;

	void SaveScoreToFile(const int aLevelID);
	void SaveUnlockedLevels(const int aLevelID);

	const CU::GrowingArray<const Score*>& myScores;
	const ScoreInfo& myScoreInfo;
	CU::GrowingArray<ScoreWidget*> myScoreWidgets;
};

