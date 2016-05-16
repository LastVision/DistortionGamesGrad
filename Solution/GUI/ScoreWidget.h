#pragma once
#include "Widget.h"

namespace Prism
{
	class SpriteProxy;
}

namespace tinyxml2
{
	class XMLElement;
}

class XMLReader;

namespace GUI
{
	class ScoreWidget : public Widget
	{
	public:
		ScoreWidget(XMLReader* aReader, tinyxml2::XMLElement* anXMLElement);
		~ScoreWidget();

		virtual void Render(const CU::Vector2<float>& aParentPosition) override;

		void OnResize(const CU::Vector2<float>& aNewSize, const CU::Vector2<float>& anOldSize) override;

	protected:
		Prism::SpriteProxy* myBackground;
	};
}