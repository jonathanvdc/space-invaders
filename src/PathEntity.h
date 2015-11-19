#pragma once
#include <functional>
#include "Common.h"
#include "Entity.h"
#include "PhysicsEntity.h"

namespace si
{
	namespace model
	{
		typedef std::function<sf::Vector2<double>(duration_t)> PathFunction;

		/// Defines a type of entity whose position is defined
		/// by a path function. This type of entity also has physics 
		/// properties.
		class PathEntity : public PhysicsEntity
		{
		public:
			/// Creates a new path entity from the given path.
			PathEntity(PhysicsProperties physProps, PathFunction path);

			/// Gets this path entity's position.
			virtual Vector2d getPosition() const override;
		private:
			PathFunction path;
		};
	}
}

