#include "IntervalActionController.h"

#include <chrono>
#include <functional>
#include "Common.h"
#include "Game.h"
#include "IController.h"

using namespace si;
using namespace si::controller;
using namespace std::chrono_literals;

/// Creates a new action controller from the given
/// function and duration.
IntervalActionController::IntervalActionController(
	duration_t interval,
	const PerformActionPredicate& actionPredicate,
	const ActionFunction& performAction,
	const LivelinessPredicate& livelinessPredicate)
	: isStillAlive(true), interval(interval), elapsed(0.0s), 
	  actionPredicate(actionPredicate), performAction(performAction),
	  livelinessPredicate(livelinessPredicate)
{ }

/// Checks if this controller is still "alive".
/// A live controller will remain in the controller
/// list, whereas dead controllers will be eliminated.
bool IntervalActionController::isAlive() const
{
	return this->isStillAlive;
}

/// Updates the game model based on the given time delta.
void IntervalActionController::update(si::model::Game& game, duration_t timeDelta)
{
	this->elapsed += timeDelta;	
	if (this->isStillAlive)
	{
		if (this->elapsed >= this->interval && this->actionPredicate(game, timeDelta))
		{
			this->performAction(game, timeDelta);
			this->elapsed = 0.0s;
		}
		this->isStillAlive = this->livelinessPredicate(game, timeDelta);
	}
}