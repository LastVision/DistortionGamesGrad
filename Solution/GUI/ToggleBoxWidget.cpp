#include "stdafx.h"

#include <AudioInterface.h>
#include <Engine.h>
#include <OnClickMessage.h>
#include <PostMaster.h>
#include "ToggleBoxWidget.h"

namespace GUI
{
	ToggleBoxWidget::ToggleBoxWidget(XMLReader* aReader, tinyxml2::XMLElement* anXMLElement)
		: Widget()
		, myImageActive(nullptr)
		, myImageActiveHover(nullptr)
		, myImageDeactive(nullptr)
		, myImageDeactiveHover(nullptr)
		, myImageCurrent(nullptr)
		, myClickEvent(nullptr)
		, myCanBeClicked(true)
		, myToggledState(true)
		, myId(-1)
		, myColor(1.f, 1.f, 1.f, 1.f)
	{
		std::string spritePathActive = "";
		std::string spritePathDeactive = "";

		std::string spritePathActiveHover = "";
		std::string spritePathDeactiveHover = "";

		aReader->ForceReadAttribute(aReader->ForceFindFirstChild(anXMLElement, "size"), "x", mySize.x);
		aReader->ForceReadAttribute(aReader->ForceFindFirstChild(anXMLElement, "size"), "y", mySize.y);
		aReader->ForceReadAttribute(aReader->ForceFindFirstChild(anXMLElement, "position"), "x", myPosition.x);
		aReader->ForceReadAttribute(aReader->ForceFindFirstChild(anXMLElement, "position"), "y", myPosition.y);

		tinyxml2::XMLElement* textElement = aReader->FindFirstChild(anXMLElement, "text");

		if (textElement != nullptr)
		{
			aReader->ForceReadAttribute(textElement, "value", myText);

			if (CU::ToLower(myText) == "")
			{
				myIsVisible = false;
			}
		}
		aReader->ForceReadAttribute(aReader->ForceFindFirstChild(anXMLElement, "spriteactive"), "path", spritePathActive);
		aReader->ForceReadAttribute(aReader->ForceFindFirstChild(anXMLElement, "spritedeactive"), "path", spritePathDeactive);
		aReader->ForceReadAttribute(aReader->ForceFindFirstChild(anXMLElement, "spriteactivehover"), "path", spritePathActiveHover);
		aReader->ForceReadAttribute(aReader->ForceFindFirstChild(anXMLElement, "spritedeactivehover"), "path", spritePathDeactiveHover);

		myImageActive = Prism::ModelLoader::GetInstance()->LoadSprite(spritePathActive, mySize, mySize / 2.f);
		myImageDeactive = Prism::ModelLoader::GetInstance()->LoadSprite(spritePathDeactive, mySize, mySize / 2.f);
		myImageActiveHover = Prism::ModelLoader::GetInstance()->LoadSprite(spritePathActiveHover, mySize, mySize / 2.f);
		myImageDeactiveHover = Prism::ModelLoader::GetInstance()->LoadSprite(spritePathDeactiveHover, mySize, mySize / 2.f);
		myImageCurrent = myImageActive;

		ReadEvent(aReader, anXMLElement);
	}


	ToggleBoxWidget::~ToggleBoxWidget()
	{
		SAFE_DELETE(myImageActive);
		SAFE_DELETE(myImageActiveHover);
		SAFE_DELETE(myImageDeactive);
		SAFE_DELETE(myImageDeactiveHover);
		SAFE_DELETE(myClickEvent);
		myImageCurrent = nullptr;
	}

	void ToggleBoxWidget::Render(const CU::Vector2<float>& aParentPosition, float anAlpha)
	{
		if (myIsVisible == true)
		{
			myColor.w = anAlpha;
			myImageCurrent->Render(myPosition + aParentPosition, { 1.f, 1.f }, myColor);

			if (myText != "")
			{
				Prism::Engine::GetInstance()->PrintText(myText, { aParentPosition.x + myPosition.x + mySize.x - 10.f
					, aParentPosition.y + myPosition.y - 10.f }, Prism::eTextType::RELEASE_TEXT, 1.f, { 1.f, 1.f, 1.f, anAlpha});
			}
		}
	}

	void ToggleBoxWidget::OnLeftMousePressed(const CU::Vector2<float>&)
	{
		myImageCurrent = myImageDeactive;
		if (myToggledState == true)
		{
			myImageCurrent = myImageActive;
		}
	}

	void ToggleBoxWidget::OnLeftMouseUp()
	{
		myToggledState = !myToggledState;
		Click();
		myImageCurrent = myImageDeactiveHover;
		if (myToggledState == true)
		{
			myImageCurrent = myImageActiveHover;
		}
	}

