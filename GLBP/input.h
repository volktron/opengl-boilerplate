#ifndef INPUT_H
#define INPUT_H

#define INPUT Input::InputManager::get_instance()

#include <Xinput.h>

namespace Input
{
	class XboxController
	{
	public:
		XINPUT_STATE	previous_state;
		XINPUT_STATE	state;
		DWORD			state_result;

		bool connected;
		DWORD id;

	public:
		XboxController(DWORD id);
		void vibrate();
		void update();
	};

	class InputEvent
	{
	public:
		unsigned long name;
		int source;
		float time;
		bool down;
	};

	class InputManager
	{
	private:
		static bool instance_flag;
		static InputManager* instance;

	
	public:
		std::vector<InputEvent> events;
		XboxController* xboxcontrollers[4];

	private:
		InputManager();

	public:
		static InputManager* get_instance();
		void initialize();
		void update();

		bool register_event();
	};

}

#endif