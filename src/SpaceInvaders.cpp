// SpaceInvaders.cpp : Defines the entry point for the application.
//

#include <chrono>
#include <exception>
#include <functional>
#include <map>
#include <SFML/Graphics.hpp>
#include "Common.h"
#include "RandomGenerator.h"
#include "Stopwatch.h"
#include "PathEntity.h"
#include "DriftingEntity.h"
#include "ProjectileEntity.h"
#include "Game.h"
#include "GameRenderer.h"
#include "GameController.h"
#include "RenderContext.h"
#include "TextRenderable.h"
#include "PathOffsetRenderable.h"
#include "SpriteRenderable.h"
#include "RelativeBoxRenderable.h"
#include "PathController.h"
#include "PlayerController.h"
#include "ActionController.h"
#include "IntervalActionController.h"
#include "ProjectileCollisionController.h"
#include "ShipCollisionController.h"
#include "Scene.h"

using namespace std::chrono_literals;

/// A physics sim for testing purposes.
void physicsSim(sf::RenderWindow& w)
{
	si::model::Game game;
	auto& rng = si::RandomGenerator::instance;
	for (size_t i = 0; i < 2000; i++)
	{
		double size = rng.nextReal<double>(0.001, 0.01);
		double mass = size * 200;
		game.add(std::make_shared<si::model::DriftingEntity>(
			si::model::PhysicsProperties(mass, size),
			si::Vector2d(rng.nextReal<double>(-2, 2), rng.nextReal<double>(-2, 2))));
	}

	auto entities = game.getAll<si::model::DriftingEntity>();

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
}

/// Creates a simple projectile with the given characteristics.
std::shared_ptr<si::model::ProjectileEntity> createProjectile(
	const si::model::PhysicsEntity& origin, 
	double mass, double radius, si::Vector2d veloc)
{
	auto bulletOffset = (origin.getPhysicsProperties().radius + radius) * si::normalizeVec(veloc);
	auto bulletPos = origin.getPosition() + bulletOffset;
	return std::make_shared<si::model::ProjectileEntity>(
		si::model::PhysicsProperties(mass, radius),
		bulletPos, origin.getVelocity() + veloc);
}

/// Fires a simple projectile with the given characteristics
/// from the given origin. Momentum is conserved.
std::shared_ptr<si::model::ProjectileEntity> fireProjectile(
	si::model::DriftingEntity& origin,
	double mass, double radius, si::Vector2d veloc)
{
	auto result = createProjectile(origin, mass, radius, veloc);
	origin.accelerate(-si::Vector2d(veloc));
	return result;
}

/// Fires a simple projectile with the given characteristics
/// from the given origin. Momentum is conserved.
std::shared_ptr<si::model::ProjectileEntity> fireProjectile(
	si::model::DriftingEntity& origin,
	double mass, double radius, double speed)
{
	auto veloc = speed * origin.getOrientation();
	return fireProjectile(origin, mass, radius, veloc);
}

/// Creates a sprite view for the given texture and dimensions
/// that follows the given entity around.
si::view::IRenderable_ptr createSprite(
	const si::model::Entity_ptr& model,
	const std::shared_ptr<sf::Texture>& texture,
	si::DoubleRect relativeSize)
{
	auto sprite = std::make_shared<si::view::SpriteRenderable>(texture);
	auto box = std::make_shared<si::view::RelativeBoxRenderable>(sprite, relativeSize);
	return std::make_shared<si::view::PathOffsetRenderable>(box, [=]() { return model->getPosition(); });
}

/// Creates a sprite view for the given texture and dimensions
/// that follows the given entity around.
si::view::IRenderable_ptr createSprite(
	const si::model::Entity_ptr& model,
	const std::shared_ptr<sf::Texture>& texture,
	double width, double height)
{
	return createSprite(model, texture, si::DoubleRect(0.0, 0.0, width, height));
}

/// Plays a space invaders game, as defined by the
/// given scene. A render window is used to
/// render the game.
void playGame(sf::RenderWindow& w, si::Scene& scene)
{
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

		auto delta = si::Stopwatch::instance.delta();

		scene.frame(w, delta);

		w.display();
	}
}

/// The actual space invaders game.
void spaceInvaders(sf::RenderWindow& w)
{
	si::Scene scene;

	sf::Font font;

	if (!font.loadFromFile("arial.ttf"))
	{
		throw std::runtime_error("Couldn't load 'arial.ttf'.");
	}

	auto shipTex = std::make_shared<sf::Texture>();
	if (!shipTex->loadFromFile("ship_cpp.png"))
	{
		throw std::runtime_error("Couldn't load 'ship_cpp.png'.");
	}

	auto framecounter = std::make_shared<si::view::TextRenderable>("", font, sf::Color::Red);
	scene.addRenderable(framecounter);

	auto ship = std::make_shared<si::model::ShipEntity>(
		si::model::PhysicsProperties(20.0, 0.1), si::Vector2d(0.5, 0.5), 40.0);
	scene.addEntity(ship, createSprite(ship, shipTex, 0.1, 0.1));
	scene.addController(std::make_shared<si::controller::PlayerController>(ship, 0.05));
	scene.addController(std::make_shared<si::controller::IntervalActionController>(0.5s,
		[](si::model::Game&, si::duration_t)
		{
			return sf::Keyboard::isKeyPressed(sf::Keyboard::Space);
		},
		[&](si::model::Game& game, si::duration_t)
		{
			auto bullet = fireProjectile(*ship, 5.0, 0.05, 0.1);
			scene.addEntity(bullet, createSprite(bullet, shipTex, 0.05, 0.05));
			scene.addController(std::make_shared<si::controller::ProjectileCollisionController>(bullet));
			scene.addBoundsConstraint(bullet, si::DoubleRect(-0.5, -0.5, 2.0, 2.0));
		},
		[=](si::model::Game&, si::duration_t)
		{
			return ship->isAlive();
		}));

	auto other = std::make_shared<si::model::ShipEntity>(
		si::model::PhysicsProperties(20.0, 0.1), si::Vector2d(0.5, 0.5), 40.0);
	scene.addEntity(other, createSprite(other, shipTex, 0.1, 0.1));
	scene.addController(std::make_shared<si::controller::PathController>(other, 5.0, [=](si::duration_t t) { return si::Vector2d(std::cos(t.count()) / 2.0 + 0.5, std::sin(t.count()) / 2.0 + 0.5); }));

	(void)si::Stopwatch::instance.delta();

	si::duration_t elapsed = si::duration_t(0.0);
	int framecount = 0;
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

		auto delta = si::Stopwatch::instance.delta();
		elapsed += delta;
		framecount++;

		if (elapsed > si::duration_t(1.0))
		{
			framecounter->setText(std::to_string(framecount / elapsed.count()) + "fps");
			framecount = 0;
			elapsed -= si::duration_t(1.0);
		}

		scene.frame(w, delta);

		w.display();
	}
}

int main(int argc, char* argv[])
{
	sf::RenderWindow w(sf::VideoMode(800, 600), "Namespace invaders");

	spaceInvaders(w);

	return 0;
}

