#include "ControllerInput.h"
#include "InputWrapper.h"
namespace CU
{
	ControllerInput::ControllerInput(int aPlayer)
		: myIsConnected(false)
		, myPrevLeftTrigger(0.f)
		, myPrevRightTrigger(0.f)
		, myIsVibrate(false)
	{
		//Set the controller ID (0 -> 3) 
		myControllerID = aPlayer;
	}

	ControllerInput::~ControllerInput()
	{
	}

	XINPUT_STATE ControllerInput::GetState()
	{
		ZeroMemory(&myControllerState, sizeof(XINPUT_STATE));
		XInputGetState(myControllerID, &myControllerState);

		return myControllerState;
	}

	bool ControllerInput::IsConnected()
	{
		return true;

		//Copy the current controllerState to the Previous one, needed to check ButtonUp and ButtonTap.
		//memcpy_s(&myPrevControllerState, sizeof(myPrevControllerState), &myControllerState, sizeof(myControllerState));

		////Gets the state and saves in the stateResult DWORD.
		//DWORD stateResult = XInputGetState(myControllerID, &myControllerState);

		//if (stateResult == ERROR_SUCCESS)
		//	return true;
		//else
		//	return false;
	}

	void ControllerInput::Update(float aDeltaTime)
	{
		//Copy the current controllerState to the Previous one, needed to check ButtonUp and ButtonTap.
		memcpy_s(&myPrevControllerState, sizeof(myPrevControllerState), &myControllerState, sizeof(myControllerState));

		myPrevLeftTrigger = LeftTrigger();
		myPrevRightTrigger = RightTrigger();

		//Gets the state and saves in the stateResult DWORD.
		DWORD stateResult = XInputGetState(myControllerID, &myControllerState);

		if (stateResult == ERROR_SUCCESS)
			myIsConnected = true;
		else
			myIsConnected = false;

		if (myIsVibrate == true)
		{
			if (myVibrationTime > 0.f)
			{
				myVibrationTime -= aDeltaTime;
			}
			else
			{
				myRightMotorValue = 0;
				myLeftMotorValue = 0;
				XINPUT_VIBRATION vibration;
				ZeroMemory(&vibration, sizeof(XINPUT_VIBRATION));
				vibration.wLeftMotorSpeed = myLeftMotorValue;
				vibration.wRightMotorSpeed = myRightMotorValue;
				XInputSetState(myControllerID, &vibration);
				myIsVibrate = false;
			}
		}
	}

	const int ControllerInput::GetControllerID() const //Get the controllerID, required to controll a player (as an example)
	{
		return myControllerID;
	}

	bool ControllerInput::CheckConnection()
	{
		return myIsConnected;
	}

	unsigned int ControllerInput::ConvertInput(const eXboxButton& aButton)
	{
		switch (aButton)
		{
		case eXboxButton::A:
		{
			if (myControllerID == eControllerID::Controller2)
			{
				return DIK_NUMPAD0;
			}
			return DIK_SPACE;

			break;
		}
		case eXboxButton::RTRIGGER:
		{
			if (myControllerID == eControllerID::Controller2)
			{
				return DIK_NUMPAD1;
			}
			return DIK_LSHIFT;

			break;
		}
		case eXboxButton::LTRIGGER:
		{
			if (myControllerID == eControllerID::Controller2)
			{
				return DIK_NUMPAD3;
			}
			return DIK_LALT;

			break;
		}
		case eXboxButton::X:
		{
			if (myControllerID == eControllerID::Controller2)
			{
				return DIK_NUMPAD9;
			}
			return DIK_F3;

			break;
		}
		default:
			break;
		}
		if (aButton != eXboxButton::Y)
		{
			DL_ASSERT("Tried to convert to unknown button.");
		}
		return 0;
	}

	void ControllerInput::Vibrate(unsigned short aLeftVal, unsigned short aRightVal, float someTime)
	{
		myVibrationTime = someTime;
		myRightMotorValue = aRightVal;
		myLeftMotorValue = aLeftVal;

		XINPUT_VIBRATION vibration;
		ZeroMemory(&vibration, sizeof(XINPUT_VIBRATION));
		vibration.wLeftMotorSpeed = myLeftMotorValue;
		vibration.wRightMotorSpeed = myRightMotorValue;
		XInputSetState(myControllerID, &vibration);

		if (myVibrationTime > 0.f)
		{
			myIsVibrate = true;
		}
	}

