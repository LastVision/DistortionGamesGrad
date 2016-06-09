#pragma once
#include <SQLWrapper.h>
#include "Widget.h"

namespace Prism
{
	class SpriteProxy;
}

namespace GUI
{
	class HighscoreWidget : public Widget
	{
	public:
		HighscoreWidget(XMLReader* aReader, tinyxml2::XMLElement* anXMLElement, const int aLevelID);
		~HighscoreWidget();

		void Update(float aDelta) override;
		virtual void Render(const CU::Vector2<float>& aParentPosition, float anAlpha) override;
		void OnResize(const CU::Vector2<float>& aNewSize, const CU::Vector2<float>& anOldSize) override;
		void ReduceAlpha(float aReduceAmount);

	private:
		void ConstructHighscoreText();

		std::string myLocalBestScoreText;
		std::string myLocalBestScore;

		std::string myLevelText;
		std::string myHighscoreTextRank;
		std::string myHighscoreTextName;
		std::string myHighscoreTextScore;

		CU::Vector2<float> myLocalBestScoreTextPosition;
		CU::Vector2<float> myLocalScorePosition;

		CU::Vector2<float> myTextPosition;
		CU::Vector2<float> myTextRankPosition;
		CU::Vector2<float> myTextNamePosition;
		CU::Vector2<float> myTextScorePosition;

		int myCurrentLevel;

		bool myIsCoop;

		CU::SQLWrapper mySQLWrapper;
		CU::GrowingArray<Highscore> myHighscores;

		Prism::SpriteProxy* myBackgroundSprite;

		float myTextScale;

		float myParentAlpha;
		float myOwnAlpha;
	};
}