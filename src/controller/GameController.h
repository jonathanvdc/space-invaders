#pragma once

#include "Container.h"
#include "IController.h"

namespace si
{
	namespace controller
	{
		/// Defines a controller class for the entire game.
		class GameController final : public Container<IController>, public virtual IController
		{
		public:
			/// Checks if this game controller is still alive.
			/// This is equivalent to checking if 
			/// any of its children are still alive.
			bool isAlive() const final override;

			/// Updates the game model based on the given time delta.
			void update(si::model::Game& game, duration_t timeDelta) final override;
		};
	}
}