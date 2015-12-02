#pragma once

#include <functional>
#include "Common.h"
#include "Game.h"
#include "IController.h"

namespace si
{
	namespace controller
	{
		/// Defines a type of controller that performs some action 
		/// every frame. Said action is performed by calling
		/// a function, which tells if this action controller
		/// is still alive.
		class ActionController final : public IController
		{
		public:
			typedef std::function<bool(si::model::Game& game, duration_t timeDelta)> ActionFunction;

			/// Creates a new action controller from the given
			/// function, which is called on every frame
			/// for as long as this controller is alive.
			/// Once this function returns false, the action
			/// controller is marked as dead.
			ActionController(
				const ActionFunction& performAction);

			/// Checks if this controller is still "alive".
			/// A live controller will remain in the controller
			/// list, whereas dead controllers will be eliminated.
			bool isAlive() const final override;

			/// Updates the game model based on the given time delta.
			void update(si::model::Game& game, duration_t timeDelta) final override;

			
		private:
			bool isStillAlive;
			ActionFunction performAction;
		};

	}
}
