#include "stdafx.h"
#include <AudioInterface.h>
#include <BarWidget.h>
#include <CommonHelper.h>
#include <Engine.h>
#include <ModelLoader.h>
#include <OnClickMessage.h>
#include <PostMaster.h>
#include <Score.h>
#include "ScoreInfo.h"
#include "ScoreWidget.h"
#include <SpriteProxy.h>
#include "StarWidget.h"

ScoreWidget::ScoreWidget(const Score* aScore, const ScoreInfo& aScoreInfo)
	: GUI::Widget()
	, myScore(aScore)
	, myScoreInfo(aScoreInfo)
	, myBar(nullptr)
	, myMaxValue(1.f)
	, myCurrentValue(0.f)
	, myStars(3)
{
	mySize = CU::Vector2<float>(512.f, 512.f);
	myPosition = CU::Vector2<float>(694.f, 540.f);

	myBar = new GUI::BarWidget(myMaxValue, myCurrentValue, CU::Vector2<float>(512.f, 64.f), CU::Vector4<float>(1.f, 1.f, 1.f, 1.f));

	myGoalValue = CalculateGoalValue();

	for (int i = 0; i < 3; ++i)
	{
		myStars.Add(new StarWidget(true, i));
	}

	myBackground = Prism::ModelLoader::GetInstance()->LoadSprite("Data/Resource/Texture/Menu/ScoreScreen/T_score_window_background.dds", mySize, mySize / 2.f);
}

ScoreWidget::~ScoreWidget()
{
	SAFE_DELETE(myBackground);
	SAFE_DELETE(myBar);
	myStars.DeleteAll();
}

void ScoreWidget::Update(float aDeltaTime)
{
	myCurrentValue += aDeltaTime * 0.5f;
	myCurrentValue = fminf(myCurrentValue, myGoalValue);

	myBar->Update(aDeltaTime);

	for each(StarWidget* star in myStars)
	{
		star->Update(aDeltaTime);
	}
}

void ScoreWidget::Render(const CU::Vector2<float>& aParentPosition)
{
	if (myIsVisible == true)
	{
		myBackground->Render(myPosition + aParentPosition);
		myBar->Render(myPosition + aParentPosition);
		
		CU::Vector2<float> starOffset(138.f, -100.f);

		for (int i = 0; i < myStars.Size(); ++i)
		{
			myStars[i]->Render(myPosition + aParentPosition + CU::Vector2<float>(i * starOffset.x, starOffset.y) - CU::Vector2<float>(starOffset.x, 0.f));
		}
	}
}

void ScoreWidget::OnResize(const CU::Vector2<float>& aNewSize, const CU::Vector2<float>& anOldSize)
{
	GUI::Widget::OnResize(aNewSize, anOldSize);
	myBackground->SetSize(mySize, mySize / 2.f);
	myBar->OnResize(aNewSize, anOldSize);
	for each(StarWidget* star in myStars)
	{
		star->OnResize(aNewSize, anOldSize);
	}
}

float ScoreWidget::CalculateGoalValue()
{
	if (myScore->myTime < myScoreInfo.myShortTime)
	{
		return 1.f;
	}
	if (myScore->myTime > myScoreInfo.myLongTime)
	{
		return 0.f;
	}
	if (myScore->myTime < myScoreInfo.myMediumTime)
	{
		float score = myScore->myTime - myScoreInfo.myShortTime;

		score /= (myScoreInfo.myMediumTime - myScoreInfo.myShortTime);

		score = 1.f - score;
		score *= 0.5f;
		score += 0.5f;

		return score;
	}

	float score = myScore->myTime - myScoreInfo.myMediumTime;

	score /= (myScoreInfo.myLongTime - myScoreInfo.myMediumTime);

	score = 1.f - score;
	score *= 0.5f;

	return score;
}