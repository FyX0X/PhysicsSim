#pragma once
#include "object.hpp"

#include <SFML/Graphics.hpp>
#include <iostream>


struct Link
{

	VerletObject& object_1;
	VerletObject& object_2;


	float target_distance;
	const float strength = 1.0f;

	const int ID;

	Link() = default;
	Link(VerletObject& _object_1, VerletObject& _object_2, int _id, float _strength = 1.0f) :
		object_1(_object_1),
		object_2(_object_2),
		strength(_strength),
		ID(_id)
	{
		sf::Vector2f axis = object_1.position - object_2.position;
		target_distance = sqrt(axis.x * axis.x + axis.y * axis.y);
		std::cout << "link created\n";
	}

	void apply()
	{
		/*
		VerletObject& obj1 = object_1;
		VerletObject& obj2 = object_2;*/


		const sf::Vector2f axis = object_1.position - object_2.position;
		const float dist = sqrt(axis.x * axis.x + axis.y * axis.y);
		const sf::Vector2f n = axis / dist;
		const float delta = target_distance - dist;
		if (!object_1.is_static) object_1.position += strength * delta * n;
		if (!object_2.is_static) object_2.position -= strength * delta * n;
	}
};