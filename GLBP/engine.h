#ifndef ENGINE_H
#define ENGINE_H

#define ENGINE Engine::getInstance()

class Engine
{
private:
	Engine();
	static Engine* engine;
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

	// Gameplay functions
	
	// Standard functions
	void (*update)(float fps, long time);
};
#endif