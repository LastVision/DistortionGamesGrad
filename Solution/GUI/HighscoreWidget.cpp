#include "stdafx.h"

#include <Engine.h>
#include <GameConstants.h>
#include <HighscoreSetLevelMessage.h>
#include "HighscoreWidget.h"
#include <SQLWrapper.h>
#include <Text.h>
#include <PostMaster.h>
#include <ScoreIsLoadingMessage.h>

namespace GUI
{
	HighscoreWidget::HighscoreWidget(XMLReader* aReader, tinyxml2::XMLElement* anXMLElement, const int aLevelID)
		: Widget()
		, myHighscores(64)
		, myCurrentLevel(aLevelID)
		, myTextScale(1.f)
		, myOwnAlpha(0.f)
		, myParentAlpha(0.f)
		, myIsLoadingScore(false)
		, myLoadingAlpha(0.f)
		, myLevelLoadingTextTimer(0.f)
	{
		std::string spritePathBackground = "";

		aReader->ForceReadAttribute(aReader->ForceFindFirstChild(anXMLElement, "size"), "x", mySize.x);
		aReader->ForceReadAttribute(aReader->ForceFindFirstChild(anXMLElement, "size"), "y", mySize.y);
		aReader->ForceReadAttribute(aReader->ForceFindFirstChild(anXMLElement, "position"), "x", myPosition.x);
		aReader->ForceReadAttribute(aReader->ForceFindFirstChild(anXMLElement, "position"), "y", myPosition.y);

		aReader->ForceReadAttribute(aReader->ForceFindFirstChild(anXMLElement, "backgroundsprite"), "path", spritePathBackground);
		aReader->ForceReadAttribute(aReader->ForceFindFirstChild(anXMLElement, "coopwidget"), "flag", myIsCoop);

		myBackgroundSprite = Prism::ModelLoader::GetInstance()->LoadSprite(spritePathBackground, mySize, mySize * 0.5f);
		
		myPosition.y -= (1080.f - Prism::Engine::GetInstance()->GetWindowSize().y) * 0.1f;

		myScoreLoading = Prism::ModelLoader::GetInstance()->LoadSprite("Data/Resource/Texture/Menu/T_loading_screen_rotating_thing.dds", { 256.f, 256.f }, { 128.f, 128.f });

		SetLevel(myCurrentLevel);
		PostMaster::GetInstance()->Subscribe(this, eMessageType::HIGHSCORE_SET_LEVEL | eMessageType::SCORE_IS_LOADING);
	}

	HighscoreWidget::~HighscoreWidget()
	{
		PostMaster::GetInstance()->UnSubscribe(this, 0);
		SAFE_DELETE(myBackgroundSprite);
		SAFE_DELETE(myScoreLoading);
	}

	void HighscoreWidget::Update(float aDelta)
	{
		if (myParentAlpha >= 1.f)
		{
			myOwnAlpha += aDelta;
			if (myOwnAlpha > 1.f)
			{
				myOwnAlpha = 1.f;
			}
		}

		if (myIsLoadingScore == true)
		{
			myLoadingAlpha += aDelta;
			myLevelLoadingTextTimer += aDelta;
			if (myLevelLoadingTextTimer >= 0.2f)
			{
				if (myLevelText.size() < 20)
				{
					myLevelText += ".";
				}
				else
				{
					if (myCurrentLevel < 1000)
					{
						myLevelText = "Highscore loading";
					}
					else
					{
						myLevelText = "Nightmare loading";
					}
				}
				myLevelLoadingTextTimer = 0.f;
			}
		}
		else
		{
			myLoadingAlpha -= aDelta * 2.f;
		}

		myScoreLoading->Rotate(aDelta * 2.5f);


	}

