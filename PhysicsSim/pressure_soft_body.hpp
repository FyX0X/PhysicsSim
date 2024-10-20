#pragma once
#include <iostream>

class PressureSoftBody
{
public:
	PressureSoftBody(std::vector<VerletObject*> points, float initial_pressure):
		points(points),
		initial_pressure(initial_pressure),
		initial_volume(calculateVolume())
	{
		std::cout << calculateVolume();
	}


	void update()
	{
		float pressure = calculatePressure();

		// for each edge (with vertex i and j)
		for (size_t i = 0; i < points.size(); i++)
		{
			int j = (i + 1) % points.size();
			sf::Vector2f edge = points[j]->position - points[i]->position;
			float length = hypot(edge.x, edge.y);			

			// calculate normal vector
			sf::Vector2f normal = { edge.y / length, -edge.x / length };

			// force = P*A (here pressure * length/2)
			points[i]->addForce(pressure * length / 2 * normal);
			points[j]->addForce(pressure * length / 2 * normal);

			sf::Vector2f force = pressure * length / 2 * normal;
		}

	}


private:
	const std::vector<VerletObject*> points;
	const float initial_pressure;
	const float initial_volume;

	float calculateVolume()	// actually area
	{
		float area = 0;
		for (size_t i = 0; i < points.size(); i++)
		{
			int j = (i + 1) % points.size();

			area += points[i]->position.x * points[j]->position.y;
			area -= points[i]->position.y * points[j]->position.x;
		}

		return area/2;
	}

	float calculatePressure()
	{
		// PV = nRT
		return initial_pressure * initial_volume / calculateVolume();
	}
};