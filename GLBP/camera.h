#ifndef CAMERA_H
#define CAMERA_H

#include "physics.h"

class Camera
{
public:
	Physics::Body* body;

public:
	Camera();
	~Camera();

	void update(double delta_time);
	void translate();
};

#endif CAMERA_H