	bool ControllerInput::ButtonWhileDown(eXboxButton aKey)
	{
		if (CheckConnection() == true)
		{
			if (aKey == eXboxButton::LTRIGGER)
			{
				return (LeftTrigger() > 0.5f) && (myPrevLeftTrigger > 0.5f);
			}
			else if (aKey == eXboxButton::RTRIGGER)
			{
				return (RightTrigger() > 0.5f) && (myPrevRightTrigger> 0.5f);
			}
			else
			{
				return (myControllerState.Gamepad.wButtons & static_cast<int>(aKey)) != 0;
			}
		}
		return CU::InputWrapper::GetInstance()->KeyIsPressed(ConvertInput(aKey));
	}

	bool ControllerInput::ButtonOnUp(eXboxButton aKey)
	{
		if (CheckConnection() == true)
		{
			if (aKey == eXboxButton::LTRIGGER)
			{
				return (LeftTrigger() < 0.5f) && (myPrevLeftTrigger > 0.5f);
			}
			else if (aKey == eXboxButton::RTRIGGER)
			{
				return (RightTrigger() < 0.5f) && (myPrevRightTrigger > 0.5f);
			}
			else
			{
				return (((myControllerState.Gamepad.wButtons & static_cast<int>(aKey)) == 0) && ((myPrevControllerState.Gamepad.wButtons & static_cast<int>(aKey)) != 0));
			}
		}

		return CU::InputWrapper::GetInstance()->KeyUp(ConvertInput(aKey));
	}

	bool ControllerInput::ButtonOnDown(eXboxButton aKey)
	{
		if (CheckConnection() == true)
		{
			if (aKey == eXboxButton::LTRIGGER)
			{
				return (LeftTrigger() > 0.5f) && (myPrevLeftTrigger < 0.5f);
			}
			else if (aKey == eXboxButton::RTRIGGER)
			{
				return (RightTrigger() > 0.5f) && (myPrevRightTrigger < 0.5f);
			}
			else
			{
				return (((myControllerState.Gamepad.wButtons & static_cast<int>(aKey)) != 0) && ((myPrevControllerState.Gamepad.wButtons & static_cast<int>(aKey)) == 0));
			}
		}

		return CU::InputWrapper::GetInstance()->KeyDown(ConvertInput(aKey));
	}

	float ControllerInput::LeftThumbstickX()
	{
		if (CheckConnection() == true)
			return static_cast<float>(myControllerState.Gamepad.sThumbLX) / SHRT_MAX;
		else
		{
			if (myControllerID == eControllerID::Controller1)
			{
				if (CU::InputWrapper::GetInstance()->KeyIsPressed(DIK_A))
				{
					return -1.f;
				}
				else if (CU::InputWrapper::GetInstance()->KeyIsPressed(DIK_D))
				{
					return 1.f;
				}
			}
			else if (myControllerID == eControllerID::Controller2)
			{
				if (CU::InputWrapper::GetInstance()->KeyIsPressed(DIK_LEFTARROW))
				{
					return -1.f;
				}
				else if (CU::InputWrapper::GetInstance()->KeyIsPressed(DIK_RIGHTARROW))
				{
					return 1.f;
				}
			}
		}

		return 0.f;
	}

	float ControllerInput::LeftThumbstickY()
	{
		if (CheckConnection() == true)
			return static_cast<float>(myControllerState.Gamepad.sThumbLY) / SHRT_MAX;
		else
		{
			if (myControllerID == eControllerID::Controller1)
			{
				if (CU::InputWrapper::GetInstance()->KeyIsPressed(DIK_S))
				{
					return -1.f;
				}
				else if (CU::InputWrapper::GetInstance()->KeyIsPressed(DIK_W))
				{
					return 1.f;
				}
			}
			else if (myControllerID == eControllerID::Controller2)
			{
				if (CU::InputWrapper::GetInstance()->KeyIsPressed(DIK_DOWNARROW))
				{
					return -1.f;
				}
				else if (CU::InputWrapper::GetInstance()->KeyIsPressed(DIK_UPARROW))
				{
					return 1.f;
				}
			}
		}

		return 0.f;
	}

	float ControllerInput::RightThumbstickX()
	{
		return static_cast<float>(myControllerState.Gamepad.sThumbRX) / SHRT_MAX;
	}
	float ControllerInput::RightThumbstickY()
	{
		return static_cast<float>(myControllerState.Gamepad.sThumbRY) / SHRT_MAX;
	}

	float ControllerInput::LeftTrigger()
	{
		return static_cast<float>(myControllerState.Gamepad.bLeftTrigger) / 255;
	}

	float ControllerInput::RightTrigger()
	{
		return static_cast<float>(myControllerState.Gamepad.bRightTrigger) / 255;
	}
}
