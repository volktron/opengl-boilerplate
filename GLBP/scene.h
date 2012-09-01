#ifndef SCENE_H
#define SCENE_H

#include "entity.h"
#include <vector>

using namespace std;

class Scene
{
private:
	vector<Entity*> entities;

public:
	Scene();
	~Scene();

	bool add();
	bool remove();

	void play();
	void pause();
	void update(float delta_time);
	
};

#endif