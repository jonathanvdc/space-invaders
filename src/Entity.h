#pragma once
#include <memory>
#include "Common.h"

namespace si
{
	namespace model
	{
		/// Defines an entity: the absolute base class
		/// for just about anything in the model.
		class Entity
		{
		public:
			/// Gets this entity's position, as an offset 
			/// relative to the origin. The game's "board"
			/// ranges from zero to one in both x and y 
			/// directions.
			virtual Vector2d getPosition() const = 0;

			/// Adds the given time delta to the total amount 
			/// of time elapsed.
			/// The entity may also update some of its properties,
			/// such as its position, based on this delta.
			virtual void updateTime(duration_t delta);

			/// Gets the total lifetime of this entity,
			/// i.e. the number of seconds that have elapsed
			/// since its creation.
			duration_t getLifetime() const;
		private:
			duration_t elapsed{ 0.0 };
		};

		typedef std::shared_ptr<Entity> Entity_ptr;
	}
}