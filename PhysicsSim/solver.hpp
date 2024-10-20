#pragma once

#include <vector>
#include <cmath>
#include <SFML/Graphics.hpp>
#include <iostream>
#include "object.hpp"
#include "link.hpp"
#include "spring.hpp"
#include "pressure_soft_body.hpp"
#include "rigid_body.hpp"



class Solver
{
public:

	Solver() = default;    


	int addObject(sf::Vector2f position, float radius, bool is_static=false, sf::Vector2f vel = {0.0f, 0.0f})
	{
		std::cout << "add object " << m_objects.size() << "\n";
		//m_objects.emplace_back(position, radius, m_objects.size(), is_static);
		m_objects.push_back(std::make_unique<VerletObject>(position, radius, m_objects.size(), is_static, vel));
		
		return (*m_objects[m_objects.size() - 1]).ID;
	}


	int addLink(int _id1, int _id2, float strength = 1.0f)
	{
		std::cout << "add link " << m_links.size() << "\n";
		VerletObject& obj1 = *m_objects[_id1];
		VerletObject& obj2 = *m_objects[_id2];
		m_links.emplace_back(obj1, obj2, m_links.size(), strength);

		return m_links[m_links.size() - 1].ID;
	}

	int addSpring(int _id1, int _id2, float spring_constant = 1.0f, float damping = 1.0f)
	{
		std::cout << "add spring " << m_springs.size() << "\n";
		VerletObject& obj1 = *m_objects[_id1];
		VerletObject& obj2 = *m_objects[_id2];
		m_springs.emplace_back(obj1, obj2, m_springs.size(), spring_constant, damping);
		return m_springs[m_springs.size() - 1].ID;
	}

	void createPressureSoftBody(sf::Vector2f pos, const int number_of_points, float size, float initial_pressure, float spring_strength)
	{
		std::vector<int> IDs;
		// create each vertexes
		for (size_t i = 0; i < number_of_points; i++)
		{
			float theta = 2 * 3.1415926535 / number_of_points * i;
			float radius = 2.5f * size / (2 * sin(3.141592 / number_of_points));
			sf::Vector2f delta_position = {radius * cos(theta), radius * sin(theta) };
			IDs.push_back(addObject(pos + delta_position, size));
		}

		// get pointer for each vertexes
		std::vector<VerletObject*> points;
		for (size_t i = 0; i < number_of_points; i++)
		{
			points.push_back(m_objects[IDs[i]].get());
		}

		// add springs
		for (size_t i = 0; i < number_of_points; i++)
		{
			int j = (i + 1) % number_of_points;
			addSpring(IDs[i], IDs[j], spring_strength);
		}

		// create soft body
		m_pressure_soft_bodies.emplace_back(points, initial_pressure);
	}


	const std::vector<std::unique_ptr<VerletObject>>& getObject() const
	{
		return m_objects;
	}

	const std::vector<Link>& getLinks() const
	{
		return m_links;
	}

	const std::vector<Spring>& getSprings() const
	{
		return m_springs;
	}


	void update(float dt)
	{
		float sub_dt = dt / sub_steps;
		for (int i = 0; i < sub_steps; i++)
		{

			if (n_body_problem) applyNewtonianGravity();
			else applyEarthGravity();
			if (constraint_enabled)applyConstraint();
			applyLink();
			applySpring();
			updatePressureSoftBodies();
			if (collision_enabled) checkCollision();
			if (drag_enabled) applyDrag();
			updatePosition(sub_dt);
		}
	}

	sf::Vector3f getConstraint() const
	{
		return { constraint_position.x, constraint_position.y, constraint_radius };
	}

	bool isConstraintEnabled() const
	{
		return constraint_enabled;
	}

	void spawnSoftBody(sf::Vector2f center, float size, float strength, float damping)
	{
		// add each balls
		int id1 = addObject(center + sf::Vector2f(size * 2.5f * cos(3.1415 * 2 / 3), size * 2.5f * sin(3.1415 * 2 / 3)), size);
		int id2 = addObject(center + sf::Vector2f(size * 2.5f * cos(3.1415 / 3), size * 2.5f * sin(3.1415 / 3)), size);
		int id3 = addObject(center + sf::Vector2f(-size * 2.5f, 0), size);
		int id4 = addObject(center, size);
		int id5 = addObject(center + sf::Vector2f(size * 2.5f, 0), size);
		int id6 = addObject(center + sf::Vector2f(size * 2.5f * cos(3.1415 * 4 / 3), size * 2.5f * sin(3.1415 * 4 / 3)), size);
		int id7 = addObject(center + sf::Vector2f(size * 2.5f * cos(3.1415 * 5 / 3), size * 2.5f * sin(3.1415 * 5 / 3)), size);

		// add springs
		// 
		// inner springs
		addSpring(id1, id4, strength, damping);
		addSpring(id2, id4, strength, damping);
		addSpring(id3, id4, strength, damping);
		addSpring(id5, id4, strength, damping);
		addSpring(id6, id4, strength, damping);
		addSpring(id7, id4, strength, damping);

		// outer springs
		addSpring(id1, id2, strength, damping);
		addSpring(id2, id5, strength, damping);
		addSpring(id5, id7, strength, damping);
		addSpring(id7, id6, strength, damping);
		addSpring(id6, id3, strength, damping);
		addSpring(id3, id1, strength, damping);
	}
	void addRectSoftBody(sf::Vector2f center, float size, int width, int height, float strength, float damping)
	{

		// adding vertex
		std::vector<std::vector<int>> IDs(width, std::vector<int>(height, 0));
		for (size_t x = 0; x < width; x++)
		{
			for (size_t y = 0; y < height; y++)
			{
				sf::Vector2f position = { x * size * 2.5f, y * size * 2.5f };
				IDs[x][y] = addObject(center + position, size);
			}
		}


		// add springs

		// horizontal springs
		for (size_t x = 0; x < width - 1; x++)
		{
			for (size_t y = 0; y < height; y++)
			{
				addSpring(IDs[x][y], IDs[x+1][y], strength, damping);
			}
		}
		// vertical springs
		for (size_t y = 0; y < height - 1; y++)
		{
			for (size_t x = 0; x < width; x++)
			{
				addSpring(IDs[x][y], IDs[x][y+1], strength, damping);
			}
		}
		// diagonal springs top left bottom right
		for (size_t x = 0; x < width - 1; x++)
		{
			for (size_t y = 0; y < height -1; y++)
			{
				addSpring(IDs[x][y], IDs[x + 1][y+1], strength, damping);
			}
		}
		// diagonal springs top rigth bottom left
		for (size_t x = 0; x < width - 1; x++)
		{
			for (size_t y = 0; y < height - 1; y++)
			{
				addSpring(IDs[x+1][y], IDs[x][y + 1], strength, damping);
			}
		}

	}



private:

