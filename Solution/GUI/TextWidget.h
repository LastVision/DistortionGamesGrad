#pragma once
#include "Widget.h"
namespace GUI
{
	class TextWidget : public Widget
	{
	public:
		TextWidget(XMLReader* aReader, tinyxml2::XMLElement* anXMLElement);
		~TextWidget();

		void Render(const CU::Vector2<float>& aParentPosition) override;

		void OnResize(const CU::Vector2<float>& aNewSize, const CU::Vector2<float>& anOldSize) override;

	private:
		std::string myText;
	};
}
