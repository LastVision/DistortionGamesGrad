#include "ControllerInput.h"
namespace CommonUtilities
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

	bool ControllerInput::ButtonDown(eXboxButton aKey)
	{
		return (myControllerState.Gamepad.wButtons & static_cast<int>(aKey)) != 0;
	}

	bool ControllerInput::ButtonUp(eXboxButton aKey)
	{
		return (((myControllerState.Gamepad.wButtons & static_cast<int>(aKey)) == 0) && ((myPrevControllerState.Gamepad.wButtons & static_cast<int>(aKey)) != 0));
	}

	bool ControllerInput::ButtonTap(eXboxButton aKey)
	{
		return (((myControllerState.Gamepad.wButtons & static_cast<int>(aKey)) != 0) && ((myPrevControllerState.Gamepad.wButtons & static_cast<int>(aKey)) == 0));
	}

	bool ControllerInput::ButtonDown(int aKey)
	{

		return (myControllerState.Gamepad.wButtons & (aKey)) != 0;
	}

	bool ControllerInput::ButtonUp(int aKey)
	{
		return (((myControllerState.Gamepad.wButtons & (aKey)) == 0) && ((myPrevControllerState.Gamepad.wButtons & (aKey)) != 0));
	}

	bool ControllerInput::ButtonTap(int aKey)
	{
		return (((myControllerState.Gamepad.wButtons & (aKey)) != 0) && ((myPrevControllerState.Gamepad.wButtons & (aKey)) == 0));
	}

	float ControllerInput::LeftThumbstickX()
	{
		return static_cast<float>(myControllerState.Gamepad.sThumbLX) / SHRT_MAX;
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
