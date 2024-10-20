#pragma once
#include "object.hpp"
#include <SFML/Graphics.hpp>
#include <iostream>


struct Spring
{

	VerletObject& object_1;
	VerletObject& object_2;


	float target_distance;
	const float spring_constant;
	const float damping_constant;

	const int ID;

	Spring() = default;
	Spring(VerletObject& _object_1, VerletObject& _object_2, int _id, float _spring_constant = 1.0f, float _damping = 1.0f) :
		object_1(_object_1),
		object_2(_object_2),
		spring_constant(_spring_constant),
		damping_constant(_damping),
		ID(_id)
	{
		//std::cout << _spring_constant << "init\n";
		sf::Vector2f axis = object_1.position - object_2.position;
		target_distance = sqrt(axis.x * axis.x + axis.y * axis.y);
		//std::cout << spring_constant << "test\n";
		
		//std::cout << "link created\n";
	}

	void apply() 
	{
		const sf::Vector2f axis = object_1.position - object_2.position;

		// add spring force
		const float dist = sqrt(axis.x * axis.x + axis.y * axis.y);
		const sf::Vector2f n = axis / dist;
		const float delta = target_distance - dist;
		if (!object_1.is_static) object_1.addForce(+spring_constant * delta * n);
		if (!object_2.is_static) object_2.addForce(-spring_constant * delta * n);

		// add damping force
		sf::Vector2f relative_motion = object_1.getVelocity() - object_2.getVelocity();
		float projected_speed = relative_motion.x * n.x + relative_motion.y * n.y;
		if (!object_1.is_static) object_1.addForce(+damping_constant * projected_speed * n);
		if (!object_2.is_static) object_2.addForce(-damping_constant * projected_speed * n);

	}

};