#ifndef PHYSICS_H
#define PHYSICS_H

#include <vector>

namespace Physics
{
	class Vector3
	{
	public:
		double x;
		double y;
		double z;

		Vector3();

		Vector3(double x, double y, double z);
		void set(double x, double y, double z);

		Vector3* operator+=(Vector3* v);
		void cross(Vector3* v);

		Vector3* operator+(Vector3* other);
		Vector3* operator*(double mul);

	};

	class Body
	{
	public:
		Vector3* position;
		Vector3* position_velocity;
		Vector3* position_acceleration;

		Vector3* rotation;
		Vector3* rotation_velocity;
		Vector3* rotation_acceleration;

		float mass;
		float friction;

	public:
		Body()
		{
			position				= new Vector3();
			position_velocity		= new Vector3();
			position_acceleration	= new Vector3();
			
			rotation				= new Vector3();
			rotation_velocity		= new Vector3(120.0f, 360.0f, 60.0f);
			rotation_acceleration	= new Vector3();

			mass		= 0.0f;
			friction	= 0.0f;
		}

		void update(double delta_time);

	};

}
#endif