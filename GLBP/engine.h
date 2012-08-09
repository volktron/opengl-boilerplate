#ifndef ENGINE_H
#define ENGINE_H

#define ENGINE Engine::getInstance()

#include "timer.h"

class Engine
{
private:
	Engine();
	static Engine* engine;
	static Timer* g_time;		// Global application time
	static bool instanceflag;
	
public:
	// Gameplay logic variables
	float time_rate;	// The rate of passage of time
	int day_length;

	// Done?
	bool done;

	// Keyboard
	bool	keys[256];	// Array Used For The Keyboard Routine

public:
	~Engine();
	static Engine* getInstance();
	void initialize();
	Timer* timer();

	// Gameplay functions
	
	// Standard functions
	void update();
};

#endif