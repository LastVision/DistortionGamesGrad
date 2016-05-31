#include "stdafx.h"

#include <AudioInterface.h>
#include "ButtonWidget.h"
#include <Engine.h>
#include <OnClickMessage.h>
#include <Text.h>
#include <PostMaster.h>
#include "VolumeWidget.h"

namespace GUI
{
	VolumeWidget::VolumeWidget(XMLReader* aReader, tinyxml2::XMLElement* anXMLElement)
		: myTextScale(1.f)
	{
		std::string spriteDecreasePathNormal = "";
		std::string spriteDecreasePathHover = "";
		std::string spriteDecreasePathPressed = "";

		std::string spriteIncreasePathNormal = "";
		std::string spriteIncreasePathHover = "";
		std::string spriteIncreasePathPressed = "";

		std::string buttonEventType = "";

		CU::Vector2<float> buttonSize;
		CU::Vector2<float> buttonOffset;

		aReader->ForceReadAttribute(aReader->ForceFindFirstChild(anXMLElement, "size"), "x", mySize.x);
		aReader->ForceReadAttribute(aReader->ForceFindFirstChild(anXMLElement, "size"), "y", mySize.y);
		aReader->ForceReadAttribute(aReader->ForceFindFirstChild(anXMLElement, "position"), "x", myPosition.x);
		aReader->ForceReadAttribute(aReader->ForceFindFirstChild(anXMLElement, "position"), "y", myPosition.y);

		aReader->ForceReadAttribute(aReader->ForceFindFirstChild(anXMLElement, "buttonsize"), "x", buttonSize.x);
		aReader->ForceReadAttribute(aReader->ForceFindFirstChild(anXMLElement, "buttonsize"), "y", buttonSize.y);
		aReader->ForceReadAttribute(aReader->ForceFindFirstChild(anXMLElement, "buttonoffset"), "x", buttonOffset.x);
		aReader->ForceReadAttribute(aReader->ForceFindFirstChild(anXMLElement, "buttonoffset"), "y", buttonOffset.y);

		aReader->ForceReadAttribute(aReader->ForceFindFirstChild(anXMLElement, "spritedecreasenormal"), "path", spriteDecreasePathNormal);
		aReader->ForceReadAttribute(aReader->ForceFindFirstChild(anXMLElement, "spritedecreasehover"), "path", spriteDecreasePathHover);
		aReader->ForceReadAttribute(aReader->ForceFindFirstChild(anXMLElement, "spritedecreasepressed"), "path", spriteDecreasePathPressed);

		aReader->ForceReadAttribute(aReader->ForceFindFirstChild(anXMLElement, "spriteincreasenormal"), "path", spriteIncreasePathNormal);
		aReader->ForceReadAttribute(aReader->ForceFindFirstChild(anXMLElement, "spriteincreasehover"), "path", spriteIncreasePathHover);
		aReader->ForceReadAttribute(aReader->ForceFindFirstChild(anXMLElement, "spriteincreasepressed"), "path", spriteIncreasePathPressed);

		aReader->ForceReadAttribute(aReader->ForceFindFirstChild(anXMLElement, "buttonevent"), "type", buttonEventType);
		aReader->ForceReadAttribute(aReader->ForceFindFirstChild(anXMLElement, "text"), "value", myTextValue);
		
		CU::Vector2<float> position({ (buttonSize.x * 1 + buttonOffset.x * 1) + myPosition.x, myPosition.y });
		myDecreaseButton = new ButtonWidget(buttonSize, position, spriteDecreasePathNormal, spriteDecreasePathHover, spriteDecreasePathPressed, "");
		position.x += buttonSize.x + buttonOffset.x + 128;
		myTextPosition.x = buttonSize.x + buttonOffset.x + 96;
		position.x += 128;
		myIncreaseButton = new ButtonWidget(buttonSize, position, spriteIncreasePathNormal, spriteIncreasePathHover, spriteIncreasePathPressed, "");

		if (buttonEventType == "volume")
		{
			myDecreaseButton->SetEvent(new OnClickMessage(eOnClickEvent::OPTIONS_DECREASE_VOLUME));
			myIncreaseButton->SetEvent(new OnClickMessage(eOnClickEvent::OPTIONS_INCREASE_VOLUME));
			myType = eVolumeType::VOLUME;
			int currentVolume = Prism::Audio::AudioInterface::GetInstance()->GetSFXVolume();
			if (currentVolume != myValue)
			{
				myValue = currentVolume;
				myText = myTextValue + ": " + std::to_string(myValue);
			}
		}
		else if (buttonEventType == "music")
		{
			myDecreaseButton->SetEvent(new OnClickMessage(eOnClickEvent::OPTIONS_DECREASE_MUSIC));
			myIncreaseButton->SetEvent(new OnClickMessage(eOnClickEvent::OPTIONS_INCREASE_MUSIC));
			myType = eVolumeType::MUSIC;
			int currentVolume = Prism::Audio::AudioInterface::GetInstance()->GetMusicVolume();
			if (currentVolume != myValue)
			{
				myValue = currentVolume;
				myText = myTextValue + ": " + std::to_string(myValue);
			}
		}
		else if (buttonEventType == "voice")
		{
			myDecreaseButton->SetEvent(new OnClickMessage(eOnClickEvent::OPTIONS_DECREASE_VOICE));
			myIncreaseButton->SetEvent(new OnClickMessage(eOnClickEvent::OPTIONS_INCREASE_VOICE));
			myType = eVolumeType::VOICE;
			int currentVolume = Prism::Audio::AudioInterface::GetInstance()->GetVoiceVolume();
			if (currentVolume != myValue)
			{
				myValue = currentVolume;
				myText = myTextValue + ": " + std::to_string(myValue);
			}
		}

		PostMaster::GetInstance()->Subscribe(this, eMessageType::ON_CLICK);
	}

