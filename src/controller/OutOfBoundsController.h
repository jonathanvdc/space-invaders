#pragma once

#include "Common.h"
#include "model/Entity.h"
#include "IController.h"

namespace si
{
	namespace controller
	{
		/// A type of controller that removes
		/// its associated entity from the game
		/// if it goes out of bounds.
		class OutOfBoundsController final : public IController
		{
		public:
			/// Creates an out-of-bounds controller
			/// that manages the given entity, by
			/// removing it from the game if its
			/// position exceeds the given bounds.
			OutOfBoundsController(
				const si::model::Entity_ptr& entity,
				si::DoubleRect bounds);

			/// Checks if this controller is still "alive".
			/// A live controller will remain in the controller
			/// list, whereas dead controllers will be eliminated.
			bool isAlive() const final override;

			/// Updates the game model based on the given time delta.
			void update(si::model::Game& game, duration_t timeDelta) final override;
		private:
			si::model::Entity_ptr entity;
			const si::DoubleRect bounds;
		};
	}
}
