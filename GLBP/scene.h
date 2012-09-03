#ifndef SCENE_H
#define SCENE_H

#include "entity.h"
#include "camera.h"
#include <vector>

using namespace std;

class Scene
{
public:
	vector<Entity*> entities;
	vector<Camera*> cameras;

public:
	Scene();
	~Scene();

	bool add(Entity* e);
	bool remove();

	void play();
	void pause();
	void update(double delta_time);
	
};

#endif