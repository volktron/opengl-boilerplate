#include "camera.h"

Camera::Camera()
{
	this->body = new Physics::Body();
	this->body->position->z = 5;
	this->body->position_velocity->x = 0.01;
}

Camera::~Camera()
{
	delete body;
}

void Camera::update(double delta_time)
{
	this->body->update(delta_time);
}
