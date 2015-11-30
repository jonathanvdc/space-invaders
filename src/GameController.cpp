#include "GameController.h"

#include <algorithm>
#include "Container.h"
#include "IController.h"

using namespace si;
using namespace si::controller;

/// Checks if this game controller is still alive.
/// This is equivalent to checking if 
/// any of its children are still alive.
bool GameController::isAlive() const
{
	return std::any_of(this->items.begin(), this->items.end(), 
		[](const std::shared_ptr<IController>& x) -> bool { return x->isAlive(); });
}

/// Updates the game model based on the given time delta.
void GameController::update(si::model::Game& game, duration_t timeDelta)
{
	// First, remove all dead controllers.
	this->items.erase(
		std::remove_if(this->items.begin(), this->items.end(),
			[](const std::shared_ptr<IController>& x) -> bool { return !x->isAlive(); }),
		this->items.end());
	// Next, update controllers.
	for (const auto& item : this->items)
	{
		item->update(game, timeDelta);
	}
}