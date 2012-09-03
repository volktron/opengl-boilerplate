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

	this->scenes.push_back(new Scene());
	this->scenes[0]->add(new Entity());
	this->scenes[0]->add(new Entity());
	this->scenes[0]->add(new Entity());
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
	double delta = 1.0 / g_time->get_fps();

	for(unsigned int i = 0; i < this->scenes.size(); i++)
	{
		this->scenes[i]->update(delta);
	}
}

Timer* Engine::timer()
{
	return g_time;
}