	void HighscoreWidget::Render(const CU::Vector2<float>& aParentPosition, float anAlpha)
	{
		myParentAlpha = anAlpha;

		Prism::Engine* engine = Prism::Engine::GetInstance();
		myBackgroundSprite->Render(myPosition + aParentPosition, { 1.f, 1.f }, { 1.f, 1.f, 1.f, myOwnAlpha });
		engine->PrintText(myLocalBestScoreText, myLocalBestScoreTextPosition, Prism::eTextType::RELEASE_TEXT, myTextScale, { 1.f, 1.f, 1.f, myOwnAlpha });
		engine->PrintText(myLocalBestScore, myLocalScorePosition, Prism::eTextType::RELEASE_TEXT, myTextScale, { 1.f, 1.f, 1.f, myOwnAlpha });
		engine->PrintText(myLevelText, myTextPosition, Prism::eTextType::RELEASE_TEXT, myTextScale, { 1.f, 1.f, 1.f, myOwnAlpha });
		engine->PrintText(myHighscoreTextRank, myTextRankPosition, Prism::eTextType::RELEASE_TEXT, myTextScale, { 1.f, 1.f, 1.f, myOwnAlpha });
		engine->PrintText(myHighscoreTextName, myTextNamePosition, Prism::eTextType::RELEASE_TEXT, myTextScale, { 1.f, 1.f, 1.f, myOwnAlpha });
		engine->PrintText(myHighscoreTextScore, myTextScorePosition, Prism::eTextType::RELEASE_TEXT, myTextScale, { 1.f, 1.f, 1.f, myOwnAlpha });


		myScoreLoading->Render(myPosition + aParentPosition, { 1.f, 1.f }, { 1.f, 1.f, 1.f, myLoadingAlpha });
		
	}

	void HighscoreWidget::OnResize(const CU::Vector2<float>& aNewSize, const CU::Vector2<float>& anOldSize)
	{
		Widget::OnResize(aNewSize, anOldSize);
		myBackgroundSprite->SetSize(mySize, mySize * 0.5f);
		//CU::Vector2<float> ratio = aNewSize / anOldSize;
		//myTextScale = 1.f;
		//myTextPosition = (myTextPosition / anOldSize) * aNewSize;
		//myTextRankPosition = (myTextRankPosition / anOldSize) * aNewSize;
		//myTextNamePosition = (myTextNamePosition / anOldSize) * aNewSize;
		//myTextScorePosition = (myTextScorePosition / anOldSize) * aNewSize;
		//myLocalBestScoreTextPosition = (myLocalBestScoreTextPosition / anOldSize) * aNewSize;
		//myLocalScorePosition = (myLocalScorePosition / anOldSize) * aNewSize;


		ConstructHighscoreText();
		//CU::Vector2<float> ratioPostion = myPosition / anOldWindowSize;
		//myPosition = (myPosition / anOldWindowSize) * aNewWindowSize;
	}

	void HighscoreWidget::ReduceAlpha(float aReduceAmount)
	{
		myOwnAlpha -= aReduceAmount;
	}

	void HighscoreWidget::ReceiveMessage(const HighscoreSetLevelMessage& aMessage)
	{
		SetLevel(aMessage.myLevelID);
	}

	void HighscoreWidget::ReceiveMessage(const ScoreIsLoadingMessage& aMessage)
	{
		if (aMessage.myIsLoading == true)
		{
			myIsLoadingScore = true;
			myLoadingAlpha = 0.f;
			myHighscoreTextName = "";
			myHighscoreTextRank = "";
			myHighscoreTextScore = "";
			if (myCurrentLevel < 1000)
			{
				myLevelText = "Highscore loading";
			}
			else
			{
				myLevelText = "Nightmare loading";
			}
		}
		else
		{
			if (myLoadingAlpha <= 2.f)
			{
				SetLevel(myCurrentLevel);
			}
			else
			{
				myLoadingAlpha = 1.f;
			}
			myIsLoadingScore = false;
		}
	}

