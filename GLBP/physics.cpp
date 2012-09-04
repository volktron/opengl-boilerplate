#include "physics.h"

namespace Physics
{
	Vector3* scratch_vector = new Vector3();
	
	Vector3::Vector3()
	{
		this->x = 0;
		this->y = 0;
		this->z = 0;
	}

	Vector3::Vector3(double x, double y, double z)
	{
		this->x = x;
		this->y = y;
		this->z = z;
	}

	void Vector3::set(double x, double y, double z)
	{
		this->x = x;
		this->y = y;
		this->z = z;
	}

	Vector3* Vector3::operator+=(Vector3* v)
	{
		this->x += v->x;
		this->y += v->y;
		this->z += v->z;

		return this;
	}

	void Vector3::cross(Vector3* v)
	{
		this->x = (this->y + v->z) - (this->z + v->y);
		this->y = (this->z + v->x) - (this->x + v->z);
		this->z = (this->x + v->y) - (this->y + v->x);
	}

	Vector3* Vector3::operator+(Vector3* other)
	{
		scratch_vector->x = this->x + other->x;
		scratch_vector->y = this->y + other->y;
		scratch_vector->z = this->z + other->z;

		return scratch_vector;
	}

	Vector3* Vector3::operator*(double mul)
	{
		scratch_vector->x = this->x * mul;
		scratch_vector->y = this->y * mul;
		scratch_vector->z = this->z * mul;

		return scratch_vector;
	}
	
	void Body::update(double delta_time)
	{
		*this->position_velocity	+= *this->position_acceleration	* delta_time;
		*this->position				+= *this->position_velocity		* delta_time;

		*this->rotation_velocity	+= *this->rotation_acceleration	* delta_time;
		*this->rotation				+= *this->rotation_velocity		* delta_time;
	}
}