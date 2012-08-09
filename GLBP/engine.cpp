#include "engine.h"

bool Engine::instanceflag = false;
Engine* Engine::engine = 0;
Timer* Engine::g_time = 0;

Engine::Engine()
{
	for (int i = 0; i < 256; i++)
	{
		keys[i] = false;
	}
}

Engine::~Engine()
{
	delete g_time;
}

void Engine::initialize()
{
	time_rate = 0.0f;
	day_length = 60;
	
	g_time = new Timer();

	done = false;
}

// The Engine Class is a Singleton
Engine* Engine::getInstance()
{
	if (instanceflag)
		return engine;

	instanceflag = true;
	engine = new Engine();

	return engine;
}

void Engine::update()
{
	g_time->update();
}

Timer* Engine::timer()
{
	return g_time;
}