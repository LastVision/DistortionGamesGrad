#include "ControllerInput.h"
#include "InputWrapper.h"
namespace CU
{
	ControllerInput::ControllerInput(int aPlayer)
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
		if (myControllerID < 0)
			return true;

		//Copy the current controllerState to the Previous one, needed to check ButtonUp and ButtonTap.
		memcpy_s(&myPrevControllerState, sizeof(myPrevControllerState), &myControllerState, sizeof(myControllerState));

		//Gets the state and saves in the stateResult DWORD.
		DWORD stateResult = XInputGetState(myControllerID, &myControllerState);

		if (stateResult == ERROR_SUCCESS)
			return true;
		else
			return false;
	}

	const int ControllerInput::GetControllerID() const //Get the controllerID, required to controll a player (as an example)
	{
		return myControllerID;
	}

	unsigned int ControllerInput::ConvertInput(const eXboxButton& aButton)
	{
		switch (aButton)
		{
		case eXboxButton::A:
			return DIK_SPACE;
			break;




		default:
			break;
		}
		DL_ASSERT("Tried to convert to unknown button.");
		return -1;
	}

	void ControllerInput::Vibrate(unsigned short aLeftVal, unsigned short aRightVal, float someTime)
	{
		XINPUT_VIBRATION vibration;
		ZeroMemory(&vibration, sizeof(XINPUT_VIBRATION));
		vibration.wLeftMotorSpeed = myLeftMotorValue;
		vibration.wRightMotorSpeed = myRightMotorValue;
		XInputSetState(myControllerID, &vibration);

		myVibrationTime = someTime;
		myRightMotorValue = aRightVal;
		myLeftMotorValue = aLeftVal;
	}

	bool ControllerInput::ButtonWhileDown(eXboxButton aKey)
	{
		if (myControllerID >= 0)
			return (myControllerState.Gamepad.wButtons & static_cast<int>(aKey)) != 0;
		else
			return CU::InputWrapper::GetInstance()->KeyIsPressed(ConvertInput(aKey));

		return false;
	}

	bool ControllerInput::ButtonOnUp(eXboxButton aKey)
	{
		if (myControllerID >= 0)
			return (((myControllerState.Gamepad.wButtons & static_cast<int>(aKey)) == 0) && ((myPrevControllerState.Gamepad.wButtons & static_cast<int>(aKey)) != 0));
		else
			return CU::InputWrapper::GetInstance()->KeyUp(ConvertInput(aKey));
		return false;
	}

	bool ControllerInput::ButtonOnDown(eXboxButton aKey)
	{
		if (myControllerID >= 0)
			return (((myControllerState.Gamepad.wButtons & static_cast<int>(aKey)) != 0) && ((myPrevControllerState.Gamepad.wButtons & static_cast<int>(aKey)) == 0));
		else
			return CU::InputWrapper::GetInstance()->KeyDown(ConvertInput(aKey));
		return false;
	}

	float ControllerInput::LeftThumbstickX()
	{
		if (myControllerID >= 0)
			return static_cast<float>(myControllerState.Gamepad.sThumbLX) / SHRT_MAX;
		else
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

		return 0.f;
	}

	float ControllerInput::LeftThumbstickY()
	{
		return static_cast<float>(myControllerState.Gamepad.sThumbLY) / SHRT_MAX;
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
