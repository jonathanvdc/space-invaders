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
	return{ 0.0, 0.0 };
}

/// Adds the given time delta to the total amount 
/// of time elapsed.
/// This will recursively update all entities in the
/// game.
void Game::updateTime(duration_t delta)
{
	this->Entity::updateTime(delta);

	for (const auto& item : this->entities)
	{
		item->updateTime(delta);
	}
}

/// Adds an entity to this game.
void Game::addEntity(Entity_ptr item)
{
	this->entities.push_back(item);
}

/// Removes an entity from the game.
/// A boolean is returned that indicates
/// whether the action was successful.
bool Game::removeEntity(Entity_ptr item)
{
	this->entities.erase(std::remove(this->entities.begin(), this->entities.end(), item), this->entities.end());
}