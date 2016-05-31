#include "stdafx.h"
#include <AudioInterface.h>
#include "ButtonWidget.h"
#include <CommonHelper.h>
#include <Engine.h>
#include <OnClickMessage.h>
#include <PostMaster.h>

namespace GUI
{
	ButtonWidget::ButtonWidget(XMLReader* aReader, tinyxml2::XMLElement* anXMLElement)
		: Widget()
		, myImageNormal(nullptr)
		, myImagePressed(nullptr)
		, myImageHover(nullptr)
		, myImageCurrent(nullptr)
		, myClickEvent(nullptr)
		, myHoverText("")
		, myIsTextButton(false)
		, myCanBeClicked(true)
		, myId(-1)
		, myColor(1.f, 1.f, 1.f, 1.f)
	{
		std::string spritePathNormal = "";
		std::string spritePathHover = "";
		std::string spritePathPressed = "";

		aReader->ForceReadAttribute(aReader->ForceFindFirstChild(anXMLElement, "size"), "x", mySize.x);
		aReader->ForceReadAttribute(aReader->ForceFindFirstChild(anXMLElement, "size"), "y", mySize.y);
		aReader->ForceReadAttribute(aReader->ForceFindFirstChild(anXMLElement, "position"), "x", myPosition.x);
		aReader->ForceReadAttribute(aReader->ForceFindFirstChild(anXMLElement, "position"), "y", myPosition.y);

		tinyxml2::XMLElement* textElement = aReader->FindFirstChild(anXMLElement, "text");

		if (textElement != nullptr)
		{
			myIsTextButton = true;
			aReader->ForceReadAttribute(textElement, "value", myButtonText);

			if (CU::ToLower(myButtonText) == "default")
			{
				myIsVisible = false;
			}
		}
		aReader->ForceReadAttribute(aReader->ForceFindFirstChild(anXMLElement, "spritenormal"), "path", spritePathNormal);
		aReader->ForceReadAttribute(aReader->ForceFindFirstChild(anXMLElement, "spritehover"), "path", spritePathHover);
		aReader->ForceReadAttribute(aReader->ForceFindFirstChild(anXMLElement, "spritepressed"), "path", spritePathPressed);

		myImageNormal = Prism::ModelLoader::GetInstance()->LoadSprite(spritePathNormal, mySize, mySize / 2.f);
		myImageHover = Prism::ModelLoader::GetInstance()->LoadSprite(spritePathHover, mySize, mySize / 2.f);
		myImagePressed = Prism::ModelLoader::GetInstance()->LoadSprite(spritePathPressed, mySize, mySize / 2.f);
		myImageCurrent = myImageNormal;

		tinyxml2::XMLElement* hoverElement = aReader->FindFirstChild(anXMLElement, "hover");
		if (hoverElement != nullptr)
		{
			aReader->ForceReadAttribute(hoverElement, "text", myHoverText);
		}

		ReadEvent(aReader, anXMLElement);
	}

	ButtonWidget::ButtonWidget(const CU::Vector2<float>& aSize, const CU::Vector2<float>& aPosition,
		const std::string& aSpritePath, const std::string& aSpriteHoverPath, const std::string& aSpritePressedPath,
		const std::string& aButtonText, const std::string& aHoverText, const CU::Vector2<float>& aTextOffset)
		: Widget()
		, myImageNormal(nullptr)
		, myImagePressed(nullptr)
		, myImageHover(nullptr)
		, myImageCurrent(nullptr)
		, myClickEvent(nullptr)
		, myHoverText(aHoverText)
		, myIsTextButton(false)
		, myCanBeClicked(true)
		, myId(-1)
		, myColor(1.f, 1.f, 1.f, 1.f)
		, myTextOffset(aTextOffset)
	{
		mySize = aSize;
		myPosition = aPosition;

		myImageNormal = Prism::ModelLoader::GetInstance()->LoadSprite(aSpritePath, mySize, mySize / 2.f);
		myImagePressed = Prism::ModelLoader::GetInstance()->LoadSprite(aSpritePressedPath, mySize, mySize / 2.f);
		myImageHover = Prism::ModelLoader::GetInstance()->LoadSprite(aSpriteHoverPath, mySize, mySize / 2.f);
		myImageCurrent = myImageNormal;

		if (aButtonText == "default")
		{
			myIsVisible = false;
		}
		else if (aButtonText != "")
		{
			myButtonText = aButtonText;
			myIsTextButton = true;
		}
	}

