#include "stdafx.h"

#include <Engine.h>
#include <GameConstants.h>
#include "HighscoreWidget.h"
#include <SQLWrapper.h>
#include <Text.h>

namespace GUI
{
	HighscoreWidget::HighscoreWidget(XMLReader* aReader, tinyxml2::XMLElement* anXMLElement, const int aLevelID)
		: Widget()
		, myHighscores(64)
		, myCurrentLevel(aLevelID)
		, myTextScale(1.f)
	{
		std::string spritePathBackground = "";

		aReader->ForceReadAttribute(aReader->ForceFindFirstChild(anXMLElement, "size"), "x", mySize.x);
		aReader->ForceReadAttribute(aReader->ForceFindFirstChild(anXMLElement, "size"), "y", mySize.y);
		aReader->ForceReadAttribute(aReader->ForceFindFirstChild(anXMLElement, "position"), "x", myPosition.x);
		aReader->ForceReadAttribute(aReader->ForceFindFirstChild(anXMLElement, "position"), "y", myPosition.y);

		aReader->ForceReadAttribute(aReader->ForceFindFirstChild(anXMLElement, "backgroundsprite"), "path", spritePathBackground);
		aReader->ForceReadAttribute(aReader->ForceFindFirstChild(anXMLElement, "coopwidget"), "flag", myIsCoop);

		if (GC::OptionsEnableOffline == false)
		{
			mySQLWrapper.Connect("server.danielcarlsson.net", "Test@d148087", "DGames2016", "danielcarlsson_net_db_1", CLIENT_COMPRESS | CLIENT_FOUND_ROWS | CLIENT_MULTI_STATEMENTS | CLIENT_MULTI_RESULTS);
			myHighscores = mySQLWrapper.RetriveOnlineHighcore(myCurrentLevel);
		}
		float localscore = mySQLWrapper.RetriveLocalHighscore(myCurrentLevel);

		std::stringstream ss;
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

		myLocalBestScore = ss.str();

		myBackgroundSprite = Prism::ModelLoader::GetInstance()->LoadSprite(spritePathBackground, mySize);
		ConstructHighscoreText();
	}

	HighscoreWidget::~HighscoreWidget()
	{
		SAFE_DELETE(myBackgroundSprite);
	}

	void HighscoreWidget::Render(const CU::Vector2<float>& aParentPosition)
	{
		if ((myIsCoop == false && GC::CurrentActivePlayers == 1) || (myIsCoop == true && GC::CurrentActivePlayers == 2))
		{
			Prism::Engine* engine = Prism::Engine::GetInstance();
			myBackgroundSprite->Render(myPosition + aParentPosition);
			engine->PrintText(myLocalBestScoreText, myLocalBestScoreTextPosition, Prism::eTextType::RELEASE_TEXT, myTextScale);
			engine->PrintText(myLocalBestScore, myLocalScorePosition, Prism::eTextType::RELEASE_TEXT, myTextScale);
			engine->PrintText(myLevelText, myTextPosition, Prism::eTextType::RELEASE_TEXT, myTextScale);
			engine->PrintText(myHighscoreTextRank, myTextRankPosition, Prism::eTextType::RELEASE_TEXT, myTextScale);
			engine->PrintText(myHighscoreTextName, myTextNamePosition, Prism::eTextType::RELEASE_TEXT, myTextScale);
			engine->PrintText(myHighscoreTextScore, myTextScorePosition, Prism::eTextType::RELEASE_TEXT, myTextScale);
		}
	}

	void HighscoreWidget::OnResize(const CU::Vector2<float>& aNewSize, const CU::Vector2<float>& anOldSize)
	{
		Widget::OnResize(aNewSize, anOldSize);
		myBackgroundSprite->SetSize(mySize, mySize / 2.f);
		CU::Vector2<float> ratio = aNewSize / anOldSize;
		myTextScale = ratio.y;
		myTextPosition *= ratio;
		myTextRankPosition *= ratio;
		myTextNamePosition *= ratio;
		myTextScorePosition *= ratio;
		myLocalBestScoreTextPosition *= ratio;
		myLocalScorePosition *= ratio;
	}

	void HighscoreWidget::ConstructHighscoreText()
	{
		float textHeight = 36.f;
		CU::Vector2<float> textPosition;
		textPosition = myPosition + myTextPosition;
		textPosition.x -= myBackgroundSprite->GetSize().x / 2.f;
		//textPosition.y += myBackgroundSprite->GetSize().y / 2.f;
		textPosition.x += myTextPosition.x / 2.f;
		textPosition.x += 150;
		myLocalBestScoreTextPosition = textPosition;
		myLocalBestScoreText = "Local Best Score";

		textPosition.x += 80;
		myLocalScorePosition = textPosition;

		myTextRankPosition = myTextPosition;
		textPosition = (myPosition + myTextPosition);
		textPosition.x -= myBackgroundSprite->GetSize().x / 2.f;
		textPosition.y += myBackgroundSprite->GetSize().y / 2.f;
		textPosition.x += myTextPosition.x / 2.f;
		textPosition.x += 150;
		textPosition.y -= textHeight * 0.5f;
		myTextPosition = textPosition;
		myLevelText = "Highscore Level " + std::to_string(myCurrentLevel);

		myHighscoreTextRank = "Rank\n";
		textPosition = (myPosition + myTextRankPosition);
		textPosition.x -= myBackgroundSprite->GetSize().x / 2.f;
		textPosition.y += myBackgroundSprite->GetSize().y / 2.f;
		textPosition -= myTextRankPosition.x;
		textPosition.x += 90;
		textPosition.y -= textHeight * 1.f;
		myTextRankPosition = textPosition;

		myHighscoreTextName = "Name\n";
		textPosition.x += 120;
		myTextNamePosition = textPosition;

		myHighscoreTextScore = "Score\n";
		textPosition.x += 160;
		myTextScorePosition = textPosition;
		myLocalBestScoreTextPosition.y = textPosition.y - textHeight * 11.f;
		if (mySQLWrapper.GetIsOnline() == true)
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
		myLocalScorePosition.y = myLocalBestScoreTextPosition.y - textHeight * 1.f;
	}
}