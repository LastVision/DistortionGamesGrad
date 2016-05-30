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

		virtual void Render(const CU::Vector2<float>& aParentPosition) override;
		void OnResize(const CU::Vector2<float>& aNewSize, const CU::Vector2<float>& anOldSize) override;
	private:
		std::string myLevelText;
		std::string myHighscoreText;
		CU::Vector2<float> myTextPosition;

		int myCurrentLevel;

		bool myIsLocal;

		CU::SQLWrapper mySQLWrapper;
		CU::GrowingArray<Highscore> myHighscores;

		Prism::SpriteProxy* myBackgroundSprite;
	};
}