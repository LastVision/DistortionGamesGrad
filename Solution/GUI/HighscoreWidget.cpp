#include "stdafx.h"
#include <Engine.h>
#include "HighscoreWidget.h"
#include <SQLWrapper.h>
#include <Text.h>

namespace GUI
{
	HighscoreWidget::HighscoreWidget(XMLReader* aReader, tinyxml2::XMLElement* anXMLElement, const int aLevelID)
		: Widget()
		, myHighscores(64)
		, myCurrentLevel(aLevelID)
	{
		std::string spritePathBackground = "";

		aReader->ForceReadAttribute(aReader->ForceFindFirstChild(anXMLElement, "size"), "x", mySize.x);
		aReader->ForceReadAttribute(aReader->ForceFindFirstChild(anXMLElement, "size"), "y", mySize.y);
		aReader->ForceReadAttribute(aReader->ForceFindFirstChild(anXMLElement, "position"), "x", myPosition.x);
		aReader->ForceReadAttribute(aReader->ForceFindFirstChild(anXMLElement, "position"), "y", myPosition.y);

		aReader->ForceReadAttribute(aReader->ForceFindFirstChild(anXMLElement, "backgroundsprite"), "path", spritePathBackground);

		aReader->ForceReadAttribute(aReader->ForceFindFirstChild(anXMLElement, "localhighscore"), "flag", myIsLocal);

		if (myIsLocal == false)
		{
			mySQLWrapper.Connect("mysql334.loopia.se", "Test@d148087", "DGames2016", "danielcarlsson_net_db_1", CLIENT_COMPRESS | CLIENT_FOUND_ROWS | CLIENT_MULTI_STATEMENTS | CLIENT_MULTI_RESULTS);
			myHighscores = mySQLWrapper.RetriveOnlineHighcore(myCurrentLevel);
		}
		else
		{
			myHighscores = mySQLWrapper.RetriveLocalHighcore(myCurrentLevel);
		}

		myBackgroundSprite = Prism::ModelLoader::GetInstance()->LoadSprite(spritePathBackground, mySize, mySize / 2.f);
		ConstructHighscoreText();
	}

	HighscoreWidget::~HighscoreWidget()
	{
		SAFE_DELETE(myBackgroundSprite);
	}

	void HighscoreWidget::Render(const CU::Vector2<float>& aParentPosition)
	{
		Prism::Engine* engine = Prism::Engine::GetInstance();
		myBackgroundSprite->Render(myPosition + aParentPosition);
		engine->PrintText(myLevelText, myTextPosition, Prism::eTextType::RELEASE_TEXT);
		engine->PrintText(myHighscoreTextRank, myTextRankPosition, Prism::eTextType::RELEASE_TEXT);
		engine->PrintText(myHighscoreTextName, myTextNamePosition, Prism::eTextType::RELEASE_TEXT);
		engine->PrintText(myHighscoreTextScore, myTextScorePosition, Prism::eTextType::RELEASE_TEXT);
	}

	void HighscoreWidget::OnResize(const CU::Vector2<float>& aNewSize, const CU::Vector2<float>& anOldSize)
	{
		Widget::OnResize(aNewSize, anOldSize);
		myBackgroundSprite->SetSize(mySize, mySize / 2.f);
		CU::Vector2<float> ratio = aNewSize / anOldSize;
		myTextPosition *= ratio;
		myTextRankPosition *= ratio;
		myTextNamePosition *= ratio;
		myTextScorePosition *= ratio;
	}

	void HighscoreWidget::ConstructHighscoreText()
	{
		CU::Vector2<float> textPosition = (myPosition + myTextPosition);
		textPosition.x -= myBackgroundSprite->GetSize().x / 2.f;
		textPosition.y += myBackgroundSprite->GetSize().y / 2.f;
		textPosition.x += myTextPosition.x / 2.f;
		textPosition.y -= 20;
		myTextPosition = textPosition;
		myLevelText = "Level " + std::to_string(myCurrentLevel);

		myHighscoreTextRank = "Rank\n";
		textPosition = (myPosition + myTextPosition);
		textPosition.x -= myBackgroundSprite->GetSize().x / 2.f;
		textPosition.y += myBackgroundSprite->GetSize().y / 2.f;
		textPosition -= myTextPosition.x;
		textPosition.y -= 100;
		myTextRankPosition = textPosition;

		myHighscoreTextName = "Name\n";
		textPosition.x += 120;
		myTextNamePosition = textPosition;

		myHighscoreTextScore = "Score\n";
		textPosition.x += 200;
		myTextScorePosition = textPosition;

		for each(const Highscore& score in myHighscores)
		{
			myHighscoreTextRank += std::to_string(score.myRank) + "\n";
			myHighscoreTextName += score.myName + "\n";
			myHighscoreTextScore += std::to_string(score.myScore) + "\n";
		}
	}
}