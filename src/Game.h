#pragma once
#include "Common.h"
#include "Entity.h"
#include "Container.h"

namespace si
{
	namespace model
	{
		/// An entity that describes the game itself.
		class Game final : public Container<Entity>, public virtual Entity
		{
		public:
			/// Gets the game's origin.
			Vector2d getPosition() const final override;

			/// Adds the given time delta to the total amount 
			/// of time elapsed.
			/// This will recursively update all entities in the
			/// game.
			void updateTime(duration_t delta) final override;
		};
	}
}