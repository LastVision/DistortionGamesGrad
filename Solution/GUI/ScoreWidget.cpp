#include "stdafx.h"
#include <AudioInterface.h>
#include "ScoreWidget.h"
#include <CommonHelper.h>
#include <Engine.h>
#include <OnClickMessage.h>
#include <PostMaster.h>

namespace GUI
{
	ScoreWidget::ScoreWidget(XMLReader* aReader, tinyxml2::XMLElement* anXMLElement)
		: Widget()
	{
		//std::string spritePathNormal = "";
		//std::string spritePathHover = "";
		//std::string spritePathPressed = "";

		aReader->ForceReadAttribute(aReader->ForceFindFirstChild(anXMLElement, "size"), "x", mySize.x);
		aReader->ForceReadAttribute(aReader->ForceFindFirstChild(anXMLElement, "size"), "y", mySize.y);
		aReader->ForceReadAttribute(aReader->ForceFindFirstChild(anXMLElement, "position"), "x", myPosition.x);
		aReader->ForceReadAttribute(aReader->ForceFindFirstChild(anXMLElement, "position"), "y", myPosition.y);

		//tinyxml2::XMLElement* textElement = aReader->FindFirstChild(anXMLElement, "text");

		//if (textElement != nullptr)
		//{
		//	myIsTextButton = true;
		//	aReader->ForceReadAttribute(textElement, "value", myButtonText);

		//	if (CU::ToLower(myButtonText) == "default")
		//	{
		//		myIsVisible = false;
		//	}
		//}
		//aReader->ForceReadAttribute(aReader->ForceFindFirstChild(anXMLElement, "spritenormal"), "path", spritePathNormal);
		//aReader->ForceReadAttribute(aReader->ForceFindFirstChild(anXMLElement, "spritehover"), "path", spritePathHover);
		//aReader->ForceReadAttribute(aReader->ForceFindFirstChild(anXMLElement, "spritepressed"), "path", spritePathPressed);

		myBackground = Prism::ModelLoader::GetInstance()->LoadSprite("Data/Resource/Texture/Menu/ScoreScreen/T_score_window_background.dds", mySize, mySize / 2.f);
		//myImageHover = Prism::ModelLoader::GetInstance()->LoadSprite(spritePathHover, mySize, mySize / 2.f);
		//myImagePressed = Prism::ModelLoader::GetInstance()->LoadSprite(spritePathPressed, mySize, mySize / 2.f);
		//myImageCurrent = myImageNormal;

		//tinyxml2::XMLElement* hoverElement = aReader->FindFirstChild(anXMLElement, "hover");
		//if (hoverElement != nullptr)
		//{
		//	aReader->ForceReadAttribute(hoverElement, "text", myHoverText);
		//}

		//ReadEvent(aReader, anXMLElement);
	}

	ScoreWidget::~ScoreWidget()
	{
		SAFE_DELETE(myBackground);
	}

	void ScoreWidget::Render(const CU::Vector2<float>& aParentPosition)
	{
		if (myIsVisible == true)
		{
			myBackground->Render(myPosition + aParentPosition);
		}
	}

	void ScoreWidget::OnResize(const CU::Vector2<float>& aNewSize, const CU::Vector2<float>& anOldSize)
	{
		Widget::OnResize(aNewSize, anOldSize);
		myBackground->SetSize(mySize, mySize / 2.f);
	}
}