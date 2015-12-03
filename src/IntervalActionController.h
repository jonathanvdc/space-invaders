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
		/// every frame, provided that a set amount of time
		/// has elapsed since the last time that the action
		/// was performed. Said action is performed by calling
		/// a function, which tells if this action controller
		/// is still alive.
		class IntervalActionController final : public IController
		{
		public:
			/// A type for functions that check if an action
			/// should be performed.
			typedef std::function<bool(si::model::Game& game, duration_t timeDelta)> PerformActionPredicate;

			/// A type for functions that perform some action.
			typedef std::function<void(si::model::Game& game, duration_t timeDelta)> ActionFunction;

			/// A type for functions that check if a controller
			/// is still alive.
			typedef std::function<bool(si::model::Game& game, duration_t timeDelta)> LivelinessPredicate;

			/// Creates a new action controller from the given
			/// perform-action predicate, perform-action
			/// function and liveliness predicate.
			/// A duration determines how much time
			/// must pass between two calls to the
			/// action-performing function.
			IntervalActionController(
				duration_t interval,
				const PerformActionPredicate& actionPredicate,
				const ActionFunction& performAction,
				const LivelinessPredicate& livelinessPredicate);

			/// Creates a new action controller from 
			/// the given perform-action
			/// function and liveliness predicate.
			/// A duration determines how much time
			/// must pass between two calls to the
			/// action-performing function.
			IntervalActionController(
				duration_t interval,
				const ActionFunction& performAction,
				const LivelinessPredicate& livelinessPredicate);

			/// Checks if this controller is still "alive".
			/// A live controller will remain in the controller
			/// list, whereas dead controllers will be eliminated.
			bool isAlive() const final override;

			/// Updates the game model based on the given time delta.
			void update(si::model::Game& game, duration_t timeDelta) final override;

		private:
			bool isStillAlive;
			const duration_t interval;
			duration_t elapsed;
			PerformActionPredicate actionPredicate;
			ActionFunction performAction;
			LivelinessPredicate livelinessPredicate;
		};

	}
}