	VolumeWidget::~VolumeWidget()
	{
		//SAFE_DELETE(myDecreaseButton);
		//SAFE_DELETE(myIncreaseButton);
		PostMaster::GetInstance()->UnSubscribe(this, 0);
	}

	void VolumeWidget::Render(const CU::Vector2<float>& aParentPosition)
	{
		Prism::Engine::GetInstance()->PrintText(myText, { aParentPosition.x + myPosition.x + myTextPosition.x, aParentPosition.y + myPosition.y + myTextPosition.y }, Prism::eTextType::RELEASE_TEXT, myTextScale);
		/*myDecreaseButton->Render(myPosition + aParentPosition);
		myIncreaseButton->Render(myPosition + aParentPosition);*/
	}

	void VolumeWidget::OnResize(const CU::Vector2<float>& aNewSize, const CU::Vector2<float>& anOldSize)
	{
		CU::Vector2<float> ratio = aNewSize / anOldSize;
		myPosition *= ratio;
		myTextPosition *= ratio;
	}

	void VolumeWidget::ReceiveMessage(const OnClickMessage& aMessage)
	{
		switch (aMessage.myEvent)
		{
		case eOnClickEvent::OPTIONS_INCREASE_VOLUME:
			if (myType == eVolumeType::VOLUME)
			{
				Prism::Audio::AudioInterface::GetInstance()->PostEvent("IncreaseVolume", 0);
				int currentVolume = Prism::Audio::AudioInterface::GetInstance()->GetSFXVolume();
				if (currentVolume != myValue)
				{
					myValue = currentVolume;
					myText = myTextValue + ": " + std::to_string(myValue);
				}
			}
			break;
		case eOnClickEvent::OPTIONS_DECREASE_VOLUME:
			if (myType == eVolumeType::VOLUME)
			{
				Prism::Audio::AudioInterface::GetInstance()->PostEvent("LowerVolume", 0);
				int currentVolume = Prism::Audio::AudioInterface::GetInstance()->GetSFXVolume();
				if (currentVolume != myValue)
				{
					myValue = currentVolume;
					myText = myTextValue + ": " + std::to_string(myValue);
				}
			}
			break;
		case eOnClickEvent::OPTIONS_INCREASE_VOICE:
			if (myType == eVolumeType::VOICE)
			{
				Prism::Audio::AudioInterface::GetInstance()->PostEvent("IncreaseVoice", 0);
				int currentVolume = Prism::Audio::AudioInterface::GetInstance()->GetVoiceVolume();
				if (currentVolume != myValue)
				{
					myValue = currentVolume;
					myText = myTextValue + ": " + std::to_string(myValue);
				}
			}
			break;
		case eOnClickEvent::OPTIONS_DECREASE_VOICE:
			if (myType == eVolumeType::VOICE)
			{
				Prism::Audio::AudioInterface::GetInstance()->PostEvent("LowerVoice", 0);
				int currentVolume = Prism::Audio::AudioInterface::GetInstance()->GetVoiceVolume();
				if (currentVolume != myValue)
				{
					myValue = currentVolume;
					myText = myTextValue + ": " + std::to_string(myValue);
				}
			}
			break;
		case eOnClickEvent::OPTIONS_INCREASE_MUSIC:
			if (myType == eVolumeType::MUSIC)
			{
				Prism::Audio::AudioInterface::GetInstance()->PostEvent("IncreaseMusic", 0);
				int currentVolume = Prism::Audio::AudioInterface::GetInstance()->GetMusicVolume();
				if (currentVolume != myValue)
				{
					myValue = currentVolume;
					myText = myTextValue + ": " + std::to_string(myValue);
				}
			}
			break;
		case eOnClickEvent::OPTIONS_DECREASE_MUSIC:
			if (myType == eVolumeType::MUSIC)
			{
				Prism::Audio::AudioInterface::GetInstance()->PostEvent("LowerMusic", 0);
				int currentVolume = Prism::Audio::AudioInterface::GetInstance()->GetMusicVolume();
				if (currentVolume != myValue)
				{
					myValue = currentVolume;
					myText = myTextValue + ": " + std::to_string(myValue);
				}
			}
			break;
		default:
			break;
		}
	}
}