	const bool n_body_problem = false;
	const float graviation_constant = 100000.0f;
	const float drag_constant = 20.0f;
	const bool drag_enabled = false;
	const bool collision_enabled = true;
	const bool constraint_enabled = true;

	sf::Vector2f gravity = { 0.0f, 981.0f };
	//std::vector<VerletObject> m_objects;
	std::vector<std::unique_ptr<VerletObject>> m_objects;
	std::vector<Link> m_links;
	std::vector<Spring> m_springs;
	std::vector<PressureSoftBody> m_pressure_soft_bodies;
	const int sub_steps = 10;

	// constraints
	sf::Vector2f constraint_position = { 500.0f, 450.0f };
	float constraint_radius = 400.0f;


	void updatePosition(float dt)
	{
		for (auto& ptr : m_objects)
		{
			// get the object from pointer
			auto& obj = *ptr;
			obj.updatePosition(dt);
		}
	}

	void applyLink()
	{
		for (auto& link : m_links)
		{
			link.apply();
		}
	}

	void applySpring()
	{
		for (auto& spring : m_springs)
		{
			spring.apply();
		}
	}

	void updatePressureSoftBodies()
	{
		for (auto& PSB : m_pressure_soft_bodies)
		{
			PSB.update();
		}
	}

	void applyDrag()
	{
		for (auto& ptr : m_objects)
		{
			auto& obj = *ptr;
			
			sf::Vector2f vel = obj.position - obj.position_old;
			float speed = sqrt(vel.x * vel.x + vel.y * vel.y);
			sf::Vector2f drag = -0.5f * vel * speed * drag_constant * obj.radius * obj.radius * 3.1415f;
			obj.addForce(drag);
			std::cout << drag.x << ";" << drag.y << "\n";
		}
	}

	void applyEarthGravity()
	{
		for (auto& ptr : m_objects)
		{
			// get the object from pointer
			auto& obj = *ptr;
			obj.accelerate(gravity);
		}
	}

	void applyNewtonianGravity()
	{
		const int object_count = m_objects.size();

		for (int i = 0; i < object_count; i++)
		{
			VerletObject& object_1 = *m_objects[i];
			for (int j = i + 1; j < object_count; j++)
			{
				VerletObject& object_2 = *m_objects[j];
				const sf::Vector2f axis = object_1.position - object_2.position;
				const float dist2 = axis.x * axis.x + axis.y * axis.y;

				const float dist = sqrt(dist2);
				const sf::Vector2f n = axis / dist;

				const float force = graviation_constant * object_1.mass * object_2.mass / dist2;

				// add gravitationnal pull
				if (!object_1.is_static) object_1.addForce(-force * n);
				if (!object_2.is_static) object_2.addForce(force * n);
				
			}
		}
	}

	void applyConstraint()
	{

		for (auto& ptr : m_objects) {
			// get the object from pointer
			auto& obj = *ptr;

			const sf::Vector2f v = constraint_position - obj.position;
			const float        dist = sqrt(v.x * v.x + v.y * v.y);
			if (dist > (constraint_radius - obj.radius)) {
				const sf::Vector2f n = v / dist;
				obj.position = constraint_position - n * (constraint_radius - obj.radius);
			}
		}
	}

	void checkCollision()
	{
		const int object_count = m_objects.size();

		for (int i = 0; i < object_count; i++)
		{
			VerletObject& object_1 = *m_objects[i];
			for (int j = i+1; j < object_count; j++)
			{
				VerletObject& object_2 = *m_objects[j];
				const sf::Vector2f collision_axis = object_1.position - object_2.position;
				const float dist2 = collision_axis.x * collision_axis.x + collision_axis.y * collision_axis.y;
				const float min_dist = object_1.radius + object_2.radius;
				// check overlapping
				if (dist2 < min_dist * min_dist)
				{
					const float mass_ratio_1 = object_1.radius / (min_dist);
					const float mass_ratio_2 = object_2.radius / (min_dist);
					const float dist = sqrt(dist2);
					const sf::Vector2f n = collision_axis / dist;
					const float delta = object_1.radius + object_2.radius - dist;
					// update positions
					if (!object_1.is_static) object_1.position += mass_ratio_2 * delta * n;
					if (!object_2.is_static) object_2.position -= mass_ratio_1 * delta * n;
				}
			}
		}
	}
};
