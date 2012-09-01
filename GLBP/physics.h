#ifndef PHYSICS_H
#define PHYSICS_H

namespace Physics
{
	class Vector3
	{
	public:
		float x;
		float y;
		float z;

		Vector3()
		{
			this->x = 0;
			this->y = 0;
			this->z = 0;
		}

		Vector3(float x, float y, float z)
		{
			this->x = x;
			this->y = y;
			this->z = z;
		}

		Vector3* operator+=(Vector3* v)
		{
			this->x += v->x;
			this->y += v->y;
			this->z += v->z;
			return this;
		}

		void cross(Vector3* v)
		{
			this->x = (this->y + v->z) - (this->z + v->y);
			this->y = (this->z + v->x) - (this->x + v->z);
			this->z = (this->x + v->y) - (this->y + v->x);
		}

		Vector3* operator+(Vector3* other)
		{
			return new Vector3(
				this->x + other->x,
				this->y + other->y,
				this->z + other->z);
		}

		Vector3* operator*(float mul)
		{
			return new Vector3(
				this->x * mul,
				this->y * mul,
				this->z * mul);
		}

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
			rotation_velocity		= new Vector3();
			rotation_acceleration	= new Vector3();

			mass		= 0.0f;
			friction	= 0.0f;
		}

		void update(float delta_time)
		{
			*this->position_velocity	+= *this->position_acceleration	* delta_time;
			*this->position				+= *this->position_velocity		* delta_time;

			*this->rotation_velocity	+= *this->rotation_acceleration	* delta_time;
			*this->rotation				+= *this->rotation_velocity		* delta_time;
		}


	};

}
#endif