	ButtonWidget::~ButtonWidget()
	{
		SAFE_DELETE(myImageNormal);
		SAFE_DELETE(myImagePressed);
		SAFE_DELETE(myImageHover);
		SAFE_DELETE(myClickEvent);
		myImageCurrent = nullptr;
	}

	void ButtonWidget::Render(const CU::Vector2<float>& aParentPosition)
	{
		if (myIsVisible == true)
		{
			myImageCurrent->Render(myPosition + aParentPosition, { 1.f, 1.f }, myColor);

			if (myIsTextButton == true)
			{
				Prism::Engine::GetInstance()->PrintText(myButtonText, aParentPosition + myPosition + myTextOffset, Prism::eTextType::RELEASE_TEXT);
			}

			if (myImageCurrent == myImageHover && myHoverText != "")
			{
				CU::Vector2<float> hoverPosition = { myPosition.x + mySize.x / 2.f, myPosition.y };
				hoverPosition += aParentPosition;

				Prism::Engine::GetInstance()->PrintText(myHoverText, hoverPosition, Prism::eTextType::RELEASE_TEXT);
			}
		}
	}

	void ButtonWidget::OnLeftMousePressed(const CU::Vector2<float>&)
	{
		myImageCurrent = myImagePressed;
	}

	void ButtonWidget::OnLeftMouseUp()
	{
		Click();
		myImageCurrent = myImageHover;
	}

	void ButtonWidget::OnMouseEnter()
	{
		Prism::Audio::AudioInterface::GetInstance()->PostEvent("Play_ButtonHover", 0);
		myImageCurrent = myImageHover;
	}

	void ButtonWidget::OnMouseExit()
	{
		myImageCurrent = myImageNormal;
	}

	void ButtonWidget::OnResize(const CU::Vector2<float>& aNewSize, const CU::Vector2<float>& anOldSize)
	{
		Widget::OnResize(aNewSize, anOldSize);
		myImageNormal->SetSize(mySize, mySize / 2.f);
		myImagePressed->SetSize(mySize, mySize / 2.f);
		myImageHover->SetSize(mySize, mySize / 2.f);
	}

	bool ButtonWidget::IsInside(const CU::Vector2<float>& aPosition) const
	{
		return aPosition.x >= myPosition.x - myImageCurrent->GetHotspot().x &&
			aPosition.x <= myPosition.x + mySize.x - myImageCurrent->GetHotspot().x &&
			aPosition.y >= myPosition.y - myImageCurrent->GetHotspot().y &&
			aPosition.y <= myPosition.y + mySize.y - myImageCurrent->GetHotspot().y;
	}

	void ButtonWidget::SetPosition(const CU::Vector2<float>& aPosition, bool aIsHotspot)
	{
		if (aIsHotspot == true)
		{
			myPosition = { aPosition.x + myImageCurrent->GetHotspot().x, aPosition.y - myImageCurrent->GetHotspot().y };
		}
		else
		{
			myPosition = aPosition;
		}
	}

