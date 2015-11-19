#pragma once
#include <SFML/System/Vector2.hpp>

namespace si
{
	namespace model
	{
		class Entity
		{
		public:
			/// Gets this entity's position, as an offset 
			/// relative to the origin.
			virtual sf::Vector2<double> getPosition() const = 0;
		};
	}
}