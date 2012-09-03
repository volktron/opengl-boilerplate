#ifndef ENTITY_H
#define ENTITY_H

#include "physics.h"

class Entity
{
public:
	int id;
	Physics::Body* body;
	
private:
	~Entity();

public:
	Entity();
	Entity(	float pos_x, float pos_y, float pos_z,
			float rot_x, float rot_y, float rot_z,
			float mass, float friction				);

	void update(double delta_time);
};

#endif