	void HighscoreWidget::ConstructHighscoreText()
	{
		myLocalBestScoreText = "Local Best Score";
		if (myCurrentLevel > 1000)
		{
			myLevelText = "Nightmare Level " + std::to_string(myCurrentLevel - 1000);
		}
		else
		{
			myLevelText = "Highscore Level " + std::to_string(myCurrentLevel);
		}
		myHighscoreTextRank = "Rank\n";
		myHighscoreTextName = "Name\n";
		myHighscoreTextScore = "Score\n";
	
		if (GC::HasCheatFiles == true)
		{
			myHighscoreTextRank += "You have changed files,\nyou can't submit or\nview online scores.";
		}
		else if (CU::SQLWrapper::GetInstance()->GetIsOnline() == true)
		{
			for each(const Highscore& score in myHighscores)
			{
				myHighscoreTextRank += std::to_string(score.myRank) + "\n";
				myHighscoreTextName += score.myName + "\n";
				std::stringstream ss;
				if (score.myScore < 1.f)
				{
					ss.precision(2);
				}
				else if (score.myScore < 10.f)
				{
					ss.precision(3);
				}
				else
				{
					ss.precision(4);
				}

				ss << score.myScore << "\n";
				myHighscoreTextScore += ss.str();
			}
		}
		else
		{
			myHighscoreTextRank += "No Internet connection active.";
		}

		float offsetY = 0.f;

		CU::Vector2<float> windowSize(Prism::Engine::GetInstance()->GetWindowSize());

		float offset = (1000.f - windowSize.y) * 0.2f;

		myTextPosition = myPosition;
		myTextPosition.x -= mySize.x * 0.2f;
		myTextPosition.y += mySize.y * 0.5f;
		myTextPosition.y += offset;

		myTextRankPosition = myPosition;
		myTextRankPosition.x -= mySize.x * 0.45f;
		myTextRankPosition.y += mySize.y * 0.45f;
		myTextRankPosition.y += offset;

		myTextNamePosition = myPosition;
		myTextNamePosition.x -= mySize.x * 0.1f;
		myTextNamePosition.y += mySize.y * 0.45f;
		myTextNamePosition.y += offset;

		myTextScorePosition = myPosition;
		myTextScorePosition.x += mySize.x * 0.3f;
		myTextScorePosition.y += mySize.y * 0.45f;
		myTextScorePosition.y += offset;

		myLocalBestScoreTextPosition = myPosition;
		myLocalBestScoreTextPosition.x -= mySize.x * 0.2f;
		myLocalBestScoreTextPosition.y -= mySize.y * 0.35f;

		myLocalScorePosition = myPosition;
		myLocalScorePosition.x -= mySize.x * 0.05f;
		myLocalScorePosition.y -= mySize.y * 0.45f;


		//CU::Vector2<float> windowSize(Prism::Engine::GetInstance()->GetWindowSize());
		//
		////float textHeight = 36.f;
		//
		//float textHeight = windowSize.y * 0.0333f;
		//myTextPosition = CU::Vector2<float>();
		//CU::Vector2<float> textPosition;
		//textPosition = myPosition + myTextPosition;
		//textPosition.x -= myBackgroundSprite->GetSize().x / 2.f;
		////textPosition.y += myBackgroundSprite->GetSize().y / 2.f;
		//textPosition.x += myTextPosition.x / 2.f;
		////textPosition.x += 150;
		//textPosition.x += windowSize.x * 0.078125f;
		//myLocalBestScoreTextPosition = textPosition;
		//myLocalBestScoreText = "Local Best Score";
		//
		////textPosition.x += 80;
		//textPosition.x += windowSize.x * 0.0041666f;
		//myLocalScorePosition = textPosition;
		//
		//myTextRankPosition = myTextPosition;
		//textPosition = (myPosition + myTextPosition);
		//textPosition.x -= myBackgroundSprite->GetSize().x / 2.f;
		//textPosition.y += myBackgroundSprite->GetSize().y / 2.f;
		//textPosition.x += myTextPosition.x / 2.f;
		//textPosition.x += windowSize.x * 0.078125f;
		////textPosition.x += 150;
		////textPosition.y -= textHeight * 0.5f;
		//textPosition.y -= textHeight * ((0.5f / 1080.f) * windowSize.y);
		//myTextPosition = textPosition;
		//if (myCurrentLevel > 1000)
		//{
		//	myLevelText = "Nightmare Level " + std::to_string(myCurrentLevel - 1000);
		//}
		//else
		//{
		//	myLevelText = "Highscore Level " + std::to_string(myCurrentLevel);
		//}
		//
		//myHighscoreTextRank = "Rank\n";
		//textPosition = (myPosition + myTextRankPosition);
		//textPosition.x -= myBackgroundSprite->GetSize().x / 2.f;
		//textPosition.y += myBackgroundSprite->GetSize().y / 2.f;
		//textPosition -= myTextRankPosition.x;
		////textPosition.x += 90;
		//textPosition.x += windowSize.x * 0.046875f;
		//
		////textPosition.y -= textHeight * 1.5f;
		//textPosition.y -= textHeight * (windowSize.y * 0.00138888f);
		//myTextRankPosition = textPosition;
		//
		//myHighscoreTextName = "Name\n";
		////textPosition.x += 120;
		//textPosition.x += windowSize.x * 0.0625f;
		//myTextNamePosition = textPosition;
		//
		//myHighscoreTextScore = "Score\n";
		////textPosition.x += 160;
		//textPosition.x += windowSize.x * 0.08333f;
		//myTextScorePosition = textPosition;
		//myLocalBestScoreTextPosition.y = textPosition.y - textHeight * (windowSize.y * 0.0097222f);
		//myLocalBestScoreTextPosition.y = myPosition.y - myBackgroundSprite->GetSize().y * 0.5f;
		//if (GC::HasCheatFiles == true)
		//{
		//	myHighscoreTextRank += "You have changed files,\nyou can't submit or\nview online scores.";
		//}
		//else if (mySQLWrapper.GetIsOnline() == true)
		//{
		//	for each(const Highscore& score in myHighscores)
		//	{
		//		myHighscoreTextRank += std::to_string(score.myRank) + "\n";
		//		myHighscoreTextName += score.myName + "\n";
		//		std::stringstream ss;
		//		if (score.myScore < 1.f)
		//		{
		//			ss.precision(2);
		//		}
		//		else if (score.myScore < 10.f)
		//		{
		//			ss.precision(3);
		//		}
		//		else
		//		{
		//			ss.precision(4);
		//		}
		//
		//		ss << score.myScore << "\n";
		//		myHighscoreTextScore += ss.str();
		//	}
		//}
		//else
		//{
		//	myHighscoreTextRank += "No Internet connection active.";
		//}
		//myLocalScorePosition.y = myLocalBestScoreTextPosition.y - textHeight * 1.f;
	}

	void HighscoreWidget::SetLevel(const int aLevel)
	{
		myCurrentLevel = aLevel;
		if (GC::OptionsEnableOffline == false && GC::HasCheatFiles == false)
		{
			myHighscores = CU::SQLWrapper::GetInstance()->RetriveOnlineHighcore(myCurrentLevel);
		}
		float localscore = CU::SQLWrapper::GetInstance()->RetriveLocalHighscore(myCurrentLevel);
		std::stringstream ss;
		if (localscore != 0.f)
		{
			if (localscore < 1.f)
			{
				ss.precision(2);
			}
			else if (localscore < 10.f)
			{
				ss.precision(3);
			}
			else
			{
				ss.precision(4);
			}
			ss << localscore;
		}
		else
		{
			ss << "--";
		}

		myLocalBestScore = ss.str();
		ConstructHighscoreText();
	}
}