	void ButtonWidget::ReadEvent(XMLReader* aReader, tinyxml2::XMLElement* anXMLElement)
	{
		std::string clickEvent = "";
		aReader->ForceReadAttribute(aReader->ForceFindFirstChild(anXMLElement, "onclick"), "event", clickEvent);

		if (clickEvent == "start_level")
		{
			bool isNightmare = false;
			aReader->ForceReadAttribute(aReader->ForceFindFirstChild(anXMLElement, "onclick"), "id", myId);
			aReader->ReadAttribute(aReader->FindFirstChild(anXMLElement, "onclick"), "isNightmare", isNightmare);
			myButtonText = std::to_string(myId + 1);
			myIsTextButton = true;
			myClickEvent = new OnClickMessage(eOnClickEvent::START_LEVEL, myId, isNightmare);
		}
		else if (clickEvent == "restart_level")
		{
			myClickEvent = new OnClickMessage(eOnClickEvent::RESTART_LEVEL);
		}
		else if (clickEvent == "next_level")
		{
			myClickEvent = new OnClickMessage(eOnClickEvent::NEXT_LEVEL);
		}
		else if (clickEvent == "level_select")
		{
			bool isNightmare = false;
			aReader->ReadAttribute(aReader->FindFirstChild(anXMLElement, "onclick"), "isNightmare", isNightmare);
			myClickEvent = new OnClickMessage(eOnClickEvent::LEVEL_SELECT, myId, isNightmare);
		}
		else if (clickEvent == "credits")
		{
			myClickEvent = new OnClickMessage(eOnClickEvent::CREDITS);
		}
		else if (clickEvent == "options")
		{
			myClickEvent = new OnClickMessage(eOnClickEvent::OPTIONS);
		}
		else if (clickEvent == "hat")
		{
			myClickEvent = new OnClickMessage(eOnClickEvent::HAT);
		}
		else if (clickEvent == "hat_selection")
		{
			myClickEvent = new OnClickMessage(eOnClickEvent::HAT_SELECTION);
		}
		else if (clickEvent == "hat_unlock")
		{
			myClickEvent = new OnClickMessage(eOnClickEvent::HAT_UNLOCK);
		}
		else if (clickEvent == "hat_quit")
		{
			myClickEvent = new OnClickMessage(eOnClickEvent::HAT_QUIT);
		}
		else if (clickEvent == "spin")
		{
			myClickEvent = new OnClickMessage(eOnClickEvent::SPIN);
		}
		else if (clickEvent == "IncreaseVolume")
		{
			myClickEvent = new OnClickMessage(eOnClickEvent::OPTIONS_INCREASE_VOLUME);
		}
		else if (clickEvent == "LowerVolume")
		{
			myClickEvent = new OnClickMessage(eOnClickEvent::OPTIONS_DECREASE_VOLUME);
		}
		else if (clickEvent == "IncreaseMusic")
		{
			myClickEvent = new OnClickMessage(eOnClickEvent::OPTIONS_INCREASE_MUSIC);
		}
		else if (clickEvent == "LowerMusic")
		{
			myClickEvent = new OnClickMessage(eOnClickEvent::OPTIONS_DECREASE_MUSIC);
		}
		else if (clickEvent == "IncreaseVoice")
		{
			myClickEvent = new OnClickMessage(eOnClickEvent::OPTIONS_INCREASE_VOICE);
		}
		else if (clickEvent == "LowerVoice")
		{
			myClickEvent = new OnClickMessage(eOnClickEvent::OPTIONS_DECREASE_VOICE);
		}
		else if (clickEvent == "quit")
		{
			myClickEvent = new OnClickMessage(eOnClickEvent::GAME_QUIT);
		}
		else if (clickEvent == "returnToGame")
		{
			myClickEvent = new OnClickMessage(eOnClickEvent::RETURN_TO_GAME);
		}
		else
		{
			std::string message = "[ButtonWidget]: No onclick event named " + clickEvent;
			DL_ASSERT(message);
		}
	}

	void ButtonWidget::SetButtonText(int aButtonId, const std::string& aText, bool& aSuccessOut)
	{
		if (myId == aButtonId)
		{
			DL_ASSERT_EXP(aSuccessOut == false, CU::Concatenate("Button %d, %s duplicate.", aButtonId, aText.c_str()));
			myButtonText = aText;
			aSuccessOut = true;
			myIsVisible = true;
			if (aText == "")
			{
				myIsVisible = false;
			}
		}
	}

	void ButtonWidget::SetActive(const bool aActiveFlag)
	{
		if (aActiveFlag == true)
		{
			myColor = { 1.f, 1.f, 1.f, 1.f };
			myCanBeClicked = true;
		}
		else
		{
			myColor = { 0.5f, 0.5f, 0.5f, 1.f };
			myCanBeClicked = false;
		}
	}

	void ButtonWidget::Click()
	{
		if (myClickEvent != nullptr)
		{
			if (myCanBeClicked == true)
			{
				Prism::Audio::AudioInterface::GetInstance()->PostEvent("Play_ButtonClick", 0);
				PostMaster::GetInstance()->SendMessage(*myClickEvent);
			}
			else
			{
				// nope sound
			}
		}
	}
}