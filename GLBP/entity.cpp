#include "entity.h"

Entity::Entity()
{
	this->body = new Physics::Body();
}

Entity::Entity(	float pos_x, float pos_y, float pos_z,
				float rot_x, float rot_y, float rot_z,
				float mass, float friction				)
{
	this->body = new Physics::Body();

	this->body->position->x = pos_x;
	this->body->position->y = pos_y;
	this->body->position->z = pos_z;

	this->body->rotation->x = rot_x;
	this->body->rotation->y = rot_y;
	this->body->rotation->z = rot_z;

	this->body->mass = mass;
	this->body->friction = friction;
}

void Entity::update(double delta_time)
{
	this->body->update(delta_time);
}