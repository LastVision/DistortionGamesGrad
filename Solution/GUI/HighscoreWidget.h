#pragma once
#include "Widget.h"
#include <Subscriber.h>
#include <SQLWrapper.h>

namespace Prism
{
	class SpriteProxy;
}

namespace GUI
{
	class HighscoreWidget : public Widget, public Subscriber
	{
	public:
		HighscoreWidget(XMLReader* aReader, tinyxml2::XMLElement* anXMLElement, const int aLevelID);
		~HighscoreWidget();

		void Update(float aDelta) override;
		virtual void Render(const CU::Vector2<float>& aParentPosition, float anAlpha) override;
		void OnResize(const CU::Vector2<float>& aNewSize, const CU::Vector2<float>& anOldSize) override;
		void ReduceAlpha(float aReduceAmount);
		void ReceiveMessage(const HighscoreSetLevelMessage& aMessage) override;
		void ReceiveMessage(const ScoreIsLoadingMessage& aMessage) override;

	private:
		void ConstructHighscoreText();
		void SetLevel(const int aLevel);

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

		CU::GrowingArray<Highscore> myHighscores;

		Prism::SpriteProxy* myBackgroundSprite;

		float myTextScale;

		float myParentAlpha;
		float myOwnAlpha;

		Prism::SpriteProxy* myScoreLoading;
		bool myIsLoadingScore;
		float myLoadingAlpha;
		float myLevelLoadingTextTimer;
	};
}