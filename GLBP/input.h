#ifndef INPUT_H
#define INPUT_H

#define INPUT Input::InputManager::get_instance()

#include <Xinput.h>

namespace Input
{
	class XboxController
	{
	public:
		static unsigned long buttons[14];

		XINPUT_STATE	previous_state;
		XINPUT_STATE	current_state;
		XINPUT_STATE	polling_state;
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

	public:
		InputEvent(	unsigned long name,
					int source,
					float time,
					bool down);
	};

	class InputManager
	{
	private:
		static bool instance_flag;
		static InputManager* instance;

	
	public:
		std::vector<InputEvent> events;
		std::vector<InputEvent> triggers;
		std::vector<void ( *)(void)> trigger_functions;
		XboxController* xboxcontrollers[4];

	private:
		InputManager();

	public:
		static InputManager* get_instance();
		void initialize();
		void update();

		bool register_event(InputEvent* e);
		bool register_trigger(InputEvent* e, void (*trigger)(void));
	};

}

#endif