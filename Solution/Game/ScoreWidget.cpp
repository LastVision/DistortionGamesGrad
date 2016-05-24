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
#include <StarWidget.h>
#include "ScoreWidget.h"
#include <SpriteProxy.h>
#include <sstream>

#include <InputWrapper.h>

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

	if (myScore->myReachedGoal == true)
	{
		myStars.Add(new StarWidget(myGoalValue > 0.f, 0));
		myStars.Add(new StarWidget(myGoalValue > 0.5f, 1));
		myStars.Add(new StarWidget(myGoalValue >= 1.f, 2));
	}
	else
	{
		myStars.Add(new StarWidget(false, 0));
		myStars.Add(new StarWidget(false, 1));
		myStars.Add(new StarWidget(false, 2));
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
	if (myIsVisible == true && myScore->myActive == true)
	{
		myBackground->Render(myPosition + aParentPosition);
		myBar->Render(myPosition + aParentPosition);
		
		CU::Vector2<float> starOffset(138.f, -200.f);

		for (int i = 0; i < myStars.Size(); ++i)
		{
			myStars[i]->Render(myPosition + aParentPosition + CU::Vector2<float>(i * starOffset.x, starOffset.y) - CU::Vector2<float>(starOffset.x, 0.f));
		}

		std::stringstream ss;


		if (myScore->myTime < 1.f)
		{
			ss.precision(2);
		}
		else if (myScore->myTime < 10.f)
		{
			ss.precision(3);
		}
		else
		{
			ss.precision(4);
		}

		ss << "Your time: ";
		if (myScore->myReachedGoal == true)
		{
			ss << myScore->myTime;
		}
		else
		{
			ss << "--";
		}
		ss << " sec" << std::endl;

		ss << "You died " << myScore->myDeathCount << " time";
		if (myScore->myDeathCount != 1)
		{
			ss << "s";
		}

		Prism::Engine::GetInstance()->PrintText(ss.str(), aParentPosition + myPosition + CU::Vector2<float>(-100.f, 120.f), Prism::eTextType::RELEASE_TEXT);

		Prism::Engine::GetInstance()->PrintText(CU::Concatenate("%i sec          %i sec          %i sec"
			, int(myScoreInfo.myLongTime), int(myScoreInfo.myMediumTime), int(myScoreInfo.myShortTime))
			, aParentPosition + myPosition + CU::Vector2<float>(-243.f, -50.f), Prism::eTextType::RELEASE_TEXT);
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
	if (myScore->myReachedGoal == false)
	{
		return 0.f;
	}
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