	void ToggleBoxWidget::OnMouseEnter(bool aShouldSound)
	{
		if (aShouldSound == true)
		{
			Prism::Audio::AudioInterface::GetInstance()->PostEvent("Play_ButtonHover", 0);
		}
		myImageCurrent = myImageDeactiveHover;
		if (myToggledState == true)
		{
			myImageCurrent = myImageActiveHover;
		}
	}

	void ToggleBoxWidget::OnMouseExit()
	{
		myImageCurrent = myImageDeactive;
		if (myToggledState == true)
		{
			myImageCurrent = myImageActive;
		}
	}

	void ToggleBoxWidget::OnResize(const CU::Vector2<float>& aNewSize, const CU::Vector2<float>& anOldSize)
	{
		Widget::OnResize(aNewSize, anOldSize);
		myImageActive->SetSize(mySize, mySize / 2.f);
		myImageDeactive->SetSize(mySize, mySize / 2.f);
	}

	bool ToggleBoxWidget::IsInside(const CU::Vector2<float>& aPosition) const
	{
		return aPosition.x >= myPosition.x - myImageCurrent->GetHotspot().x &&
			aPosition.x <= myPosition.x + mySize.x - myImageCurrent->GetHotspot().x &&
			aPosition.y >= myPosition.y - myImageCurrent->GetHotspot().y &&
			aPosition.y <= myPosition.y + mySize.y - myImageCurrent->GetHotspot().y;
	}

	void ToggleBoxWidget::SetPosition(const CU::Vector2<float>& aPosition, bool aIsHotspot)
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

	void ToggleBoxWidget::ReadEvent(XMLReader* aReader, tinyxml2::XMLElement* anXMLElement)
	{
		std::string clickEvent = "";
		aReader->ForceReadAttribute(aReader->ForceFindFirstChild(anXMLElement, "onclick"), "event", clickEvent);

		if (clickEvent == "toggle_vibration")
		{
			myClickEvent = new OnClickMessage(eOnClickEvent::OPTIONS_TOGGLE_VIBRATION, static_cast<int>(myToggledState));
			myToggledState = GC::OptionsUseViberations;
			myImageCurrent = myImageDeactive;
			if (myToggledState == true)
			{
				myImageCurrent = myImageActive;
			}

		}
		else if (clickEvent == "toggle_shadows")
		{
			myClickEvent = new OnClickMessage(eOnClickEvent::OPTIONS_TOGGLE_SHADOWS, static_cast<int>(myToggledState));
			myToggledState = GC::OptionsUseShadows;
			myImageCurrent = myImageDeactive;
			if (myToggledState == true)
			{
				myImageCurrent = myImageActive;
			}
		}
		else if (clickEvent == "toggle_offline_mode")
		{
			myClickEvent = new OnClickMessage(eOnClickEvent::OPTIONS_TOGGLE_OFFLINE_MODE, static_cast<int>(myToggledState));
			myToggledState = GC::OptionsEnableOffline;
			myImageCurrent = myImageDeactive;
			if (myToggledState == true)
			{
				myImageCurrent = myImageActive;
			}
		}
		else
		{
			std::string message = "[ButtonWidget]: No onclick event named " + clickEvent;
			DL_ASSERT(message);
		}
	}

	void ToggleBoxWidget::SetButtonText(int aButtonId, const std::string& aText, bool& aSuccessOut)
	{
		if (myId == aButtonId)
		{
			DL_ASSERT_EXP(aSuccessOut == false, CU::Concatenate("Button %d, %s duplicate.", aButtonId, aText.c_str()));
			myText = aText;
			aSuccessOut = true;
			myIsVisible = true;
			if (aText == "")
			{
				myIsVisible = false;
			}
		}
	}

	void ToggleBoxWidget::SetToggleState(const bool aFlag)
	{
		myToggledState = aFlag;
		myImageCurrent = myImageDeactive;
		if (myToggledState == true)
		{
			myImageCurrent = myImageActive;
		}
	}

	void ToggleBoxWidget::Click()
	{
		if (myClickEvent != nullptr)
		{
			if (myCanBeClicked == true)
			{
				Prism::Audio::AudioInterface::GetInstance()->PostEvent("Play_ButtonClick", 0);
				if (myClickEvent->myEvent == eOnClickEvent::OPTIONS_TOGGLE_VIBRATION ||
					myClickEvent->myEvent == eOnClickEvent::OPTIONS_TOGGLE_SHADOWS ||
					myClickEvent->myEvent == eOnClickEvent::OPTIONS_TOGGLE_OFFLINE_MODE)
				{
					PostMaster::GetInstance()->SendMessage(OnClickMessage(myClickEvent->myEvent, myToggledState));
				}
				else
				{
					PostMaster::GetInstance()->SendMessage(*myClickEvent);
				}
			}
			else
			{
				// nope sound
			}
		}
	}
}