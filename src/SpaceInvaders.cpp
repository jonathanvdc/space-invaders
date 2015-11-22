// SpaceInvaders.cpp : Defines the entry point for the application.
//

#include <functional>
#include <SFML/Graphics.hpp>
#include "Common.h"
#include "RandomGenerator.h"
#include "Stopwatch.h"
#include "PathEntity.h"
#include "DriftingEntity.h"
#include "Game.h"

int main(int argc, char* argv[])
{
	sf::RenderWindow w(sf::VideoMode(800, 600), "Namespace invaders");

	si::model::Game game;
	auto& rng = si::RandomGenerator::instance;
	for (size_t i = 0; i < 2000; i++)
	{
		double size = rng.nextReal<double>(0.001, 0.01);
		double mass = size * 200;
		game.addEntity(std::make_shared<si::model::DriftingEntity>(
			si::model::PhysicsProperties(mass, size), 
			si::Vector2d(rng.nextReal<double>(-2, 2), rng.nextReal<double>(-2, 2))));
	}
	
	auto entities = game.getEntities<si::model::DriftingEntity>();

	(void)si::Stopwatch::instance.delta();

	while (w.isOpen())
	{
		sf::Event event;
		while (w.pollEvent(event))
		{
			if (event.type == sf::Event::Closed)
			{
				w.close();
			}
			else if (event.type == sf::Event::Resized)
			{
				auto size = sf::Vector2f(w.getSize());
				w.setView(sf::View(0.5f * size, size));
			}
		}
		w.clear(sf::Color::Black);

		auto delta = si::Stopwatch::instance.delta();

		double maxVeloc = 0;
		for (size_t i = 0; i < entities.size(); i++)
		{
			auto& outer = entities[i];
			auto outerPos = outer->getPosition();
			double outerMass = outer->getPhysicsProperties().mass;
			for (size_t j = i + 1; j < entities.size(); j++)
			{
				auto& inner = entities[j];

				// simulate gravity
				const double G = 0.00006;
				auto innerPos = inner->getPosition();
				double innerMass = inner->getPhysicsProperties().mass;
				auto offset = innerPos - outerPos;
				double offsetLength = si::vecLength(offset);
				double gravityForce = G * innerMass * outerMass / offsetLength * offsetLength;
				outer->accelerate(delta.count() * gravityForce / innerMass * offset);
				inner->accelerate(-delta.count() * gravityForce / innerMass * offset);

				if (outer->overlaps(*inner))
				{
					auto totalMomentum = (si::vecLength(outer->getMomentum()) + si::vecLength(inner->getMomentum())) / 2.0;
					inner->accelerate(-(totalMomentum / inner->getPhysicsProperties().mass / si::vecLength(inner->getVelocity())) * inner->getVelocity() - inner->getVelocity());
					outer->accelerate(-(totalMomentum / outer->getPhysicsProperties().mass / si::vecLength(outer->getVelocity())) * outer->getVelocity() - outer->getVelocity());
				}
			}
			auto outerVeloc = si::vecLengthSqr(outer->getVelocity());
			if (outerVeloc > maxVeloc)
			{
				maxVeloc = outerVeloc;
			}
		}
		maxVeloc = std::sqrt(maxVeloc);

		for (const auto& item : entities)
		{
			auto dir = si::Vector2d(0.5, 0.5) - item->getPosition();
			// item.accelerate(0.01 * delta.count() * dir);

			double itemRadius = item->getPhysicsProperties().radius;
			float radius = (float)itemRadius * std::sqrt((float)w.getSize().x * (float)w.getSize().y);
			sf::CircleShape circle(radius);
			circle.setFillColor(sf::Color(255, 255, 255, sf::Uint8(255 * si::vecLength(item->getVelocity()) / maxVeloc)));
			auto pos = 0.95 * (item->getPosition() + (si::Vector2d(itemRadius, itemRadius) / 2.0));
			auto screenSize = 0.5f * sf::Vector2f(w.getSize());
			circle.setPosition((float)pos.x * screenSize.x + screenSize.x, (float)pos.y * screenSize.y + screenSize.y);
			w.draw(circle);
		}

		game.updateTime(delta);

		w.display();
	}

	return 0;
}

