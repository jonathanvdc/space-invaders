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
			/// Updates the game model based on the given time delta.
			void update(si::model::Game& game, duration_t timeDelta) const final override;
		};
	}
}