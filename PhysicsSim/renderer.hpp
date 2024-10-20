#include "solver.hpp"
#include <iostream>

struct Renderer
{
public:

	explicit
	Renderer(sf::RenderTarget& target):
		m_target{target},
		camera_position{0,0},
		camera_scale(1)
	{}

	void setScale(float new_scale, sf::Vector2f scale_origin = {0,0})
	{
		sf::Vector2f world_pos_before = getWorldCoordinate(scale_origin);
		camera_scale = new_scale;
		sf::Vector2f world_pos_after = getWorldCoordinate(scale_origin);
		camera_position += world_pos_before - world_pos_after;
	}

	void changeZoom(float factor, sf::Vector2f scale_origin = { 0,0 })
	{
		sf::Vector2f world_pos_before = getWorldCoordinate(scale_origin);
		camera_scale *= factor;
		sf::Vector2f world_pos_after = getWorldCoordinate(scale_origin);
		camera_position += world_pos_before - world_pos_after;
	}

	void setPosition(sf::Vector2f position)
	{
		camera_position = position;
	}

	sf::Vector2f getCameraPosition()
	{
		return camera_position;
	}

	float getScale()
	{
		return camera_scale;
	}

	sf::Vector2f getWorldCoordinate(sf::Vector2f screen_pos)
	{
		return screen_pos / camera_scale + camera_position;
	}

	void render(const Solver& solver)
	{
		// Render Constraint
		if (solver.isConstraintEnabled())
		{
			// sets the color of the constraint
			m_target.clear(sf::Color::White);

			const sf::Vector3f constraint = solver.getConstraint();
			draw_circle({ constraint.x, constraint.y }, constraint.z, sf::Color::Black, 128);
		}

		// Render Objects
		const auto& objects = solver.getObject();
		for (auto& pObj : objects)
		{
			draw_circle(pObj->position, pObj->radius, pObj->color);
		}


		// Render Links
		const auto& links = solver.getLinks();
		for (auto& link : links)
		{
			draw_line(link.object_1.position, link.object_2.position, sf::Color::Red);
		}

		// Render Springs
		const auto& springs = solver.getSprings();
		for (auto& spring : springs)
		{
			draw_line(spring.object_1.position, spring.object_2.position, sf::Color::Blue);
		}



	}


private:
	sf::RenderTarget& m_target;

	sf::Vector2f camera_position;
	float camera_scale;


	void draw_circle(sf::Vector2f position, float radius, sf::Color color, int point_count=32)
	{
		// initialize shape
		sf::CircleShape circle(radius);
		circle.setOrigin(radius, radius);
		circle.setPointCount(point_count);
		circle.setFillColor(color);

		// calcutate relative screen size and position
		sf::Vector2f screen_position = (position - camera_position) * camera_scale;
		circle.setScale(camera_scale, camera_scale);
		circle.setPosition(screen_position);

		// draw shape
		m_target.draw(circle);
	}

	void draw_line(sf::Vector2f pos1, sf::Vector2f pos2, sf::Color color)
	{
		// initialize line
		sf::VertexArray line(sf::LinesStrip, 2);
		line[0].color = color;
		line[1].color = color;

		// calculate screen coordinate
		line[0].position = (pos1 - camera_position) * camera_scale;
		line[1].position = (pos2 - camera_position) * camera_scale;

		// draw line
		m_target.draw(line);
	}
};