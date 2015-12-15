#include "CollisionControllerBase.h"

#include <memory>
#include <vector>
#include "Common.h"
#include "model/Entity.h"
#include "model/PhysicsEntity.h"
#include "model/Game.h"
#include "IController.h"

using namespace si;
using namespace si::controller;

/// Checks if this controller is still "alive".
/// A live controller will remain in the controller
/// list, whereas dead controllers will be eliminated.
bool CollisionControllerBase::isAlive() const
{
	return this->getEntity() != nullptr;
}

/// Updates the game model based on the given time delta.
void CollisionControllerBase::update(si::model::Game& game, duration_t)
{
	// First, handle any collisions detected in the previous frame.
	for (const auto& item : this->collisionTargets)
	{
		this->handleCollision(game, item);
	}
	this->collisionTargets.clear();

	if (!this->isAlive())
	{
		// Don't try to detect collisions if
		// this controller is dead.
		return;
	}

	// Then, find collisions in this frame.
	auto curEntity = this->getEntity();
	for (const auto& item : game.getAll<si::model::PhysicsEntity>())
	{
		if (item != curEntity && curEntity->overlaps(*item))
		{
			this->collisionTargets.push_back(item);
		}
	}
}
