#pragma once
#include <iostream>
#include "object.hpp"

class RigidBody : VerletObject
{
public:
	RigidBody(sf::Vector2f pos, float width, float heigth, float mass, float rotation, int _id, bool is_static) :
		VerletObject(pos, 0, -1, is_static),
		width(width),
		height(height),
		rotation(rotation),
		_rigid_body_id(_id)
	{}


	const float width;
	const float height;
	const int _rigid_body_id;
	float rotation;
	float angular_velocity;
};