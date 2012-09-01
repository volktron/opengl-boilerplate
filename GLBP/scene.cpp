#include "scene.h"

Scene::Scene()
{
	
}

Scene::~Scene()
{

}

bool Scene::add()
{
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

void Scene::update(float delta_time)
{
	for(int i = 0; i < this->entities.size(); i++)
	{
		this->entities[i]->update(delta_time);
	}
}

