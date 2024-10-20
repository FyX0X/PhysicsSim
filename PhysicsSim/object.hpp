#pragma once


struct VerletObject
{
	sf::Vector2f position;
	sf::Vector2f position_old;
	sf::Vector2f acceleration;

	const int ID = 0;
	const bool is_static;
	const float radius;
	const float mass;

	sf::Color color = sf::Color::White;

	VerletObject() = default;
	VerletObject(sf::Vector2f _position, float _radius, int _ID, bool _is_static = false, sf::Vector2f vel = {0.0f, 0.0f}) :
		position(_position),
		position_old(_position + vel),
		acceleration({0.0f, 0.0f}),
		radius(_radius),
		mass(_radius),
		ID(_ID),
		is_static(_is_static)
	{}

	

	void updatePosition(float dt)
	{
		if (!is_static)
		{
			// calculate velocity
			const sf::Vector2f velocity = position - position_old;

			// save current_position
			position_old = position;

			// Perform Verlet Integration
			position = position + velocity + acceleration * dt * dt;


			//std::cout << acceleration.y << " accy\n";
			//std::cout << acceleration.x << " accx\n";

			// resets acceleration
			acceleration = {};
		}
	}

	void accelerate(sf::Vector2f acc)
	{
		acceleration += acc;
	}

	void addForce(sf::Vector2f force)
	{
		acceleration += force / mass;
		/*std::cout << force.x << "," << force.y << " force\n";*/
	}

	sf::Vector2f getVelocity()
	{
		return position - position_old;
	}
};