// SpaceInvaders.cpp : Defines the entry point for the application.
//

#include <SFML/Graphics.hpp>
#include "RandomGenerator.h"

int main(int argc, char* argv[])
{
	sf::RenderWindow w(sf::VideoMode(250, 250), "Namespace invaders");

	while (w.isOpen())
	{
		sf::Event event;
		while (w.pollEvent(event))
		{
			if (event.type == sf::Event::Closed)
			{
				w.close();
			}
		}
		w.clear(sf::Color::Cyan);

		for (size_t i = 0; i < 200; i++)
		{
			auto& rng = si::RandomGenerator::instance;
			sf::CircleShape circle{ rng.nextReal<float>(0.1f, 15.0f) };
			circle.setPosition(rng.nextReal<float>(0, (float)w.getSize().x), rng.nextReal<float>(0, (float)w.getSize().y));
			circle.setFillColor(sf::Color(rng.nextInt<int>(0, 255), rng.nextInt<int>(0, 255), rng.nextInt<int>(0, 255), rng.nextInt<int>(0, 255)));
			w.draw(circle);
		}

		w.display();
	}

	return 0;
}

