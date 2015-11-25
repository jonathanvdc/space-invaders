#include "Game.h"

#include <vector>
#include <memory>
#include <algorithm>
#include "Common.h"
#include "Entity.h"

using namespace si;
using namespace si::model;

/// Gets the game's origin.
Vector2d Game::getPosition() const
{
	return Vector2d(0.0, 0.0);
}

/// Adds the given time delta to the total amount 
/// of time elapsed.
/// This will recursively update all entities in the
/// game.
void Game::updateTime(duration_t delta)
{
	this->Entity::updateTime(delta);

	for (const auto& item : this->items)
	{
		item->updateTime(delta);
	}
}