#include <Windows.h>
#include <vector>
#include "input.h"

namespace Input
{
	bool InputManager::instance_flag = false;
	InputManager* InputManager::instance = 0;

	XboxController::XboxController(DWORD id)
	{
		this->id = id;
	}

	void XboxController::update()
	{
		this->previous_state = this->state;
		this->state_result = XInputGetState(this->id, &this->state);
		
	}

	void XboxController::vibrate()
	{
		
	}

	InputManager::InputManager()
	{

	}

	InputManager* InputManager::get_instance()
	{
		if(InputManager::instance_flag)
			return InputManager::instance;
		InputManager::instance_flag = true;
		InputManager::instance = new InputManager();
		return InputManager::instance;
	}

	void InputManager::initialize()
	{
		for(int i = 0; i < 4; i++)
			this->xboxcontrollers[i] = new XboxController(i);
	}

	void InputManager::update()
	{
		for(int i = 0; i < 4; i++)
			this->xboxcontrollers[i]->update();
	}
}