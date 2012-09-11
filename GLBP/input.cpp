#include <Windows.h>
#include <vector>
#include <iostream>
#include "input.h"
#include "engine.h"

namespace Input
{
	bool InputManager::instance_flag = false;
	InputManager* InputManager::instance = 0;

	unsigned long XboxController::buttons[14] = {
		XINPUT_GAMEPAD_DPAD_UP          ,
		XINPUT_GAMEPAD_DPAD_DOWN        ,
		XINPUT_GAMEPAD_DPAD_LEFT        ,
		XINPUT_GAMEPAD_DPAD_RIGHT       ,
		XINPUT_GAMEPAD_START            ,
		XINPUT_GAMEPAD_BACK             ,
		XINPUT_GAMEPAD_LEFT_THUMB       ,
		XINPUT_GAMEPAD_RIGHT_THUMB      ,
		XINPUT_GAMEPAD_LEFT_SHOULDER    ,
		XINPUT_GAMEPAD_RIGHT_SHOULDER   ,
		XINPUT_GAMEPAD_A                ,
		XINPUT_GAMEPAD_B                ,
		XINPUT_GAMEPAD_X                ,
		XINPUT_GAMEPAD_Y                

	};

	InputEvent::InputEvent(	unsigned long name,
							int source,
							float time,
							bool down)
	{
		this->name		= name;
		this->source	= source;
		this->time		= time;
		this->down		= down;
	}

	XboxController::XboxController(DWORD id)
	{
		this->id = id;
	}

	void XboxController::update()
	{
		this->state_result = XInputGetState(this->id, &this->polling_state);
		if (this->state_result == ERROR_SUCCESS)
		{
			this->previous_state = this->current_state;
			this->current_state = this->polling_state;
		
			for(int i = 0; i < 14; i++)
			{
				if ((this->current_state.Gamepad.wButtons & XboxController::buttons[i]) &&
				   !(this->previous_state.Gamepad.wButtons & XboxController::buttons[i]))
				{
					INPUT->register_event(
						new InputEvent(XboxController::buttons[i],
						this->id,
						ENGINE->timer()->get_time_local(),
						true));
					std::cout << XboxController::buttons[i] << " button pressed\n";
				}
				else if (!(this->current_state.Gamepad.wButtons & XboxController::buttons[i]) &&
						  (this->previous_state.Gamepad.wButtons & XboxController::buttons[i]))
				{
					INPUT->register_event(
						new InputEvent(XboxController::buttons[i],
						this->id,
						ENGINE->timer()->get_time_local(),
						false));
					std::cout << XboxController::buttons[i] << " button released" << INPUT->events.size() << "\n";
				}			
			}

		}
		
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

	bool InputManager::register_event(InputEvent* e)
	{
		// Triggers
		for(int i = 0; i < INPUT->triggers.size(); i++)
				if(	e->source	== INPUT->triggers[i].source &&
					e->name		== INPUT->triggers[i].name)
					(INPUT->trigger_functions[i])();

		// State change
		if(e->down)
			this->events.push_back(*e);
		else
			for(int i = 0; i < INPUT->events.size(); i++)
				if(	e->source	== INPUT->events[i].source &&
					e->name		== INPUT->events[i].name)
					INPUT->events.erase(INPUT->events.begin() + i);

		return true;
	}

	bool InputManager::register_trigger(InputEvent* e, void (*trigger)(void))
	{
		this->triggers.push_back(*e);
		this->trigger_functions.push_back(trigger);
		return true;
	}
}