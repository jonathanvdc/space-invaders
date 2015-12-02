#include "ActionController.h"

#include <functional>
#include "Common.h"
#include "Game.h"
#include "IController.h"

using namespace si;
using namespace si::controller;

/// Creates a new action controller from the given
/// function.
ActionController::ActionController(
	const ActionFunction& performAction)
	: isStillAlive(true), performAction(performAction)
{ }

/// Checks if this controller is still "alive".
/// A live controller will remain in the controller
/// list, whereas dead controllers will be eliminated.
bool ActionController::isAlive() const
{
	return this->isStillAlive;
}

/// Updates the game model based on the given time delta.
void ActionController::update(si::model::Game& game, duration_t timeDelta)
{
	this->isStillAlive = this->performAction(game, timeDelta);
}