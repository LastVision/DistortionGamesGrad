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
		myLevelText = "Level ";

	}

	HighscoreWidget::~HighscoreWidget()
	{
	}

	void HighscoreWidget::Render(const CU::Vector2<float>& aParentPosition)
	{
		Prism::Engine* engine = Prism::Engine::GetInstance();
		myBackgroundSprite->Render(myPosition + aParentPosition);
		CU::Vector2<float> textPosition = aParentPosition + myPosition + myTextPosition;
		textPosition.y -= myTextPosition.y / 2.f;
		engine->PrintText(myLevelText + std::to_string(myCurrentLevel), textPosition, Prism::eTextType::RELEASE_TEXT);

		myLevelText = "Rank";
		textPosition = aParentPosition + myPosition + myTextPosition;
		textPosition.y -= 20;
		engine->PrintText(myLevelText, textPosition, Prism::eTextType::RELEASE_TEXT);
		myLevelText = "Name";
		textPosition.x += 160;
		engine->PrintText(myLevelText, textPosition, Prism::eTextType::RELEASE_TEXT);
		myLevelText = "Score";
		textPosition.x += 250;
		engine->PrintText(myLevelText, textPosition, Prism::eTextType::RELEASE_TEXT);

		int rows = 0;
		for each(const Highscore& score in myHighscores)
		{
			myHighscoreText = score.myRank;
			textPosition = aParentPosition + myPosition + myTextPosition;
			textPosition.y += 20 * rows;
			engine->PrintText(myHighscoreText, textPosition, Prism::eTextType::RELEASE_TEXT);
			myHighscoreText = score.myName;
			textPosition.x += 160;
			engine->PrintText(myHighscoreText, textPosition, Prism::eTextType::RELEASE_TEXT);
			myHighscoreText = score.myScore;
			textPosition.x += 250;
			engine->PrintText(myHighscoreText, textPosition, Prism::eTextType::RELEASE_TEXT);
			rows++;
		}
	}

	void HighscoreWidget::OnResize(const CU::Vector2<float>& aNewSize, const CU::Vector2<float>& anOldSize)
	{
		Widget::OnResize(aNewSize, anOldSize);
		myBackgroundSprite->SetSize(mySize, mySize / 2.f);
		CU::Vector2<float> ratio = aNewSize / anOldSize;
		myTextPosition *= ratio;
	}
}