#include "scene.h"

Scene::Scene()
{
	this->cameras.push_back(new Camera());
}

Scene::~Scene()
{

}

bool Scene::add(Entity* e)
{
	this->entities.push_back(e);
	return 1;
}

bool Scene::remove()
{
	return 1;
}

void Scene::play()
{

}

void Scene::pause()
{

}

void Scene::update(double delta_time)
{
	for(unsigned int i = 0; i < this->entities.size(); i++)
	{
		this->entities[i]->update(delta_time);
	}

	for(unsigned int i = 0; i < this->cameras.size(); i++)
	{
		this->cameras[i]->update(delta_time);
	}
}
