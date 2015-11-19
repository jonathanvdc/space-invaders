#pragma once
#include <functional>
#include <SFML/System/Vector2.hpp>
#include "Entity.h"

namespace si
{
	namespace model
	{
		typedef double time_delta;
		typedef std::function<sf::Vector2<double>(time_delta)> PathFunction;

		/// Defines a type of entity whose position is defined
		/// by a path function.
		class PathEntity : public Entity
		{
		public:
			/// Creates a new path entity from the given path.
			PathEntity(PathFunction path);

			/// Gets this path entity's position.
			virtual sf::Vector2<double> getPosition() const override;

			/// Adds the given time delta to the total amount of time elapsed.
			void updateTime(time_delta delta);
		private:
			time_delta elapsed;
			PathFunction path;
		};
	}
}

