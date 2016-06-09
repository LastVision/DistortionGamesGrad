#include "stdafx.h"

#include <CommonHelper.h>
#include <Engine.h>
#include <Text.h>
#include "TextWidget.h"

namespace GUI
{
	TextWidget::TextWidget(XMLReader* aReader, tinyxml2::XMLElement* anXMLElement)
	{
		aReader->ForceReadAttribute(aReader->ForceFindFirstChild(anXMLElement, "position"), "x", myPosition.x);
		aReader->ForceReadAttribute(aReader->ForceFindFirstChild(anXMLElement, "position"), "y", myPosition.y);

		aReader->ForceReadAttribute(aReader->ForceFindFirstChild(anXMLElement, "text"), "value", myText);

		std::string reformatedText = myText;
		myText = CU::ReplaceAllInString(reformatedText, "%username%", CU::GetUsername());
	}

	TextWidget::~TextWidget()
	{
	}

	void TextWidget::Render(const CU::Vector2<float>& aParentPosition, float anAlpha)
	{
		Prism::Engine::GetInstance()->PrintText(myText, aParentPosition + myPosition, Prism::eTextType::RELEASE_TEXT, 1.f, { 1.f, 1.f, 1.f, anAlpha });
	}

	void TextWidget::OnResize(const CU::Vector2<float>& aNewSize, const CU::Vector2<float>& anOldSize)
	{
		CU::Vector2<float> ratio = aNewSize / anOldSize;
		myPosition *= ratio;
	}
}