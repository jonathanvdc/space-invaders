// SpaceInvaders.cpp : Defines the entry point for the application.
//

#include <cstddef>
#include <chrono>
#include <exception>
#include <functional>
#include <iostream>
#include <map>
#include <string>
#include <SFML/Graphics.hpp>
#include "Common.h"
#include "Stopwatch.h"
#include "Scene.h"
#include "parser/SceneDescription.h"

/// Plays a space invaders game, as defined by the
/// given scene. A render window is used to
/// render the game.
void playGame(si::Scene& scene)
{
	sf::RenderWindow w(sf::VideoMode(800, 600), scene.getName());

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

int main(int argc, char* argv[])
{
	if (argc != 2)
	{
		std::cout << "Expected exactly one argument, which refers to a scene description. "
				  << "Got " << (argc < 2 ? "none" : std::to_string(argc - 1)) << "."
				  << std::endl;
		return 1;
	}

	try
	{
		auto scene = si::parser::parseScene(argv[1]);

		playGame(*scene);
		return 0;
	}
	catch (si::parser::XMLParseException& ex)
	{
		std::cout << "The given file is not valid XML. Here's what went wrong: " << std::endl
			<< ex.what() << std::endl;
		return 1;
	}
	catch (si::parser::SceneDescriptionException& ex)
	{
		std::cout << "The given scene description contains an error. To be exact: " << std::endl
			<< ex.what() << std::endl;
		return 1;
	}
}
