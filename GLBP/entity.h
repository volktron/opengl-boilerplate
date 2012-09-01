#ifndef ENTITY_H
#define ENTITY_H

#include "physics.h"

class Entity
{
public:
	Physics::Body body;
	
private:
	~Entity();

public:
	Entity();

	void update(float delta_time);
};

#endif