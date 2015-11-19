// SpaceInvaders.cpp : Defines the entry point for the application.
//

#include <functional>
#include <SFML/Graphics.hpp>
#include "RandomGenerator.h"
#include "Stopwatch.h"
#include "PathEntity.h"

int main(int argc, char* argv[])
{
	sf::RenderWindow w(sf::VideoMode(800, 600), "Namespace invaders");

	si::model::PathEntity entity([=](si::model::time_delta t) 
	{ 
		return sf::Vector2<double>(std::cos(t * 2.0), std::sin(t * 2.0)); 
	});

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

		entity.updateTime(si::Stopwatch::instance.delta().count());

		/*for (size_t i = 0; i < 200; i++)
		{
			auto& rng = si::RandomGenerator::instance;
			sf::CircleShape circle{ rng.nextReal<float>(0.1f, 15.0f) };
			circle.setPosition(rng.nextReal<float>(0, (float)w.getSize().x), rng.nextReal<float>(0, (float)w.getSize().y));
			circle.setFillColor(sf::Color(rng.nextInt<int>(0, 255), rng.nextInt<int>(0, 255), rng.nextInt<int>(0, 255), rng.nextInt<int>(0, 255)));
			w.draw(circle);
		}*/

		sf::CircleShape circle{ 5.0f };
		circle.setFillColor(sf::Color::Cyan);
		auto pos = 0.95 * entity.getPosition();
		auto screenSize = 0.5f * sf::Vector2f(w.getSize());
		circle.setPosition((float)pos.x * screenSize.x + screenSize.x, (float)pos.y * screenSize.y + screenSize.y);
		w.draw(circle);

		w.display();
	}

	return 0;
}

